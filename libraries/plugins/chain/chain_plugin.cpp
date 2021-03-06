#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/chain/database_exceptions.hpp>
#include <xgt/chain/transaction_object.hpp>

#include <xgt/plugins/chain/abstract_block_producer.hpp>
#include <xgt/plugins/chain/chain_plugin.hpp>
#include <xgt/plugins/chain/statefile/statefile.hpp>

#include <xgt/utilities/benchmark_dumper.hpp>
#include <xgt/utilities/database_configuration.hpp>

#include <fc/string.hpp>
#include <fc/io/json.hpp>
#include <fc/io/fstream.hpp>

#include <boost/optional.hpp>
#include <boost/bind.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/thread/future.hpp>
#include <boost/thread/sync_queue.hpp>

#include <thread>
#include <memory>
#include <iostream>
#include <future>

namespace xgt { namespace plugins { namespace chain {

using namespace xgt;
using fc::flat_map;
using xgt::chain::block_id_type;

#define NUM_THREADS 1

struct generate_block_request
{
   generate_block_request(
      const fc::time_point_sec w,
      const wallet_name_type& wo,
      const fc::ecc::private_key& priv_key,
      fc::optional< xgt::chain::signed_transaction > br,
      uint32_t s
   ) :
      when( w ),
      witness_recovery( wo ),
      block_signing_private_key( priv_key ),
      block_reward( br ), 
      skip( s ) {}

   const fc::time_point_sec when;
   const wallet_name_type& witness_recovery;
   const fc::ecc::private_key& block_signing_private_key;
   fc::optional< xgt::chain::signed_transaction > block_reward;
   uint32_t skip;
   signed_block block;
};

typedef fc::static_variant< const signed_block*, const signed_transaction*, generate_block_request* > write_request_ptr;
typedef fc::static_variant< boost::promise< void >*, fc::future< void >* > promise_ptr;

struct write_context
{
   write_request_ptr             req_ptr;
   uint32_t                      skip = 0;
   bool                          shutdown = false;
   bool                          success = true;
   fc::optional< fc::exception > except;
   promise_ptr                   prom_ptr;
};

namespace detail {

class chain_plugin_impl
{
   public:
      chain_plugin_impl() : write_queue() {}
      ~chain_plugin_impl() { stop_write_processing(); }

      void start_write_processing();
      void stop_write_processing();
      void write_default_database_config( bfs::path& p );

      void post_block( const block_notification& note );

      uint32_t                         chainbase_flags = 0;
      bfs::path                        blockchain_dir;
      bool                             replay = false;
      bool                             resync   = false;
      bool                             readonly = false;
      bool                             validate_invariants = false;
      bool                             dump_memory_details = false;
      bool                             benchmark_is_enabled = false;
      uint32_t                         stop_at_block = 0;
      uint32_t                         benchmark_interval = 0;
      uint32_t                         flush_interval = 0;
      flat_map<uint32_t,block_id_type> loaded_checkpoints;
      std::string                      from_state = "";
      std::string                      to_state = "";
      statefile::state_format_info     state_format;

      uint32_t allow_future_time = 5;

      std::shared_ptr< std::thread >   write_processor_thread;
      boost::concurrent::sync_queue< write_context* > write_queue;

      flat_map< string, fc::variant_object > plugin_state_opts;
      bfs::path                        database_cfg;

      database  db;
      std::string block_generator_registrant;
      std::shared_ptr< abstract_block_producer > block_generator;

      boost::signals2::connection      _post_apply_block_conn;
};

struct write_request_visitor
{
   write_request_visitor() {}

   database* db;
   uint32_t  skip = 0;
   fc::optional< fc::exception >* except;
   std::shared_ptr< abstract_block_producer > block_generator;

   typedef bool result_type;

   bool operator()( const signed_block* block )
   {
      bool result = false;

      try
      {
         result = db->push_block( *block, skip );
      }
      catch( const fc::exception& e )
      {
         *except = e;
      }
      catch( ... )
      {
         *except = fc::unhandled_exception( FC_LOG_MESSAGE( warn, "Unexpected exception while pushing block." ),
                                           std::current_exception() );
      }

      return result;
   }

   bool operator()( const signed_transaction* trx )
   {
      bool result = false;

      try
      {
         db->push_transaction( *trx );

         result = true;
      }
      catch( const fc::exception& e )
      {
         *except = e;
      }
      catch( ... )
      {
         *except = fc::unhandled_exception( FC_LOG_MESSAGE( warn, "Unexpected exception while pushing block." ),
                                           std::current_exception() );
      }

      return result;
   }

   bool operator()( generate_block_request* req )
   {
      bool result = false;

      try
      {
         if( !block_generator )
            FC_THROW_EXCEPTION( chain_exception, "Received a generate block request, but no block generator has been registered." );

         req->block = block_generator->generate_block(
            req->when,
            req->witness_recovery,
            req->block_signing_private_key,
            fc::optional< xgt::chain::signed_transaction >(req->block_reward),
            req->skip
            );

         result = true;
      }
      catch( const fc::exception& e )
      {
         *except = e;
      }
      catch( ... )
      {
         *except = fc::unhandled_exception( FC_LOG_MESSAGE( warn, "Unexpected exception while pushing block." ),
                                           std::current_exception() );
      }

      return result;
   }
};

struct request_promise_visitor
{
   request_promise_visitor(){}

   typedef void result_type;

   template< typename T >
   void operator()( T* t )
   {
      t->set_value();
   }
};

void chain_plugin_impl::start_write_processing()
{
   write_processor_thread = std::make_shared< std::thread >( [&]()
   {
      write_context* cxt;
      write_request_visitor req_visitor;
      req_visitor.db = &db;
      req_visitor.block_generator = block_generator;

      request_promise_visitor prom_visitor;

      while( true )
      {
         write_queue >> cxt;

         if (cxt->shutdown)
            break;

         req_visitor.skip = cxt->skip;
         req_visitor.except = &(cxt->except);

         db.with_write_lock( [&]()
         {
            cxt->success = cxt->req_ptr.visit( req_visitor );
            cxt->prom_ptr.visit( prom_visitor );
         });
      }
   });
}

void chain_plugin_impl::stop_write_processing()
{
   auto shutdown_msg = write_context{.shutdown = true};
   write_queue << &shutdown_msg;

   if( write_processor_thread )
      write_processor_thread->join();

   write_processor_thread.reset();
}

void chain_plugin_impl::write_default_database_config( bfs::path &p )
{
   ilog( "writing database configuration: ${p}", ("p", p.string()) );
   fc::json::save_to_file( xgt::utilities::default_database_configuration(), p );
}

void chain_plugin_impl::post_block( const block_notification& note )
{
   if( db.get_dynamic_global_properties().last_irreversible_block_num >= stop_at_block )
   {
      stop_write_processing();
      std::async( std::launch::async, [&]{ app().quit(); } );
   }
}

} // detail


chain_plugin::chain_plugin() : my( new detail::chain_plugin_impl() ) {}
chain_plugin::~chain_plugin(){}

database& chain_plugin::db() { return my->db; }
const xgt::chain::database& chain_plugin::db() const { return my->db; }

bfs::path chain_plugin::state_storage_dir() const
{
   return my->blockchain_dir;
}

void chain_plugin::set_program_options(options_description& cli, options_description& cfg)
{
   cfg.add_options()
         ("blockchain-dir", bpo::value<bfs::path>()->default_value("blockchain"),
            "the location of the blockchain data files (absolute path or relative to application data dir)")
         ("checkpoint,c", bpo::value<vector<string>>()->composing(), "Pairs of [BLOCK_NUM,BLOCK_ID] that should be enforced as checkpoints.")
         ("flush-state-interval", bpo::value<uint32_t>(),
            "flush shared memory changes to disk every N blocks")
         ("from-state", bpo::value<string>()->default_value(""), "Load from state, then replay subsequent blocks")
         ("to-state", bpo::value<string>()->default_value(""), "File to save state to on shutdown")
         ("state-format", bpo::value<string>()->default_value("binary"), "State file save format (binary|json)")
         ("memory-replay-indices", bpo::value<vector<string>>()->multitoken()->composing(), "Specify which indices should be in memory during replay")
         ;
   cli.add_options()
         ("replay-blockchain", bpo::bool_switch()->default_value(false), "clear chain database and replay all blocks")
         ("force-open", bpo::bool_switch()->default_value(false), "force open the database, skipping the environment check")
         ("resync-blockchain", bpo::bool_switch()->default_value(false), "clear chain database and block log")
         ("stop-at-block", bpo::value<uint32_t>(), "Stop and exit after reaching given block number")
         ("advanced-benchmark", "Make profiling for every plugin.")
         ("set-benchmark-interval", bpo::value<uint32_t>(), "Print time and memory usage every given number of blocks")
         ("dump-memory-details", bpo::bool_switch()->default_value(false), "Dump database objects memory usage info. Use set-benchmark-interval to set dump interval.")
         ("validate-database-invariants", bpo::bool_switch()->default_value(false), "Validate all supply invariants check out")
         ("database-cfg", bpo::value<bfs::path>()->default_value("database.cfg"), "The database configuration file location")
         ("memory-replay,m", bpo::bool_switch()->default_value(false), "Replay with state in memory instead of on disk")
         ("chain-id", bpo::value< std::string >()->default_value( XGT_CHAIN_ID ), "chain ID to connect to")
         ;
}

void chain_plugin::plugin_initialize(const variables_map& options)
{
   my->blockchain_dir = app().data_dir() / "blockchain";

   if( options.count("blockchain-dir") )
   {
      auto sfd = options.at("blockchain-dir").as<bfs::path>();
      if(sfd.is_relative())
         my->blockchain_dir = app().data_dir() / sfd;
      else
         my->blockchain_dir = sfd;
   }

   my->chainbase_flags |= options.at( "force-open" ).as< bool >() ? chainbase::skip_env_check : chainbase::skip_nothing;

   my->from_state          = options.at( "from-state" ).as<string>();
   my->to_state            = options.at( "to-state" ).as<string>();
   my->replay              = options.at( "replay-blockchain").as<bool>();
   my->resync              = options.at( "resync-blockchain").as<bool>();
   my->stop_at_block      =
      options.count( "stop-at-block" ) ? options.at( "stop-at-block" ).as<uint32_t>() : 0;
   my->benchmark_interval  =
      options.count( "set-benchmark-interval" ) ? options.at( "set-benchmark-interval" ).as<uint32_t>() : 0;
   my->validate_invariants = options.at( "validate-database-invariants" ).as<bool>();
   my->dump_memory_details = options.at( "dump-memory-details" ).as<bool>();
   if( options.count( "flush-state-interval" ) )
      my->flush_interval = options.at( "flush-state-interval" ).as<uint32_t>();
   else
      my->flush_interval = 10000;

   if( options.at( "state-format" ).as<string>() == "binary" )
   {
      my->state_format.is_binary = true;
   }
   else if( options.at( "state-format" ).as<string>() == "json" )
   {
      my->state_format.is_binary = false;
   }
   else
   {
      FC_ASSERT( false, "Unknown state format ${f}", ("f", options.at("state-format").as<string>()) );
   }

   if(options.count("checkpoint"))
   {
      auto cps = options.at("checkpoint").as<vector<string>>();
      my->loaded_checkpoints.reserve(cps.size());
      for(const auto& cp : cps)
      {
         auto item = fc::json::from_string(cp).as<std::pair<uint32_t,block_id_type>>();
         my->loaded_checkpoints[item.first] = item.second;
      }
   }

   my->benchmark_is_enabled = (options.count( "advanced-benchmark" ) != 0);

   my->database_cfg = options.at( "database-cfg" ).as< bfs::path >();

   if( my->database_cfg.is_relative() )
      my->database_cfg = app().data_dir() / my->database_cfg;

   if( !bfs::exists( my->database_cfg ) )
   {
      my->write_default_database_config( my->database_cfg );
   }

   if( options.count( "chain-id" ) )
   {
      auto chain_id_str = options.at("chain-id").as< std::string >();

      try
      {
         my->db.set_chain_id( chain_id_type( chain_id_str) );
      }
      catch( const fc::exception& )
      {
         FC_ASSERT( false, "Could not parse chain_id as hex string. Chain ID String: ${s}", ("s", chain_id_str) );
      }
   }
}

#define BENCHMARK_FILE_NAME "replay_benchmark.json"

void chain_plugin::plugin_startup()
{
   if(my->resync)
   {
      wlog("resync requested: deleting block log and shared memory");
      my->db.wipe( app().data_dir() / "blockchain", my->blockchain_dir, true );
   }

   my->db.set_flush_interval( my->flush_interval );
   my->db.add_checkpoints( my->loaded_checkpoints );

   bool dump_memory_details = my->dump_memory_details;
   xgt::utilities::benchmark_dumper dumper;

   const auto& abstract_index_cntr = my->db.get_abstract_index_cntr();

   typedef xgt::utilities::benchmark_dumper::index_memory_details_cntr_t index_memory_details_cntr_t;
   auto get_indexes_memory_details = [dump_memory_details, &abstract_index_cntr]
      (index_memory_details_cntr_t& index_memory_details_cntr, bool onlyStaticInfo)
   {
      if (dump_memory_details == false)
         return;

      for (auto idx : abstract_index_cntr)
      {
         auto info = idx->get_statistics(onlyStaticInfo);
         index_memory_details_cntr.emplace_back(std::move(info._value_type_name), info._item_count,
            info._item_sizeof, info._item_additional_allocation, info._additional_container_allocation);
      }
   };

   fc::variant database_config;

   try
   {
      database_config = fc::json::from_file( my->database_cfg, fc::json::strict_parser );
   }
   catch ( const std::exception& e )
   {
      elog( "Error while parsing database configuration: ${e}", ("e", e.what()) );
      exit( EXIT_FAILURE );
   }
   catch ( const fc::exception& e )
   {
      elog( "Error while parsing database configuration: ${e}", ("e", e.what()) );
      exit( EXIT_FAILURE );
   }

   database::open_args db_open_args;
   db_open_args.data_dir = app().data_dir() / "blockchain";
   db_open_args.blockchain_dir = my->blockchain_dir;
   db_open_args.initial_supply = XGT_INIT_SUPPLY;
   db_open_args.chainbase_flags = my->chainbase_flags;
   db_open_args.do_validate_invariants = my->validate_invariants;
   db_open_args.stop_at_block = my->stop_at_block;
   db_open_args.benchmark_is_enabled = my->benchmark_is_enabled;
   db_open_args.database_cfg = database_config;

   auto benchmark_lambda = [&dumper, &get_indexes_memory_details, dump_memory_details] ( uint32_t current_block_number,
      const chainbase::database::abstract_index_cntr_t& abstract_index_cntr )
   {
      if( current_block_number == 0 ) // initial call
      {
         typedef xgt::utilities::benchmark_dumper::database_object_sizeof_cntr_t database_object_sizeof_cntr_t;
         auto get_database_objects_sizeofs = [dump_memory_details, &abstract_index_cntr]
            (database_object_sizeof_cntr_t& database_object_sizeof_cntr)
         {
            if (dump_memory_details == false)
               return;

            for (auto idx : abstract_index_cntr)
            {
               auto info = idx->get_statistics(true);
               database_object_sizeof_cntr.emplace_back(std::move(info._value_type_name), info._item_sizeof);
            }
         };

         dumper.initialize(get_database_objects_sizeofs, BENCHMARK_FILE_NAME);
         return;
      }

      const xgt::utilities::benchmark_dumper::measurement& measure =
         dumper.measure(current_block_number, get_indexes_memory_details);
      ilog( "Performance report at block ${n}. Elapsed time: ${rt} ms (real), ${ct} ms (cpu). Memory usage: ${cm} (current), ${pm} (peak) kilobytes.",
         ("n", current_block_number)
         ("rt", measure.real_ms)
         ("ct", measure.cpu_ms)
         ("cm", measure.current_mem)
         ("pm", measure.peak_mem) );
   };

   if(my->replay || (my->from_state != ""))
   {
      ilog("Replaying blockchain on user request.");
      db_open_args.benchmark = xgt::chain::database::TBenchmark(my->benchmark_interval, benchmark_lambda);
      if( my->from_state != "" )
      {
         db_open_args.genesis_func = std::make_shared< std::function<void( database&, const database::open_args& )> >( [&]( database& db, const database::open_args& args )
         {
            statefile::init_genesis_from_state( db, ( app().data_dir() / my->from_state ).string(), args.blockchain_dir, args.database_cfg );
         } );
      }
      uint32_t last_block_number = my->db.reindex( db_open_args );

      if( my->benchmark_interval > 0 )
      {
         const xgt::utilities::benchmark_dumper::measurement& total_data = dumper.dump(true, get_indexes_memory_details);
         ilog( "Performance report (total). Blocks: ${b}. Elapsed time: ${rt} ms (real), ${ct} ms (cpu). Memory usage: ${cm} (current), ${pm} (peak) kilobytes.",
               ("b", total_data.block_number)
               ("rt", total_data.real_ms)
               ("ct", total_data.cpu_ms)
               ("cm", total_data.current_mem)
               ("pm", total_data.peak_mem) );
      }

      if( my->stop_at_block > 0 && my->stop_at_block <= last_block_number )
      {
         ilog("Stopped blockchain replaying on user request. Last applied block number: ${n}.", ("n", last_block_number));
         if( my->to_state != "" )
         {
            ilog( "Saving blockchain state" );
            auto result = statefile::write_state( my->db, ( app().data_dir() / my->to_state ).string(), my->state_format );
            ilog( "Blockchain state successful, size=${n} hash=${h}", ("n", result.size)("h", result.hash) );
         }
         exit(EXIT_SUCCESS);
      }
   }
   else
   {
      db_open_args.benchmark = xgt::chain::database::TBenchmark(dump_memory_details, benchmark_lambda);

      try
      {
         ilog("Opening shared memory from ${path}", ("path",my->blockchain_dir.generic_string()));

         my->db.open( db_open_args );

         if( dump_memory_details )
            dumper.dump( true, get_indexes_memory_details );
      }
      catch( const fc::exception& e )
      {
         wlog( "Error opening database. If the binary or configuration has changed, replay the blockchain explicitly using `--replay-blockchain`." );
         wlog( "If you know what you are doing you can skip this check and force open the database using `--force-open`." );
         wlog( "WARNING: THIS MAY CORRUPT YOUR DATABASE. FORCE OPEN AT YOUR OWN RISK." );
         wlog( " Error: ${e}", ("e", e) );
         exit(EXIT_FAILURE);
      }
   }

   my->db.with_read_lock([&]() {
      ilog( "Started on blockchain with ${n} blocks", ("n", my->db.head_block_num()) );
   });
   on_sync();

   if( my->stop_at_block )
   {
      my->_post_apply_block_conn = my->db.add_post_apply_block_handler( [&]( const block_notification& note )
      { my->post_block( note ); }, *this, 10 );
   }

   my->start_write_processing();
}

void chain_plugin::plugin_shutdown()
{
   ilog("closing chain database");
   my->stop_write_processing();

   if( my->to_state != "" )
   {
      db().with_write_lock( [&]()
      {
         db().undo_all();

         //FC_TODO( "Serializing and deserializing transaction objects does not seem to be working... :/" );
         //const auto& trx_idx = db().get_index< xgt::chain::transaction_index, by_id >();
         //while( trx_idx.begin() != trx_idx.end() )
         //{
         //   db().remove( *trx_idx.begin() );
         //}

         ilog( "Saving blockchain state" );
         auto result = statefile::write_state( my->db, ( app().data_dir() / my->to_state ).string(), my->state_format );
         ilog( "Blockchain state successful, size=${n} hash=${h}", ("n", result.size)("h", result.hash) );
      });
   }

   my->db.close();
   ilog("database closed successfully");
}

void chain_plugin::report_state_options( const string& plugin_name, const fc::variant_object& opts )
{
   my->plugin_state_opts[ plugin_name ] = opts;
}

flat_map< string, fc::variant_object >& chain_plugin::get_state_options() const
{
   return my->plugin_state_opts;
}

bool chain_plugin::accept_block( const xgt::chain::signed_block& block, bool currently_syncing, uint32_t skip )
{
   if (currently_syncing && block.block_num() % 10000 == 0) {
      ilog("Syncing Blockchain --- Got block: #${n} time: ${t} producer: ${p}",
           ("t", block.timestamp)
           ("n", block.block_num())
           ("p", block.witness) );
   }
   if (!currently_syncing) {
      ilog("!!! Incoming Block --- Got block: #${n} time: ${t} producer: ${p}",
           ("t", block.timestamp)
           ("n", block.block_num())
           ("p", block.witness) );
   }

   check_time_in_block( block );

   boost::promise< void > prom;
   write_context cxt;
   cxt.req_ptr = &block;
   cxt.skip = skip;
   cxt.prom_ptr = &prom;

   my->write_queue << &cxt;

   prom.get_future().get();

   if( cxt.except ) throw *(cxt.except);

   return cxt.success;
}

void chain_plugin::accept_transaction( const xgt::chain::signed_transaction& trx )
{
   boost::promise< void > prom;
   write_context cxt;
   cxt.req_ptr = &trx;
   cxt.prom_ptr = &prom;

   my->write_queue << &cxt;

   prom.get_future().get();

   if( cxt.except ) throw *(cxt.except);

   return;
}

xgt::chain::signed_block chain_plugin::generate_block(
   const fc::time_point_sec when,
   const wallet_name_type& witness_recovery,
   const fc::ecc::private_key& block_signing_private_key,
   fc::optional< xgt::chain::signed_transaction > block_reward,
   uint32_t skip )
{
   generate_block_request req( when, witness_recovery, block_signing_private_key, block_reward, skip );
   boost::promise< void > prom;
   write_context cxt;
   cxt.req_ptr = &req;
   cxt.prom_ptr = &prom;

   my->write_queue << &cxt;

   prom.get_future().get();

   if( cxt.except ) throw *(cxt.except);

   FC_ASSERT( cxt.success, "Block could not be generated" );

   return req.block;
}

bool chain_plugin::block_is_on_preferred_chain(const xgt::chain::block_id_type& block_id )
{
   // If it's not known, it's not preferred.
   if( !db().is_known_block(block_id) ) return false;

   // Extract the block number from block_id, and fetch that block number's ID from the database.
   // If the database's block ID matches block_id, then block_id is on the preferred chain. Otherwise, it's on a fork.
   return db().get_block_id_for_num( xgt::chain::block_header::num_from_id( block_id ) ) == block_id;
}

void chain_plugin::check_time_in_block( const xgt::chain::signed_block& block )
{
   time_point_sec now = fc::time_point::now();

   uint64_t max_accept_time = now.sec_since_epoch();
   max_accept_time += my->allow_future_time;
   FC_ASSERT( block.timestamp.sec_since_epoch() <= max_accept_time );
}

void chain_plugin::register_block_generator( const std::string& plugin_name, std::shared_ptr< abstract_block_producer > block_producer )
{
   FC_ASSERT( get_state() == appbase::abstract_plugin::state::initialized, "Can only register a block generator when the chain_plugin is initialized." );

   if ( my->block_generator )
      wlog( "Overriding a previously registered block generator by: ${registrant}", ("registrant", my->block_generator_registrant) );

   my->block_generator_registrant = plugin_name;
   my->block_generator = block_producer;
}

} } } // namespace xgt::plugis::chain::chain_apis
