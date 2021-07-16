#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/plugins/witness/witness_plugin.hpp>
#include <xgt/plugins/witness/witness_plugin_objects.hpp>

#include <xgt/chain/database_exceptions.hpp>
#include <xgt/chain/wallet_object.hpp>
#include <xgt/chain/comment_object.hpp>
#include <xgt/chain/witness_objects.hpp>
#include <xgt/chain/index.hpp>
#include <xgt/chain/util/impacted.hpp>

#include <xgt/utilities/key_conversion.hpp>
#include <xgt/utilities/plugin_utilities.hpp>

#include <fc/io/json.hpp>
#include <fc/macros.hpp>
#include <fc/smart_ref_impl.hpp>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <cstdint>
#include <iostream>
#include <limits>
#include <random>


#define DISTANCE_CALC_PRECISION (10000)
#define BLOCK_PRODUCING_LAG_TIME (750)
#define BLOCK_PRODUCTION_LOOP_SLEEP_TIME (200000)


namespace xgt { namespace plugins { namespace witness {

using namespace xgt::chain;

using std::string;
using std::vector;

namespace bpo = boost::program_options;


void new_chain_banner( const chain::database& db )
{
   std::cerr << "\n"
      "********************************\n"
      "*                              *\n"
      "*   ------- NEW CHAIN ------   *\n"
      "*   -   Welcome to Xgt!  -   *\n"
      "*   ------------------------   *\n"
      "*                              *\n"
      "********************************\n"
      "\n";
   return;
}

namespace detail {

   class witness_plugin_impl {
   public:
      witness_plugin_impl( boost::asio::io_service& io ) :
         _timer(io),
         _chain_plugin( appbase::app().get_plugin< xgt::plugins::chain::chain_plugin >() ),
         _db( appbase::app().get_plugin< xgt::plugins::chain::chain_plugin >().db() ),
         _block_producer( std::make_shared< witness::block_producer >( _db ) )
         {}

      void on_post_apply_block( const chain::block_notification& note );
      void start_mining( const fc::ecc::public_key& pub, const fc::ecc::private_key& pk, const string& miner );
      void on_pre_apply_operation( const chain::operation_notification& note );
      void on_post_apply_operation( const chain::operation_notification& note );

      void schedule_production_loop();
      void block_production_loop();
      fc::optional< chain::wallet_name_type > get_witness();

      unsigned int _num_threads;
      bool     _production_enabled              = false;
      uint32_t _production_skip_flags           = chain::database::skip_nothing;

      std::map< xgt::protocol::public_key_type, fc::ecc::private_key > _private_keys;
      std::set< xgt::protocol::wallet_name_type >                      _witnesses;
      boost::asio::deadline_timer                                      _timer;

      plugins::chain::chain_plugin& _chain_plugin;
      chain::database&              _db;
      boost::signals2::connection   _post_apply_block_conn;
      boost::signals2::connection   _pre_apply_operation_conn;
      boost::signals2::connection   _post_apply_operation_conn;
      boost::signals2::connection   _chain_sync;

      std::shared_ptr< witness::block_producer >                       _block_producer;

      bool _is_braking = false;
      std::vector<std::shared_ptr<fc::thread>> _threads;
      std::map<chain::public_key_type, fc::ecc::private_key> _mining_private_keys;
      xgt::chain::legacy_chain_properties _miner_prop_vote;
      uint64_t _head_block_num = 0;
      block_id_type _head_block_id = block_id_type();
      uint64_t _total_hashes = 0;
      fc::time_point _hash_start_time;
   };

   void check_memo( const string& memo, const chain::wallet_object& account, const account_authority_object& auth )
   {
      vector< public_key_type > keys;

      try
      {
         // Check if memo is a private key
         keys.push_back( fc::ecc::extended_private_key::from_base58( memo ).get_public_key() );
      }
      catch( fc::parse_error_exception& ) {}
      catch( fc::assert_exception& ) {}

      // Get possible keys if memo was an account password
      string recovery_seed = account.name + "recovery" + memo;
      auto recovery_secret = fc::sha256::hash( recovery_seed.c_str(), recovery_seed.size() );
      keys.push_back( fc::ecc::private_key::regenerate( recovery_secret ).get_public_key() );

      string money_seed = account.name + "money" + memo;
      auto money_secret = fc::sha256::hash( money_seed.c_str(), money_seed.size() );
      keys.push_back( fc::ecc::private_key::regenerate( money_secret ).get_public_key() );

      string social_seed = account.name + "social" + memo;
      auto social_secret = fc::sha256::hash( social_seed.c_str(), social_seed.size() );
      keys.push_back( fc::ecc::private_key::regenerate( social_secret ).get_public_key() );

      // Check keys against public keys in authorites
      for( auto& key_weight_pair : auth.recovery.key_auths )
      {
         for( auto& key : keys )
            XGT_ASSERT( key_weight_pair.first != key,  plugin_exception,
               "Detected private recovery key in memo field. You should change your recovery keys." );
      }

      for( auto& key_weight_pair : auth.money.key_auths )
      {
         for( auto& key : keys )
            XGT_ASSERT( key_weight_pair.first != key,  plugin_exception,
               "Detected private money key in memo field. You should change your money keys." );
      }

      for( auto& key_weight_pair : auth.social.key_auths )
      {
         for( auto& key : keys )
            XGT_ASSERT( key_weight_pair.first != key,  plugin_exception,
               "Detected private social key in memo field. You should change your social keys." );
      }

      const auto& memo_key = account.memo_key;
      for( auto& key : keys )
         XGT_ASSERT( memo_key != key,  plugin_exception,
            "Detected private memo key in memo field. You should change your memo key." );
   }

   struct operation_visitor
   {
      operation_visitor( const chain::database& db ) : _db( db ) {}

      const chain::database& _db;

      typedef void result_type;

      template< typename T >
      void operator()( const T& )const {}

      void operator()( const transfer_operation& o )const
      {
         if( o.memo.length() > 0 )
            check_memo( o.memo,
                        _db.get< chain::wallet_object, chain::by_name >( o.from ),
                        _db.get< account_authority_object, chain::by_account >( o.from ) );
      }
   };

   void witness_plugin_impl::on_pre_apply_operation( const chain::operation_notification& note )
   {
      if( _db.is_producing() )
      {
         note.op.visit( operation_visitor( _db ) );
      }
   }

   void witness_plugin_impl::on_post_apply_operation( const chain::operation_notification& note )
   {
      switch( note.op.which() )
      {
         case operation::tag< custom_operation >::value:
         case operation::tag< custom_json_operation >::value:
            if( _db.is_producing() )
            {
               flat_set< wallet_name_type > impacted;
               app::operation_get_impacted_accounts( note.op, impacted );

               for( const wallet_name_type& account : impacted )
               {
                  // Possible alternative implementation:  Don't call find(), simply catch
                  // the exception thrown by db.create() when violating uniqueness (std::logic_error).
                  //
                  // This alternative implementation isn't "idiomatic" (i.e. AFAICT no existing
                  // code uses this approach).  However, it may improve performance.

                  const witness_custom_op_object* coo = _db.find< witness_custom_op_object, by_account >( account );

                  if( !coo )
                  {
                     _db.create< witness_custom_op_object >( [&]( witness_custom_op_object& o )
                     {
                        o.account = account;
                        o.count = 1;
                     });
                  }
                  else
                  {
                     XGT_ASSERT( coo->count < WITNESS_CUSTOM_OP_BLOCK_LIMIT, plugin_exception,
                        "Account ${a} already submitted ${n} custom json operation(s) this block.",
                        ("a", account)("n", WITNESS_CUSTOM_OP_BLOCK_LIMIT) );

                     _db.modify( *coo, [&]( witness_custom_op_object& o )
                     {
                        o.count++;
                     });
                  }
               }
            }

            break;
         default:
            break;
      }
   }

   void witness_plugin_impl::on_post_apply_block( const block_notification& note )
   {
      // Clear out any processed custom operations
      const auto& idx = _db.get_index< witness_custom_op_index >().indices().get< by_id >();
      while( true )
      {
         auto it = idx.begin();
         if( it == idx.end() )
            break;
         _db.remove( *it );
      }
   }

   // mine_rounds performs work in work for miner solving for block_id with target and returns the number of rounds attempted.
   uint64_t mine_rounds(std::shared_ptr<protocol::sha2_pow> work, uint32_t target, uint64_t rounds) {
      thread_local uint64_t nonce = std::hash<std::thread::id>()(std::this_thread::get_id());
      uint64_t done = 0;
      for (; done != rounds; ++done) {
         work->update(++nonce);
         if (work->pow_summary < target && work->is_valid()) {
            wlog("Miner found a solution with ${n}!", ("n", nonce));
            break;
         }
      }
      return done;
   }

   void witness_plugin_impl::start_mining( const fc::ecc::public_key& pub, const fc::ecc::private_key& pk, const string& miner )
   {
      auto block_id = _db.head_block_id();
      auto block_num = _db.head_block_num();
      auto head_block_time = _db.head_block_time();
      uint32_t target = _db.get_pow_summary_target();
      wlog( "Miner has started work ${o} at block ${b} target ${t}", ("o", miner)("b", block_num)("t", target));

      _total_hashes = 0;
      _hash_start_time = fc::time_point::now();
      const auto& acct_idx  = _db.get_index< chain::wallet_index >().indices().get< chain::by_name >();
      auto acct_it = acct_idx.find( miner );
      bool has_account = (acct_it != acct_idx.end());

      std::vector<std::shared_ptr<protocol::sha2_pow>> works(_num_threads);
      for (auto& work : works) {
         work = std::make_shared<protocol::sha2_pow>();
         work->init(block_id, miner);
      }

      std::vector<fc::future<uint64_t>> tasks(_num_threads);

      while (!this->_is_braking)
      {
         for(size_t i = 0; i < _num_threads; i++) {
            auto work = works[i];
            tasks[i] = this->_threads[i]->async([=]() { return mine_rounds(work, target, 50000); });
         }

         for(auto f : tasks) {
            this->_total_hashes += f.wait();
         }

         for(auto& work : works) {
            if (work->pow_summary < target && work->is_valid()) {
               work->prev_block = block_id;

               protocol::pow_operation op;
               op.props = _miner_prop_vote;
               op.work = *work;
               if( !has_account )
                  op.new_recovery_key = pub;

               protocol::signed_transaction trx;
               trx.operations.push_back( op );
               trx.ref_block_num = block_num;
               trx.ref_block_prefix = work->input.prev_block._hash[1];
               fc::time_point_sec now_sec = fc::time_point::now();
               trx.set_expiration( now_sec + XGT_MAX_TIME_UNTIL_EXPIRATION );
               trx.sign( pk, XGT_CHAIN_ID, fc::ecc::fc_canonical );

               wlog( "Broadcasting..." );
               try
               {
                  wlog("Mined block proceeding #${n} with timestamp ${t} at time ${c}", ("n", block_num)("t", head_block_time)("c", fc::time_point::now()));
                  fc::time_point now = fc::time_point::now();
                  auto block = _chain_plugin.generate_block( now, miner, pk, _production_skip_flags);
                  _db.push_block(block, (uint32_t)0);
                  appbase::app().get_plugin< xgt::plugins::p2p::p2p_plugin >().broadcast_block( block );

                  wlog( "Broadcasting Proof of Work for ${miner}", ("miner", miner) );
                  _db.push_transaction( trx );
                  appbase::app().get_plugin< xgt::plugins::p2p::p2p_plugin >().broadcast_transaction( trx );

                  ++this->_head_block_num;
                  wlog( "Broadcast succeeded!" );
               }
               catch( const fc::exception& e )
               {
                  wlog( "Broadcast failed!" );
                  wdump((e.to_detail_string()));
               }
               schedule_production_loop();
               return;
            }
         }

         if (this->_total_hashes % 1000000 == 0) {
            uint64_t micros = (fc::time_point::now() - _hash_start_time).count();
            uint64_t hashrate = (this->_total_hashes * 1000000) / micros;
            wlog("Miner working at block ${b} rate: ${r}H/s", ("b", block_num)("r",hashrate));
         }

         auto head_block_num = _db.head_block_num();
         if( this->_head_block_num != head_block_num )
         {
            wlog( "Stop mining due new block arrival. Working at ${o}. New block ${p}", ("o",this->_head_block_num)("p",head_block_num) );
            this->_head_block_num = head_block_num;
            break;
         }
         if (this->_is_braking) {
            break;
         }
      }
      if (!this->_is_braking)
      {
         schedule_production_loop();
      }
   }

   void witness_plugin_impl::schedule_production_loop() {
      if (_db.head_block_num() != 0 && !appbase::app().get_plugin< xgt::plugins::p2p::p2p_plugin >().ready_to_mine()) {
         _timer.expires_from_now( boost::posix_time::milliseconds( 1000 ) );
         _timer.async_wait( boost::bind( &witness_plugin_impl::schedule_production_loop, this ) );
         return;
      }
      _timer.expires_from_now( boost::posix_time::milliseconds( 10 ) );
      _timer.async_wait( boost::bind( &witness_plugin_impl::block_production_loop, this ) );
   }

   void witness_plugin_impl::block_production_loop()
   {
      _is_braking = false;

      fc::time_point now = fc::time_point::now();

      if( now < fc::time_point(XGT_GENESIS_TIME) )
      {
         wlog( "Waiting until genesis time to produce block: ${t}", ("t",XGT_GENESIS_TIME) );
         schedule_production_loop();
         return;
      }

      if( _witnesses.size() == 0 || _private_keys.size() == 0)
      {
         //wlog( "Either no witness set, no private key set, or both" );
         return;
      }

      auto name_ptr = _witnesses.begin();

      if ( _db.head_block_num() == 0 )
      {
         if (*name_ptr == XGT_INIT_MINER_NAME)
         {
            wlog("Generating genesis block...");

            auto pair = _private_keys.begin();
            auto block = _chain_plugin.generate_block( now, XGT_INIT_MINER_NAME, pair->second, _production_skip_flags );
            _db.push_block(block, (uint32_t)0);
            this->_head_block_num++;
            schedule_production_loop();
            return;
         }
         schedule_production_loop();
         return;
      }

      const hardfork_property_object& hfp = _db.get_hardfork_property_object();
      uint64_t processed_hardforks = hfp.processed_hardforks.size();
      uint64_t num_hardforks = (XGT_NUM_HARDFORKS > 0) ? (XGT_NUM_HARDFORKS - 1) : 0;
      if ( processed_hardforks < num_hardforks )
      {
         schedule_production_loop();
         return;
      }

      try
      {
         _db.get< chain::witness_object, chain::by_name >(*name_ptr);
      }
      catch (const std::exception& e)
      {
         wlog("Witness does not exist yet, bailing!");
         schedule_production_loop();
         return;
      }

      fc::mutable_variant_object capture;
      try
      {
         auto pair = _private_keys.begin();
         start_mining(pair->first, pair->second, *name_ptr);
      }
      catch( const fc::canceled_exception& )
      {
         // We're trying to exit. Go ahead and let this one out.
         throw;
      }
      catch( const chain::unknown_hardfork_exception& e )
      {
         // Hit a hardfork that the current node know nothing about, stop production and inform user
         elog( "${e}\nNode may be out of date...", ("e", e.to_detail_string()) );
         throw;
      }
      catch( const fc::exception& e )
      {
         elog("Got exception while generating block:\n${e}", ("e", e.to_detail_string()));
         throw;
      }
   }

   fc::optional< chain::wallet_name_type > witness_plugin_impl::get_witness()
   {
      for (chain::wallet_name_type name : _witnesses)
      {
         return fc::optional< chain::wallet_name_type >(name);
      }
      return fc::optional< chain::wallet_name_type >();
   }
} // detail


witness_plugin::witness_plugin() {}
witness_plugin::~witness_plugin() {}

void witness_plugin::set_program_options(
   boost::program_options::options_description& cli,
   boost::program_options::options_description& cfg)
{
   string witness_id_example = "initwitness";
   cfg.add_options()
         ("enable-stale-production", bpo::value<bool>()->default_value( false ), "Enable block production, even if the chain is stale.")
         ("witness,w", bpo::value<vector<string>>()->composing()->multitoken(),
            ("name of witness controlled by this node (e.g. " + witness_id_example + " )" ).c_str() )
         ("private-key", bpo::value<vector<string>>()->composing()->multitoken(), "WIF PRIVATE KEY to be used by one or more witnesses or miners" )
         ("mining-reward-key", bpo::value<vector<string>>()->composing()->multitoken(), "WIF PRIVATE KEY to be used by one or more witnesses or miners" )
         ("miner-account-creation-fee", bpo::value<uint64_t>()->implicit_value(100000),"Account creation fee to be voted on upon successful POW - Minimum fee is 100.000 XGT (written as 100000)")
         ("miner-maximum-block-size", bpo::value<uint32_t>()->implicit_value(131072),"Maximum block size (in bytes) to be voted on upon successful POW - Max block size must be between 128 KB and 750 MB")
         ("mining-threads", bpo::value<unsigned int>()->default_value(std::thread::hardware_concurrency()), "Number of mining threads to run (default: number of hardware threads)");
         ;
   cli.add_options()
         ("enable-stale-production", bpo::bool_switch()->default_value( false ), "Enable block production, even if the chain is stale.")
         ("mining-threads", bpo::value<unsigned int>()->default_value(std::thread::hardware_concurrency()), "Number of mining threads to run (default: number of hardware threads)");
         ;
}

void witness_plugin::plugin_initialize(const boost::program_options::variables_map& options)
{ try {
   ilog( "Initializing witness plugin" );
   my = std::make_unique< detail::witness_plugin_impl >( appbase::app().get_io_service() );

   my->_num_threads = options.at("mining-threads").as<unsigned int>();
   if (my->_num_threads == 0) {
      my->_num_threads = std::thread::hardware_concurrency();
   }
   ilog("Mining configured with ${n} threads", ("n",my->_num_threads));

   my->_chain_plugin.register_block_generator( get_name(), my->_block_producer );

   ilog ("my->_witnesses ${h}", ("h", my->_witnesses));
   XGT_LOAD_VALUE_SET( options, "witness", my->_witnesses, xgt::protocol::wallet_name_type )

   // Set witnesses in the DB so they can be used elsewhere.
   my->_db.set_witnesses(my->_witnesses);

   my->_is_braking = false;
   my->_threads.resize(my->_num_threads);
   for (auto& thread : my->_threads) {
      thread = std::make_shared<fc::thread>();
   }

   if( options.count("miner-account-creation-fee") )
   {
      const uint64_t account_creation_fee = options["miner-account-creation-fee"].as<uint64_t>();

      if( account_creation_fee < XGT_MIN_WALLET_CREATION_FEE )
         wlog( "miner-account-creation-fee is below the minimum fee, using minimum instead" );
      else
         my->_miner_prop_vote.account_creation_fee.amount = account_creation_fee;
   }

   if( options.count( "miner-maximum-block-size" ) )
   {
      const uint32_t maximum_block_size = options["miner-maximum-block-size"].as<uint32_t>();

      if( maximum_block_size < XGT_MIN_BLOCK_SIZE_LIMIT )
         wlog( "miner-maximum-block-size is below the minimum block size limit, using default of 128 KB instead" );
      else if ( maximum_block_size > XGT_MAX_BLOCK_SIZE )
      {
         wlog( "miner-maximum-block-size is above the maximum block size limit, using maximum of 750 MB instead" );
         my->_miner_prop_vote.maximum_block_size = XGT_MAX_BLOCK_SIZE;
      }
      else
         my->_miner_prop_vote.maximum_block_size = maximum_block_size;
   }

   if( options.count("mining-reward-key") )
   {
      const std::vector<std::string> keys = options["mining-reward-key"].as<std::vector<std::string>>();
      for (const std::string& wif_key : keys )
      {
         fc::optional<fc::ecc::private_key> private_key = xgt::utilities::wif_to_key(wif_key);
         FC_ASSERT( private_key.valid(), "unable to parse private key" );
         my->_private_keys[private_key->get_public_key()] = *private_key;
      }
   }

   my->_production_enabled = options.at( "enable-stale-production" ).as< bool >();

   my->_post_apply_block_conn = my->_db.add_post_apply_block_handler(
      [&]( const chain::block_notification& note ){ my->on_post_apply_block( note ); }, *this, 0 );
   my->_pre_apply_operation_conn = my->_db.add_pre_apply_operation_handler(
      [&]( const chain::operation_notification& note ){ my->on_pre_apply_operation( note ); }, *this, 0);
   my->_post_apply_operation_conn = my->_db.add_pre_apply_operation_handler(
      [&]( const chain::operation_notification& note ){ my->on_post_apply_operation( note ); }, *this, 0);

   if( my->_witnesses.size() && my->_private_keys.size() )
      my->_chain_plugin.set_write_lock_hold_time( -1 );

   XGT_ADD_PLUGIN_INDEX(my->_db, witness_custom_op_index);

} FC_LOG_AND_RETHROW() }

void witness_plugin::plugin_startup()
{ try {
   ilog("witness plugin:  plugin_startup() begin" );
   chain::database& d = appbase::app().get_plugin< xgt::plugins::chain::chain_plugin >().db();

   if( my->_production_enabled )
   {
      if( !my->_witnesses.empty() )
      {
         ilog( "Launching block production for ${n} witnesses.", ("n", my->_witnesses.size()) );
         appbase::app().get_plugin< xgt::plugins::p2p::p2p_plugin >().set_block_production( true );

         ilog( "Block production is enabled" );
         if( d.head_block_num() == 0 )
            new_chain_banner( d );
         my->_production_skip_flags |= chain::database::skip_undo_history_check;

         plugins::chain::chain_plugin* chain = appbase::app().find_plugin< plugins::chain::chain_plugin >();
         my->_chain_sync = chain->on_sync.connect( 0, [this]() {
            my->schedule_production_loop();
         });
      } else
         elog("No witnesses configured! Please add witness IDs and private keys to configuration.");
      }
      ilog("witness plugin:  plugin_startup() end");
   } FC_CAPTURE_AND_RETHROW() }

void witness_plugin::plugin_shutdown()
{
   my->_is_braking = true;
   try
   {
      chain::util::disconnect_signal( my->_post_apply_block_conn );
      chain::util::disconnect_signal( my->_pre_apply_operation_conn );
      chain::util::disconnect_signal( my->_post_apply_operation_conn );
      my->_timer.cancel();
   }
   catch(fc::exception& e)
   {
      edump( (e.to_detail_string()) );
   }
}

} } } // xgt::plugins::witness
