#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/protocol/xgt_operations.hpp>

#include <xgt/chain/block_summary_object.hpp>
#include <xgt/chain/custom_operation_interpreter.hpp>
#include <xgt/chain/database.hpp>
#include <xgt/chain/database_exceptions.hpp>
#include <xgt/chain/db_with.hpp>
#include <xgt/chain/evaluator_registry.hpp>
#include <xgt/chain/global_property_object.hpp>
#include <xgt/chain/history_object.hpp>
#include <xgt/chain/optional_action_evaluator.hpp>
#include <xgt/chain/pending_required_action_object.hpp>
#include <xgt/chain/pending_optional_action_object.hpp>
#include <xgt/chain/required_action_evaluator.hpp>
#include <xgt/chain/xtt_objects.hpp>
#include <xgt/chain/xgt_evaluator.hpp>
#include <xgt/chain/xgt_objects.hpp>
#include <xgt/chain/transaction_object.hpp>
#include <xgt/chain/shared_db_merkle.hpp>

#include <xgt/chain/util/uint256.hpp>
#include <xgt/chain/util/rd_setup.hpp>
#include <xgt/chain/util/nai_generator.hpp>
#include <xgt/chain/util/xtt_token.hpp>

#include <fc/smart_ref_impl.hpp>
#include <fc/uint128.hpp>

#include <fc/io/fstream.hpp>

#include <boost/scope_exit.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

#include <rocksdb/perf_context.h>

#include <iostream>

#include <cstdint>
#include <functional>

namespace xgt { namespace chain {

struct object_schema_repr
{
   std::pair< uint16_t, uint16_t > space_type;
   std::string type;
};

struct operation_schema_repr
{
   std::string id;
   std::string type;
};

struct db_schema
{
   std::map< std::string, std::string > types;
   std::vector< object_schema_repr > object_types;
   std::string operation_type;
   std::vector< operation_schema_repr > custom_operation_types;
};

} }

FC_REFLECT( xgt::chain::object_schema_repr, (space_type)(type) )
FC_REFLECT( xgt::chain::operation_schema_repr, (id)(type) )
FC_REFLECT( xgt::chain::db_schema, (types)(object_types)(operation_type)(custom_operation_types) )

namespace xgt { namespace chain {

using boost::container::flat_set;

class database_impl
{
   public:
      database_impl( database& self );

      database&                                       _self;
      evaluator_registry< operation >                 _evaluator_registry;
      evaluator_registry< required_automated_action > _req_action_evaluator_registry;
      evaluator_registry< optional_automated_action > _opt_action_evaluator_registry;
};

database_impl::database_impl( database& self )
   : _self(self), _evaluator_registry(self), _req_action_evaluator_registry(self), _opt_action_evaluator_registry(self) {}

database::database()
   : _my( new database_impl(*this) ) {}

database::~database()
{
   clear_pending();
}

fc::sha256 bigint_to_hash(boost::multiprecision::uint256_t b)
{
   std::ostringstream os;
   os << std::hex << std::setw(64) << std::setfill('0') << b;
   std::string string_hash = os.str();
   return fc::sha256(string_hash);
}

boost::multiprecision::uint256_t hash_to_bigint(fc::sha256 h)
{
   std::string prefix = "0x";
   std::string string_hash = h.str();
   std::string prepended_string_hash = prefix.append(string_hash);
   return boost::multiprecision::uint256_t(prepended_string_hash);
}

#ifdef ENABLE_MIRA
void set_index_helper( database& db, mira::index_type type, const boost::filesystem::path& p, const boost::any& cfg, std::vector< std::string > indices )
{
   index_delegate_map delegates;

   if ( indices.size() > 0 )
   {
      for ( auto& index_name : indices )
      {
         if ( db.has_index_delegate( index_name ) )
            delegates[ index_name ] = db.get_index_delegate( index_name );
         else
            wlog( "Encountered an unknown index name '${name}'.", ("name", index_name) );
      }
   }
   else
   {
      delegates = db.index_delegates();
   }

   std::string type_str = type == mira::index_type::mira ? "mira" : "bmic";
   for ( auto const& delegate : delegates )
   {
      ilog( "Converting index '${name}' to ${type} type.", ("name", delegate.first)("type", type_str) );
      delegate.second.set_index_type( db, type, p, cfg );
   }
}
#endif


void database::open( const open_args& args )
{
   ilog("database::open");

   try
   {
      chainbase::database::open( args.shared_mem_dir, args.chainbase_flags, args.shared_file_size, args.database_cfg );

      initialize_indexes();
      initialize_evaluators();

      ilog("database::open setting up global property object...");

      if( !find< dynamic_global_property_object >() )
      {
         with_write_lock( [&]()
         {
            if( args.genesis_func )
            {
               FC_TODO( "Load directly in to mira instead of bmic first" );
               (*args.genesis_func)( *this, args );
            }
            else
               init_genesis( args.initial_supply );
         });
      }

      ilog("database::open setting up block log...");

      _benchmark_dumper.set_enabled( args.benchmark_is_enabled );

      _block_log.open( args.data_dir / "block_log" );

      auto log_head = _block_log.head();

      ilog("database::open setting up database...");

      // Rewind all undo state. This should return us to the state at the last irreversible block.
      with_write_lock( [&]()
      {
#ifndef ENABLE_MIRA
         undo_all();
#endif

         if( args.chainbase_flags & chainbase::skip_env_check )
         {
            set_revision( head_block_num() );
         }
         else
         {
            FC_ASSERT( revision() == head_block_num(), "Chainbase revision does not match head block num.",
               ("rev", revision())("head_block", head_block_num()) );
            if (args.do_validate_invariants)
               validate_invariants();
         }
      });

      ilog("database::open check for reindex...");

      if( head_block_num() )
      {
         auto head_block = _block_log.read_block_by_num( head_block_num() );
         // This assertion should be caught and a reindex should occur
         FC_ASSERT( head_block.valid() && head_block->id() == head_block_id(), "Chain state does not match block log. Please reindex blockchain." );

         _fork_db.start_block( *head_block );
      }

      ilog("database::open initialize versioning...");

      with_read_lock( [&]()
      {
         init_hardforks(); // Writes to local state, but reads from db
      });

      ilog("database::open initialize benchmarking...");

      if (args.benchmark.first)
      {
         args.benchmark.second(0, get_abstract_index_cntr());
         auto last_block_num = _block_log.head()->block_num();
         args.benchmark.second(last_block_num, get_abstract_index_cntr());
      }

      ilog("database::open finish opening...");

      _shared_file_full_threshold = args.shared_file_full_threshold;
      _shared_file_scale_rate = args.shared_file_scale_rate;
   }
   FC_CAPTURE_LOG_AND_RETHROW( (args.data_dir)(args.shared_mem_dir)(args.shared_file_size) )
}

uint32_t database::reindex( const open_args& args )
{
   reindex_notification note( args );

   BOOST_SCOPE_EXIT(this_,&note) {
      XGT_TRY_NOTIFY(this_->_post_reindex_signal, note);
   } BOOST_SCOPE_EXIT_END

   try
   {

      ilog( "Reindexing Blockchain" );
#ifdef ENABLE_MIRA
      initialize_indexes();
#endif

      wipe( args.data_dir, args.shared_mem_dir, false );

      auto start = fc::time_point::now();
      open( args );

      XGT_TRY_NOTIFY(_pre_reindex_signal, note);

#ifdef ENABLE_MIRA
      if( args.replay_in_memory )
      {
         ilog( "Configuring replay to use memory..." );
         set_index_helper( *this, mira::index_type::bmic, args.shared_mem_dir, args.database_cfg, args.replay_memory_indices );
      }
#endif

      _fork_db.reset();    // override effect of _fork_db.start_block() call in open()

      XGT_ASSERT( _block_log.head(), block_log_exception, "No blocks in block log. Cannot reindex an empty chain." );

      ilog( "Replaying blocks..." );

      uint64_t skip_flags =
         skip_witness_signature |
         skip_transaction_signatures |
         skip_transaction_dupe_check |
         skip_tapos_check |
         skip_merkle_check |
         skip_authority_check |
         skip_validate | /// no need to validate operations
         skip_validate_invariants |
         skip_block_log;

      idump( (head_block_num()) );

      auto last_block_num = _block_log.head()->block_num();

      if( args.stop_at_block > 0 && args.stop_at_block < last_block_num )
         last_block_num = args.stop_at_block;

      if( head_block_num() < last_block_num )
      {
         _block_log.set_locking( false );
         if( args.benchmark.first > 0 )
         {
            args.benchmark.second( 0, get_abstract_index_cntr() );
         }

         auto itr = _block_log.read_block( _block_log.get_block_pos( head_block_num() + 1 ) );

         with_write_lock( [&]()
         {
            FC_ASSERT( itr.first.block_num() == head_block_num() + 1 );

            while( itr.first.block_num() < last_block_num )
            {
               auto cur_block_num = itr.first.block_num();

               if( cur_block_num % 100000 == 0 )
               {
                  std::cerr << "   " << double( cur_block_num * 100 ) / last_block_num << "%   " << cur_block_num << " of " << last_block_num << "   (" <<
   #ifdef ENABLE_MIRA
                  get_cache_size()  << " objects cached using " << (get_cache_usage() >> 20) << "M"
   #else
                  (get_free_memory() >> 20) << "M free"
   #endif
                  << ")\n";

                  //rocksdb::SetPerfLevel(rocksdb::kEnableCount);
                  //rocksdb::get_perf_context()->Reset();
               }
               apply_block( itr.first, skip_flags );

               if( cur_block_num % 100000 == 0 )
               {
                  //std::cout << rocksdb::get_perf_context()->ToString() << std::endl;
                  if( cur_block_num % 1000000 == 0 )
                  {
                     dump_lb_call_counts();
                  }
               }

               if( (args.benchmark.first > 0) && (cur_block_num % args.benchmark.first == 0) )
                  args.benchmark.second( cur_block_num, get_abstract_index_cntr() );
               itr = _block_log.read_block( itr.second );
            }

            apply_block( itr.first, skip_flags );
            note.last_block_number = itr.first.block_num();

            set_revision( head_block_num() );
         });

         if( (args.benchmark.first > 0) && (note.last_block_number % args.benchmark.first == 0) )
            args.benchmark.second( note.last_block_number, get_abstract_index_cntr() );

         _block_log.set_locking( true );
      }

      if( _block_log.head()->block_num() )
         _fork_db.start_block( *_block_log.head() );

#ifdef ENABLE_MIRA
      if( args.replay_in_memory )
      {
         ilog( "Migrating state to disk..." );
         set_index_helper( *this, mira::index_type::mira, args.shared_mem_dir, args.database_cfg, args.replay_memory_indices );
      }
#endif

      auto end = fc::time_point::now();
      ilog( "Done reindexing, elapsed time: ${t} sec", ("t",double((end-start).count())/1000000.0 ) );

      note.reindex_success = true;

      return head_block_num();
   }
   FC_CAPTURE_AND_RETHROW( (args.data_dir)(args.shared_mem_dir) )

}

void database::wipe( const fc::path& data_dir, const fc::path& shared_mem_dir, bool include_blocks)
{
   close();
   chainbase::database::wipe( shared_mem_dir );
   if( include_blocks )
   {
      fc::remove_all( data_dir / "block_log" );
      fc::remove_all( data_dir / "block_log.index" );
   }
}

void database::close(bool rewind)
{
   try
   {
      // Since pop_block() will move tx's in the popped blocks into pending,
      // we have to clear_pending() after we're done popping to get a clean
      // DB state (issue #336).
      clear_pending();

#ifdef ENABLE_MIRA
      undo_all();
#endif

      chainbase::database::flush();
      chainbase::database::close();

      _block_log.close();

      _fork_db.reset();
   }
   FC_CAPTURE_AND_RETHROW()
}

bool database::is_known_block( const block_id_type& id )const
{ try {
   return fetch_block_by_id( id ).valid();
} FC_CAPTURE_AND_RETHROW() }

/**
 * Only return true *if* the transaction has not expired or been invalidated. If this
 * method is called with a VERY old transaction we will return false, they should
 * query things by blocks if they are that old.
 */
bool database::is_known_transaction( const transaction_id_type& id )const
{ try {
   const auto& trx_idx = get_index<transaction_index>().indices().get<by_trx_id>();
   return trx_idx.find( id ) != trx_idx.end();
} FC_CAPTURE_AND_RETHROW() }

block_id_type database::find_block_id_for_num( uint32_t block_num )const
{
   try
   {
      if( block_num == 0 )
         return block_id_type();

      // Reversible blocks are *usually* in the TAPOS buffer.  Since this
      // is the fastest check, we do it first.
      block_summary_id_type bsid = block_num & 0xFFFF;
      const block_summary_object* bs = find< block_summary_object, by_id >( bsid );
      if( bs != nullptr )
      {
         if( protocol::block_header::num_from_id(bs->block_id) == block_num )
            return bs->block_id;
      }

      // Next we query the block log.   Irreversible blocks are here.
      auto b = _block_log.read_block_by_num( block_num );
      if( b.valid() )
         return b->id();

      // Finally we query the fork DB.
      shared_ptr< fork_item > fitem = _fork_db.fetch_block_on_main_branch_by_number( block_num );
      if( fitem )
         return fitem->id;

      return block_id_type();
   }
   FC_CAPTURE_AND_RETHROW( (block_num) )
}

block_id_type database::get_block_id_for_num( uint32_t block_num )const
{
   block_id_type bid = find_block_id_for_num( block_num );
   FC_ASSERT( bid != block_id_type() );
   return bid;
}


optional<signed_block> database::fetch_block_by_id( const block_id_type& id )const
{ try {
   auto b = _fork_db.fetch_block( id );
   if( !b )
   {
      auto tmp = _block_log.read_block_by_num( protocol::block_header::num_from_id( id ) );

      if( tmp && tmp->id() == id )
         return tmp;

      tmp.reset();
      return tmp;
   }

   return b->data;
} FC_CAPTURE_AND_RETHROW() }

optional<signed_block> database::fetch_block_by_number( uint32_t block_num )const
{ try {
   optional< signed_block > b;
   shared_ptr< fork_item > fitem = _fork_db.fetch_block_on_main_branch_by_number( block_num );

   if( fitem )
      b = fitem->data;
   else
      b = _block_log.read_block_by_num( block_num );

   return b;
} FC_LOG_AND_RETHROW() }

const signed_transaction database::get_recent_transaction( const transaction_id_type& trx_id ) const
{ try {
   const auto& index = get_index<transaction_index>().indices().get<by_trx_id>();
   auto itr = index.find(trx_id);
   FC_ASSERT(itr != index.end());
   signed_transaction trx;
   fc::raw::unpack_from_buffer( itr->packed_trx, trx );
   return trx;;
} FC_CAPTURE_AND_RETHROW() }

std::vector< block_id_type > database::get_block_ids_on_fork( block_id_type head_of_fork ) const
{ try {
   pair<fork_database::branch_type, fork_database::branch_type> branches = _fork_db.fetch_branch_from(head_block_id(), head_of_fork);
   if( !((branches.first.back()->previous_id() == branches.second.back()->previous_id())) )
   {
      edump( (head_of_fork)
             (head_block_id())
             (branches.first.size())
             (branches.second.size()) );
      assert(branches.first.back()->previous_id() == branches.second.back()->previous_id());
   }
   std::vector< block_id_type > result;
   for( const item_ptr& fork_block : branches.second )
      result.emplace_back(fork_block->id);
   result.emplace_back(branches.first.back()->previous_id());
   return result;
} FC_CAPTURE_AND_RETHROW() }

chain_id_type database::get_chain_id() const
{
   return xgt_chain_id;
}

void database::set_chain_id( const chain_id_type& chain_id )
{
   xgt_chain_id = chain_id;

   idump( (xgt_chain_id) );
}

std::set< xgt::protocol::wallet_name_type > database::get_witnesses()
{
   return witnesses;
}

void database::set_witnesses( std::set< xgt::protocol::wallet_name_type > ws )
{
   witnesses = ws;

   idump( (ws) );
}

void database::foreach_block(std::function<bool(const signed_block_header&, const signed_block&)> processor) const
{
   if(!_block_log.head())
      return;

   auto itr = _block_log.read_block( 0 );
   auto last_block_num = _block_log.head()->block_num();
   signed_block_header previousBlockHeader = itr.first;
   while( itr.first.block_num() != last_block_num )
   {
      const signed_block& b = itr.first;
      if(processor(previousBlockHeader, b) == false)
         return;

      previousBlockHeader = b;
      itr = _block_log.read_block( itr.second );
   }

   processor(previousBlockHeader, itr.first);
}

void database::foreach_tx(std::function<bool(const signed_block_header&, const signed_block&,
   const signed_transaction&, uint32_t)> processor) const
{
   foreach_block([&processor](const signed_block_header& prevBlockHeader, const signed_block& block) -> bool
   {
      uint32_t txInBlock = 0;
      for( const auto& trx : block.transactions )
      {
         if(processor(prevBlockHeader, block, trx, txInBlock) == false)
            return false;
         ++txInBlock;
      }

      return true;
   }
   );
}

void database::foreach_operation(std::function<bool(const signed_block_header&,const signed_block&,
   const signed_transaction&, uint32_t, const operation&, uint16_t)> processor) const
{
   foreach_tx([&processor](const signed_block_header& prevBlockHeader, const signed_block& block,
      const signed_transaction& tx, uint32_t txInBlock) -> bool
   {
      uint16_t opInTx = 0;
      for(const auto& op : tx.operations)
      {
         if(processor(prevBlockHeader, block, tx, txInBlock, op, opInTx) == false)
            return false;
         ++opInTx;
      }

      return true;
   }
   );
}


const witness_object& database::get_witness( const wallet_name_type& name ) const
{ try {
   return get< witness_object, by_name >( name );
} FC_CAPTURE_AND_RETHROW( (name) ) }

const witness_object* database::find_witness( const wallet_name_type& name ) const
{
   return find< witness_object, by_name >( name );
}

const wallet_object& database::get_account( const wallet_name_type& name )const
{ try {
   return get< wallet_object, by_name >( name );
} FC_CAPTURE_AND_RETHROW( (name) ) }

const wallet_object* database::find_account( const wallet_name_type& name )const
{
   return find< wallet_object, by_name >( name );
}

const comment_object& database::get_comment( const wallet_name_type& author, const shared_string& permlink )const
{ try {
   return get< comment_object, by_permlink >( boost::make_tuple( author, permlink ) );
} FC_CAPTURE_AND_RETHROW( (author)(permlink) ) }

const comment_object* database::find_comment( const wallet_name_type& author, const shared_string& permlink )const
{
   return find< comment_object, by_permlink >( boost::make_tuple( author, permlink ) );
}

#ifndef ENABLE_MIRA
const comment_object& database::get_comment( const wallet_name_type& author, const string& permlink )const
{ try {
   return get< comment_object, by_permlink >( boost::make_tuple( author, permlink) );
} FC_CAPTURE_AND_RETHROW( (author)(permlink) ) }

const comment_object* database::find_comment( const wallet_name_type& author, const string& permlink )const
{
   return find< comment_object, by_permlink >( boost::make_tuple( author, permlink ) );
}
#endif

const escrow_object& database::get_escrow( const wallet_name_type& name, uint32_t escrow_id )const
{ try {
   return get< escrow_object, by_from_id >( boost::make_tuple( name, escrow_id ) );
} FC_CAPTURE_AND_RETHROW( (name)(escrow_id) ) }

const escrow_object* database::find_escrow( const wallet_name_type& name, uint32_t escrow_id )const
{
   return find< escrow_object, by_from_id >( boost::make_tuple( name, escrow_id ) );
}

/*
const contract_object& database::get_contract( const contract_hash_type& contract_hash )const
{ try {
   return get< contract_object, by_contract_hash >( contract_hash );
} FC_CAPTURE_AND_RETHROW( (contract_hash) ) }
*/

const dynamic_global_property_object&database::get_dynamic_global_properties() const
{ try {
   return get< dynamic_global_property_object >();
} FC_CAPTURE_AND_RETHROW() }

const node_property_object& database::get_node_properties() const
{
   return _node_property_object;
}

const hardfork_property_object& database::get_hardfork_property_object()const
{ try {
   return get< hardfork_property_object >();
} FC_CAPTURE_AND_RETHROW() }

uint32_t database::witness_participation_rate()const
{
   const dynamic_global_property_object& dpo = get_dynamic_global_properties();
   return uint64_t(XGT_100_PERCENT) * dpo.recent_slots_filled.popcount() / 128;
}

void database::add_checkpoints( const flat_map< uint32_t, block_id_type >& checkpts )
{
   for( const auto& i : checkpts )
      _checkpoints[i.first] = i.second;
}

bool database::before_last_checkpoint()const
{
   return (_checkpoints.size() > 0) && (_checkpoints.rbegin()->first >= head_block_num());
}

/**
 * Push block "may fail" in which case every partial change is unwound.  After
 * push block is successful the block is appended to the chain database on disk.
 *
 * @return true if we switched forks as a result of this push.
 */
bool database::push_block(const signed_block& new_block, uint32_t skip)
{
   //fc::time_point begin_time = fc::time_point::now();

   auto block_num = new_block.block_num();
   if( _checkpoints.size() && _checkpoints.rbegin()->second != block_id_type() )
   {
      auto itr = _checkpoints.find( block_num );
      if( itr != _checkpoints.end() )
         FC_ASSERT( new_block.id() == itr->second, "Block did not match checkpoint", ("checkpoint",*itr)("block_id",new_block.id()) );

      if( _checkpoints.rbegin()->first >= block_num )
         skip = skip_witness_signature
              | skip_transaction_signatures
              | skip_transaction_dupe_check
              /*| skip_fork_db Fork db cannot be skipped or else blocks will not be written out to block log */
              | skip_block_size_check
              | skip_tapos_check
              | skip_authority_check
              /* | skip_merkle_check While blockchain is being downloaded, txs need to be validated against block headers */
              | skip_undo_history_check
              | skip_validate
              | skip_validate_invariants
              ;
   }

   bool result;
   detail::with_skip_flags( *this, skip, [&]()
   {
      detail::without_pending_transactions( *this, std::move(_pending_tx), [&]()
      {
         try
         {
            result = _push_block(new_block);
         }
         FC_CAPTURE_AND_RETHROW( (new_block) )

         check_free_memory( false, new_block.block_num() );
      });
   });

   //fc::time_point end_time = fc::time_point::now();
   //fc::microseconds dt = end_time - begin_time;
   //if( ( new_block.block_num() % 10000 ) == 0 )
   //   ilog( "push_block ${b} took ${t} microseconds", ("b", new_block.block_num())("t", dt.count()) );
   return result;
}

void database::_maybe_warn_multiple_production( uint32_t height )const
{
   auto blocks = _fork_db.fetch_block_by_number( height );
   if( blocks.size() > 1 )
   {
      vector< std::pair< wallet_name_type, fc::time_point_sec > > witness_time_pairs;
      for( const auto& b : blocks )
      {
         witness_time_pairs.push_back( std::make_pair( b->data.witness, b->data.timestamp ) );
      }

      ilog( "Encountered block num collision at block ${n} due to a fork, witnesses are: ${w}", ("n", height)("w", witness_time_pairs) );
   }
   return;
}

bool database::_push_block(const signed_block& new_block)
{ try {
   uint32_t skip = get_node_properties().skip_flags;
   //uint32_t skip_undo_db = skip & skip_undo_block;

   if( !(skip&skip_fork_db) )
   {
      // XXX: Triggers "attempting to push a block that is too old"
      // libraries/plugins/witness/witness_plugin.cpp:439 database::apply_block
      shared_ptr<fork_item> new_head = _fork_db.push_block(new_block);
      _maybe_warn_multiple_production( new_head->num );

      //If the head block from the longest chain does not build off of the current head, we need to switch forks.
      if( new_head->data.previous != head_block_id() )
      {
         //If the newly pushed block is the same height as head, we get head back in new_head
         //Only switch forks if new_head is actually higher than head
         if( new_head->data.block_num() > head_block_num() )
         {
            wlog( "Switching to fork: ${id}", ("id",new_head->data.id()) );
            auto branches = _fork_db.fetch_branch_from(new_head->data.id(), head_block_id());

            // pop blocks until we hit the forked block
            while( head_block_id() != branches.second.back()->data.previous )
               pop_block();

            // push all blocks on the new fork
            for( auto ritr = branches.first.rbegin(); ritr != branches.first.rend(); ++ritr )
            {
                ilog( "Pushing blocks from fork ${n} ${id}", ("n",(*ritr)->data.block_num())("id",(*ritr)->data.id()) );
                optional<fc::exception> except;
                try
                {
                   _fork_db.set_head( *ritr );
                   auto session = start_undo_session();
                   apply_block( (*ritr)->data, skip );
                   session.push();
                }
                catch ( const fc::exception& e ) { except = e; }
                if( except )
                {
                   wlog( "Exception thrown while switching forks ${e}", ("e",except->to_detail_string() ) );
                   // remove the rest of branches.first from the fork_db, those blocks are invalid
                   while( ritr != branches.first.rend() )
                   {
                      _fork_db.remove( (*ritr)->data.id() );
                      ++ritr;
                   }

                   // pop all blocks from the bad fork
                   while( head_block_id() != branches.second.back()->data.previous )
                      pop_block();

                   // restore all blocks from the good fork
                   for( auto ritr = branches.second.rbegin(); ritr != branches.second.rend(); ++ritr )
                   {
                      _fork_db.set_head( *ritr );
                      auto session = start_undo_session();
                      apply_block( (*ritr)->data, skip );
                      session.push();
                   }
                   throw *except;
                }
            }
            return true;
         }
         else
            return false;
      }
   }

   try
   {
      ilog("Pushing new block #${n} from ${w} with timestamp ${t} at time ${c}", ("n", new_block.block_num())("w", new_block.witness)("t", new_block.timestamp)("c", fc::time_point::now()));
      auto session = start_undo_session();
      apply_block(new_block, skip);
      session.push();
   }
   catch( const fc::exception& e )
   {
      elog("Failed to push new block:\n${e}", ("e", e.to_detail_string()));
      _fork_db.remove(new_block.id());
      throw;
   }

   return false;
} FC_CAPTURE_AND_RETHROW() }

/**
 * Attempts to push the transaction into the pending queue
 *
 * When called to push a locally generated transaction, set the skip_block_size_check bit on the skip argument. This
 * will allow the transaction to be pushed even if it causes the pending block size to exceed the maximum block size.
 * Although the transaction will probably not propagate further now, as the peers are likely to have their pending
 * queues full as well, it will be kept in the queue to be propagated later when a new block flushes out the pending
 * queues.
 */
void database::push_transaction( const signed_transaction& trx, uint32_t skip )
{
   try
   {
      try
      {
         FC_ASSERT( fc::raw::pack_size(trx) <= (get_dynamic_global_properties().maximum_block_size - 256) );
         set_producing( true );
         set_pending_tx( true );
         detail::with_skip_flags( *this, skip,
            [&]()
            {
               _push_transaction( trx );
            });
         set_producing( false );
         set_pending_tx( false );
      }
      catch( ... )
      {
         set_producing( false );
         set_pending_tx( false );
         throw;
      }
   }
   FC_CAPTURE_AND_RETHROW( (trx) )
}

void database::_push_transaction( const signed_transaction& trx )
{
   // If this is the first transaction pushed after applying a block, start a new undo session.
   // This allows us to quickly rewind to the clean state of the head block, in case a new block arrives.
   if( !_pending_tx_session.valid() )
      _pending_tx_session = start_undo_session();

   // Create a temporary undo session as a child of _pending_tx_session.
   // The temporary session will be discarded by the destructor if
   // _apply_transaction fails.  If we make it to merge(), we
   // apply the changes.

   auto temp_session = start_undo_session();
   _apply_transaction( trx );
   _pending_tx.push_back( trx );

   // The transaction applied successfully. Merge its changes into the pending block session.
   temp_session.squash();
}

/**
 * Removes the most recent block from the database and
 * undoes any changes it made.
 */
void database::pop_block()
{
   try
   {
      _pending_tx_session.reset();
      auto head_id = head_block_id();

      /// save the head block so we can recover its transactions
      optional<signed_block> head_block = fetch_block_by_id( head_id );
      XGT_ASSERT( head_block.valid(), pop_empty_chain, "there are no blocks to pop" );

      _fork_db.pop_block();
      undo();

      _popped_tx.insert( _popped_tx.begin(), head_block->transactions.begin(), head_block->transactions.end() );

   }
   FC_CAPTURE_AND_RETHROW()
}

void database::clear_pending()
{
   try
   {
      assert( (_pending_tx.size() == 0) || _pending_tx_session.valid() );
      _pending_tx.clear();
      _pending_tx_session.reset();
   }
   FC_CAPTURE_AND_RETHROW()
}

void database::push_virtual_operation( const operation& op )
{
   FC_ASSERT( is_virtual_operation( op ) );
   operation_notification note = create_operation_notification( op );
   ++_current_virtual_op;
   note.virtual_op = _current_virtual_op;
   notify_pre_apply_operation( note );
   notify_post_apply_operation( note );
}

void database::pre_push_virtual_operation( const operation& op )
{
   FC_ASSERT( is_virtual_operation( op ) );
   operation_notification note = create_operation_notification( op );
   ++_current_virtual_op;
   note.virtual_op = _current_virtual_op;
   notify_pre_apply_operation( note );
}

void database::post_push_virtual_operation( const operation& op )
{
   FC_ASSERT( is_virtual_operation( op ) );
   operation_notification note = create_operation_notification( op );
   note.virtual_op = _current_virtual_op;
   notify_post_apply_operation( note );
}

void database::notify_pre_apply_operation( const operation_notification& note )
{
   XGT_TRY_NOTIFY( _pre_apply_operation_signal, note )
}

struct action_validate_visitor
{
   typedef void result_type;

   template< typename Action >
   void operator()( const Action& a )const
   {
      a.validate();
   }
};

void database::push_required_action( const required_automated_action& a, time_point_sec execution_time )
{
   time_point_sec exec_time = std::max( execution_time, head_block_time() );

   static const action_validate_visitor validate_visitor;
   a.visit( validate_visitor );

   if ( !is_pending_tx() )
   {
      create< pending_required_action_object >( [&]( pending_required_action_object& pending_action )
      {
         pending_action.action = a;
         pending_action.execution_time = exec_time;
      } );
   }
}

void database::push_required_action( const required_automated_action& a )
{
   push_required_action( a, head_block_time() );
}

void database::push_optional_action( const optional_automated_action& a, time_point_sec execution_time )
{
   time_point_sec exec_time = std::max( execution_time, head_block_time() );

   static const action_validate_visitor validate_visitor;
   a.visit( validate_visitor );

   if ( !is_pending_tx() )
   {
      create< pending_optional_action_object >( [&]( pending_optional_action_object& pending_action )
      {
         pending_action.action_hash = fc::sha256::hash( a );
         pending_action.action = a;
         pending_action.execution_time = exec_time;
      } );
   }
}

void database::push_optional_action( const optional_automated_action& a )
{
   push_optional_action( a, head_block_time() );
}

void database::notify_pre_apply_required_action( const required_action_notification& note )
{
   XGT_TRY_NOTIFY( _pre_apply_required_action_signal, note );
}

void database::notify_post_apply_required_action( const required_action_notification& note )
{
   XGT_TRY_NOTIFY( _post_apply_required_action_signal, note );
}

void database::notify_pre_apply_optional_action( const optional_action_notification& note )
{
   XGT_TRY_NOTIFY( _pre_apply_optional_action_signal, note );
}

void database::notify_post_apply_optional_action( const optional_action_notification& note )
{
   XGT_TRY_NOTIFY( _post_apply_optional_action_signal, note );
}

void database::notify_post_apply_operation( const operation_notification& note )
{
   XGT_TRY_NOTIFY( _post_apply_operation_signal, note )
}

void database::notify_pre_apply_block( const block_notification& note )
{
   XGT_TRY_NOTIFY( _pre_apply_block_signal, note )
}

void database::notify_irreversible_block( uint32_t block_num )
{
   XGT_TRY_NOTIFY( _on_irreversible_block, block_num )
}

void database::notify_post_apply_block( const block_notification& note )
{
   XGT_TRY_NOTIFY( _post_apply_block_signal, note )
}

void database::notify_pre_apply_transaction( const transaction_notification& note )
{
   XGT_TRY_NOTIFY( _pre_apply_transaction_signal, note )
}

void database::notify_post_apply_transaction( const transaction_notification& note )
{
   XGT_TRY_NOTIFY( _post_apply_transaction_signal, note )
}

void database::notify_pre_apply_custom_operation( const custom_operation_notification& note )
{
   XGT_TRY_NOTIFY( _pre_apply_custom_operation_signal, note )
}

void database::notify_post_apply_custom_operation( const custom_operation_notification& note )
{
   XGT_TRY_NOTIFY( _post_apply_custom_operation_signal, note )
}

fc::sha256 database::get_pow_target()const
{
   /// @since 1.1.1 added logic to slow mining for all miners by 64x
   /// @since 1.1.2 added logic for adjustable mining difficulty
   uint32_t head_num = head_block_num();
   if (head_num < 400000)
   {
      return fc::sha256("00000ffff0000000000000000000000000000000000000000000000000000000");
   }
   else if (head_num < 907200)
   {
      return fc::sha256("0000003fffc00000000000000000000000000000000000000000000000000000");
   }
   else
   {
      const auto& dgp = get_dynamic_global_properties();
      //wlog("database::get_pow_target ${t}", ("t",dgp.mining_target));
      return dgp.mining_target;
   }
}

uint32_t database::get_pow_summary_target()const
{
   fc::sha256 pow_target = get_pow_target();
   return pow_target.approx_log_32();
}

void database::clear_null_wallet_balance()
{
   const auto& null_account = get_account( XGT_NULL_WALLET );
   asset total_xgt( 0, XGT_SYMBOL );

   if( null_account.balance.amount > 0 )
      total_xgt += null_account.balance;

   if( total_xgt.amount.value == 0 )
      return;

   operation vop_op = clear_null_wallet_balance_operation();
   clear_null_wallet_balance_operation& vop = vop_op.get< clear_null_wallet_balance_operation >();
   if( total_xgt.amount.value > 0 )
      vop.total_cleared.push_back( total_xgt );
   pre_push_virtual_operation( vop_op );

   /////////////////////////////////////////////////////////////////////////////////////

   if( total_xgt.amount > 0 )
   {
      modify( null_account, [&]( wallet_object& a )
      {
         a.balance.amount = 0;
      });
   }

   //////////////////////////////////////////////////////////////

   if( total_xgt.amount > 0 )
      adjust_supply( -total_xgt );

   post_push_virtual_operation( vop_op );
}

void database::update_recovery_authority( const wallet_object& account, const authority& recovery_authority )
{
   if( head_block_num() >= XGT_RECOVERY_AUTH_HISTORY_TRACKING_START_BLOCK_NUM )
   {
      create< recovery_authority_history_object >( [&]( recovery_authority_history_object& hist )
      {
         hist.account = account.name;
         hist.previous_recovery_authority = get< account_authority_object, by_account >( account.name ).recovery;
         hist.last_valid_time = head_block_time();
      });
   }

   modify( get< account_authority_object, by_account >( account.name ), [&]( account_authority_object& auth )
   {
      auth.recovery = recovery_authority;
      auth.last_recovery_update = head_block_time();
   });
}


void database::account_recovery_processing()
{
   // Clear expired recovery requests
   const auto& rec_req_idx = get_index< account_recovery_request_index >().indices().get< by_expiration >();
   auto rec_req = rec_req_idx.begin();

   while( rec_req != rec_req_idx.end() && rec_req->expires <= head_block_time() )
   {
      remove( *rec_req );
      rec_req = rec_req_idx.begin();
   }

   // Clear invalid historical authorities
   const auto& hist_idx = get_index< recovery_authority_history_index >().indices(); //by id
   auto hist = hist_idx.begin();

   while( hist != hist_idx.end() && time_point_sec( hist->last_valid_time + XGT_RECOVERY_AUTH_RECOVERY_PERIOD ) < head_block_time() )
   {
      remove( *hist );
      hist = hist_idx.begin();
   }

   // Apply effective recovery_account changes
   const auto& change_req_idx = get_index< change_recovery_account_request_index >().indices().get< by_effective_date >();
   auto change_req = change_req_idx.begin();

   while( change_req != change_req_idx.end() && change_req->effective_on <= head_block_time() )
   {
      modify( get_account( change_req->account_to_recover ), [&]( wallet_object& a )
      {
         a.recovery_account = change_req->recovery_account;
      });

      remove( *change_req );
      change_req = change_req_idx.begin();
   }
}

void database::expire_escrow_ratification()
{
   const auto& escrow_idx = get_index< escrow_index >().indices().get< by_ratification_deadline >();
   auto escrow_itr = escrow_idx.lower_bound( false );

   while( escrow_itr != escrow_idx.end() && !escrow_itr->is_approved() && escrow_itr->ratification_deadline <= head_block_time() )
   {
      const auto& old_escrow = *escrow_itr;
      ++escrow_itr;

      adjust_balance( old_escrow.from, old_escrow.xgt_balance );
      adjust_balance( old_escrow.from, old_escrow.pending_fee );

      remove( old_escrow );
   }
}

time_point_sec database::head_block_time()const
{
   return get_dynamic_global_properties().time;
}

uint32_t database::head_block_num()const
{
   return get_dynamic_global_properties().head_block_number;
}

block_id_type database::head_block_id()const
{
   return get_dynamic_global_properties().head_block_id;
}

fc::optional<block_id_type> database::previous_block_id()const
{
   auto head_block = fetch_block_by_id( head_block_id() );
   if( head_block.valid() )
      return head_block->previous;
   else
      return fc::optional<block_id_type>();
}

node_property_object& database::node_properties()
{
   return _node_property_object;
}

uint32_t database::last_non_undoable_block_num() const
{
   return get_dynamic_global_properties().last_irreversible_block_num;
}

void database::initialize_evaluators()
{
   _my->_evaluator_registry.register_evaluator< vote_evaluator                           >();
   _my->_evaluator_registry.register_evaluator< comment_evaluator                        >();
   _my->_evaluator_registry.register_evaluator< comment_options_evaluator                >();
   _my->_evaluator_registry.register_evaluator< delete_comment_evaluator                 >();
   _my->_evaluator_registry.register_evaluator< transfer_evaluator                       >();
   _my->_evaluator_registry.register_evaluator< wallet_create_evaluator                  >();
   _my->_evaluator_registry.register_evaluator< wallet_update_evaluator                  >();
   _my->_evaluator_registry.register_evaluator< witness_update_evaluator                 >();
   _my->_evaluator_registry.register_evaluator< custom_evaluator                         >();
   _my->_evaluator_registry.register_evaluator< custom_json_evaluator                    >();
   _my->_evaluator_registry.register_evaluator< pow_evaluator                            >();
   _my->_evaluator_registry.register_evaluator< report_over_production_evaluator         >();
   _my->_evaluator_registry.register_evaluator< escrow_transfer_evaluator                >();
   _my->_evaluator_registry.register_evaluator< escrow_approve_evaluator                 >();
   _my->_evaluator_registry.register_evaluator< escrow_dispute_evaluator                 >();
   _my->_evaluator_registry.register_evaluator< escrow_release_evaluator                 >();
   _my->_evaluator_registry.register_evaluator< xtt_setup_evaluator                      >();
   _my->_evaluator_registry.register_evaluator< xtt_setup_ico_tier_evaluator             >();
   _my->_evaluator_registry.register_evaluator< xtt_set_setup_parameters_evaluator       >();
   _my->_evaluator_registry.register_evaluator< xtt_set_runtime_parameters_evaluator     >();
   _my->_evaluator_registry.register_evaluator< xtt_create_evaluator                     >();
   _my->_evaluator_registry.register_evaluator< xtt_contribute_evaluator                 >();
   _my->_evaluator_registry.register_evaluator< contract_create_evaluator                >();
   _my->_evaluator_registry.register_evaluator< contract_invoke_evaluator                >();

   _my->_req_action_evaluator_registry.register_evaluator< example_required_evaluator    >();
   _my->_opt_action_evaluator_registry.register_evaluator< example_optional_evaluator    >();

   _my->_req_action_evaluator_registry.register_evaluator< xtt_ico_launch_evaluator         >();
   _my->_req_action_evaluator_registry.register_evaluator< xtt_ico_evaluation_evaluator     >();
   _my->_req_action_evaluator_registry.register_evaluator< xtt_token_launch_evaluator       >();
   _my->_req_action_evaluator_registry.register_evaluator< xtt_refund_evaluator             >();
}


void database::register_custom_operation_interpreter( std::shared_ptr< custom_operation_interpreter > interpreter )
{
   FC_ASSERT( interpreter );
   bool inserted = _custom_operation_interpreters.emplace( interpreter->get_custom_id(), interpreter ).second;
   // This assert triggering means we're mis-configured (multiple registrations of custom JSON evaluator for same ID)
   FC_ASSERT( inserted );
}

std::shared_ptr< custom_operation_interpreter > database::get_custom_json_evaluator( const custom_id_type& id )
{
   auto it = _custom_operation_interpreters.find( id );
   if( it != _custom_operation_interpreters.end() )
      return it->second;
   return std::shared_ptr< custom_operation_interpreter >();
}

void initialize_core_indexes( database& db );

void database::initialize_indexes()
{
   initialize_core_indexes( *this );
   _plugin_index_signal();
}

const std::string& database::get_json_schema()const
{
   return _json_schema;
}

void database::init_genesis( uint64_t init_supply )
{
   try
   {
      struct auth_inhibitor
      {
         auth_inhibitor(database& db) : db(db), old_flags(db.node_properties().skip_flags)
         { db.node_properties().skip_flags |= skip_authority_check; }
         ~auth_inhibitor()
         { db.node_properties().skip_flags = old_flags; }
      private:
         database& db;
         uint32_t old_flags;
      } inhibitor(*this);

      // Create blockchain accounts
      public_key_type      init_public_key(XGT_INIT_PUBLIC_KEY);

      create< wallet_object >( [&]( wallet_object& a )
      {
         a.name = XGT_MINER_WALLET;
      } );
      create< account_authority_object >( [&]( account_authority_object& auth )
      {
         auth.account = XGT_MINER_WALLET;
         auth.recovery.weight_threshold = 1;
         auth.money.weight_threshold = 1;
      });

      create< wallet_object >( [&]( wallet_object& a )
      {
         a.name = XGT_INIT_MINER_NAME;
         a.memo_key = init_public_key;
         a.balance  = asset( init_supply, XGT_SYMBOL );
      } );

      create< account_authority_object >( [&]( account_authority_object& auth )
      {
         auth.account = XGT_INIT_MINER_NAME;
         auth.recovery.add_authority( init_public_key, 1 );
         auth.recovery.weight_threshold = 1;
         auth.money   = auth.recovery;
         auth.social  = auth.money;
      });

      create< witness_object >( [&]( witness_object& w )
      {
         w.owner        = XGT_INIT_MINER_NAME;
         w.signing_key  = init_public_key;
      } );

      create< wallet_object >( [&]( wallet_object& a )
      {
         a.name = XGT_NULL_WALLET;
      } );
      create< account_authority_object >( [&]( account_authority_object& auth )
      {
         auth.account = XGT_NULL_WALLET;
         auth.recovery.weight_threshold = 1;
         auth.money.weight_threshold = 1;
      });

      create< wallet_object >( [&]( wallet_object& a )
      {
         a.name = XGT_TREASURY_WALLET;
      } );

      create< wallet_object >( [&]( wallet_object& a )
      {
         a.name = XGT_TEMP_WALLET;
      } );
      create< account_authority_object >( [&]( account_authority_object& auth )
      {
         auth.account = XGT_TEMP_WALLET;
         auth.recovery.weight_threshold = 0;
         auth.money.weight_threshold = 0;
      });

      ilog( "!!!!!! Preparing to create genesis account..." );
      create< dynamic_global_property_object >( [&]( dynamic_global_property_object& p )
      {
         p.mining_target = fc::sha256(XGT_MINING_TARGET_START);
         //p.mining_target = fc::sha256("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
         p.current_witness = XGT_INIT_MINER_NAME;
         p.time = XGT_GENESIS_TIME;
         p.recent_slots_filled = fc::uint128::max_value();
         p.participation_count = 128;
         p.current_supply = asset( init_supply, XGT_SYMBOL );
         p.virtual_supply = p.current_supply;
         p.maximum_block_size = XGT_MAX_BLOCK_SIZE;
         p.reverse_auction_seconds = XGT_REVERSE_AUCTION_WINDOW_SECONDS;
         p.next_maintenance_time = XGT_GENESIS_TIME;
         p.last_budget_time = XGT_GENESIS_TIME;
      } );

      for( int i = 0; i < 0x10000; i++ )
         create< block_summary_object >( [&]( block_summary_object& ) {});
      create< hardfork_property_object >( [&](hardfork_property_object& hpo )
      {
         hpo.processed_hardforks.push_back( XGT_GENESIS_TIME );
      } );

      // TODO: XXX: Figure out how to set up rc dynamics
      // // Create witness scheduler
      // create< witness_schedule_object >( [&]( witness_schedule_object& wso )
      // {
      //    FC_TODO( "Copied from witness_schedule.cpp, do we want to abstract this to a separate function?" );
      //    wso.current_shuffled_witnesses[0] = XGT_INIT_MINER_NAME;
      //    util::rd_system_params account_subsidy_system_params;
      //    account_subsidy_system_params.resource_unit = XGT_WALLET_SUBSIDY_PRECISION;
      //    account_subsidy_system_params.decay_per_time_unit_denom_shift = XGT_RD_DECAY_DENOM_SHIFT;
      //    util::rd_user_params account_subsidy_user_params;
      //    account_subsidy_user_params.budget_per_time_unit = wso.median_props.account_subsidy_budget;
      //    account_subsidy_user_params.decay_per_time_unit = wso.median_props.account_subsidy_decay;

      //    util::rd_user_params account_subsidy_per_witness_user_params;
      //    int64_t w_budget = wso.median_props.account_subsidy_budget;
      //    w_budget = (w_budget * XGT_WITNESS_SUBSIDY_BUDGET_PERCENT) / XGT_100_PERCENT;
      //    w_budget = std::min( w_budget, int64_t(std::numeric_limits<int32_t>::max()) );
      //    uint64_t w_decay = wso.median_props.account_subsidy_decay;
      //    w_decay = (w_decay * XGT_WITNESS_SUBSIDY_DECAY_PERCENT) / XGT_100_PERCENT;
      //    w_decay = std::min( w_decay, uint64_t(std::numeric_limits<uint32_t>::max()) );

      //    account_subsidy_per_witness_user_params.budget_per_time_unit = int32_t(w_budget);
      //    account_subsidy_per_witness_user_params.decay_per_time_unit = uint32_t(w_decay);

      //    util::rd_setup_dynamics_params( account_subsidy_user_params, account_subsidy_system_params, wso.account_subsidy_rd );
      //    util::rd_setup_dynamics_params( account_subsidy_per_witness_user_params, account_subsidy_system_params, wso.account_subsidy_witness_rd );
      // } );

      create< nai_pool_object >( [&]( nai_pool_object& npo ) {} );
   }
   FC_CAPTURE_AND_RETHROW()
}


void database::validate_transaction( const signed_transaction& trx )
{
   database::with_write_lock( [&]()
   {
      auto session = start_undo_session();
      _apply_transaction( trx );
      session.undo();
   });
}

void database::set_flush_interval( uint32_t flush_blocks )
{
   _flush_blocks = flush_blocks;
   _next_flush_block = 0;
}

//////////////////// private methods ////////////////////

void database::apply_block( const signed_block& next_block, uint32_t skip )
{ try {
   //fc::time_point begin_time = fc::time_point::now();

   detail::with_skip_flags( *this, skip, [&]()
   {
      _apply_block( next_block );
   } );

   /*try
   {
   /// check invariants
   if( is_producing() || !( skip & skip_validate_invariants ) )
      validate_invariants();
   }
   FC_CAPTURE_AND_RETHROW( (next_block) );*/

   auto block_num = next_block.block_num();

   //fc::time_point end_time = fc::time_point::now();
   //fc::microseconds dt = end_time - begin_time;
   if( _flush_blocks != 0 )
   {
      if( _next_flush_block == 0 )
      {
         uint32_t lep = block_num + 1 + _flush_blocks * 9 / 10;
         uint32_t rep = block_num + 1 + _flush_blocks;

         // use time_point::now() as RNG source to pick block randomly between lep and rep
         uint32_t span = rep - lep;
         uint32_t x = lep;
         if( span > 0 )
         {
            uint64_t now = uint64_t( fc::time_point::now().time_since_epoch().count() );
            x += now % span;
         }
         _next_flush_block = x;
         //ilog( "Next flush scheduled at block ${b}", ("b", x) );
      }

      if( _next_flush_block == block_num )
      {
         _next_flush_block = 0;
         //ilog( "Flushing database shared memory at block ${b}", ("b", block_num) );
         chainbase::database::flush();
      }
   }

} FC_CAPTURE_AND_RETHROW( (next_block) ) }

void database::check_free_memory( bool force_print, uint32_t current_block_num )
{
#ifndef ENABLE_MIRA
   uint64_t free_mem = get_free_memory();
   uint64_t max_mem = get_max_memory();

   if( BOOST_UNLIKELY( _shared_file_full_threshold != 0 && _shared_file_scale_rate != 0 && free_mem < ( ( uint128_t( XGT_100_PERCENT - _shared_file_full_threshold ) * max_mem ) / XGT_100_PERCENT ).to_uint64() ) )
   {
      uint64_t new_max = ( uint128_t( max_mem * _shared_file_scale_rate ) / XGT_100_PERCENT ).to_uint64() + max_mem;

      wlog( "Memory is almost full, increasing to ${mem}M", ("mem", new_max / (1024*1024)) );

      resize( new_max );

      uint32_t free_mb = uint32_t( get_free_memory() / (1024*1024) );
      wlog( "Free memory is now ${free}M", ("free", free_mb) );
      _last_free_gb_printed = free_mb / 1024;
   }
   else
   {
      uint32_t free_gb = uint32_t( free_mem / (1024*1024*1024) );
      if( BOOST_UNLIKELY( force_print || (free_gb < _last_free_gb_printed) || (free_gb > _last_free_gb_printed+1) ) )
      {
         ilog( "Free memory is now ${n}G. Current block number: ${block}", ("n", free_gb)("block",current_block_num) );
         _last_free_gb_printed = free_gb;
      }

      if( BOOST_UNLIKELY( free_gb == 0 ) )
      {
         uint32_t free_mb = uint32_t( free_mem / (1024*1024) );

   #ifdef IS_TEST_NET
      if( !disable_low_mem_warning )
   #endif
         if( free_mb <= 100 && head_block_num() % 10 == 0 )
            elog( "Free memory is now ${n}M. Increase shared file size immediately!" , ("n", free_mb) );
      }
   }
#endif
}

void database::_apply_block( const signed_block& next_block )
{ try {
   block_notification note( next_block );

   notify_pre_apply_block( note );

   const uint32_t next_block_num = note.block_num;

   uint32_t skip = get_node_properties().skip_flags;

   _current_block_num    = next_block_num;
   _current_trx_in_block = 0;
   _current_virtual_op   = 0;

   if( BOOST_UNLIKELY( next_block_num == 1 ) )
   {
      // For every existing before the head_block_time (genesis time), apply the hardfork
      // This allows the test net to launch with past hardforks and apply the next harfork when running

      uint32_t n;
      for( n=0; n<XGT_NUM_HARDFORKS; n++ )
      {
         if( _hardfork_versions.times[n+1] > next_block.timestamp )
            break;
      }

      if( n > 0 )
      {
         ilog( "Processing ${n} genesis hardforks", ("n", n) );
         set_hardfork( n, true );

         const hardfork_property_object& hardfork_state = get_hardfork_property_object();
         FC_ASSERT( hardfork_state.current_hardfork_version == _hardfork_versions.versions[n], "Unexpected genesis hardfork state" );

         const auto& witness_idx = get_index<witness_index>().indices().get<by_id>();
         vector<witness_id_type> wit_ids_to_update;
         for( auto it=witness_idx.begin(); it!=witness_idx.end(); ++it )
            wit_ids_to_update.push_back(it->id);

         for( witness_id_type wit_id : wit_ids_to_update )
         {
            modify( get( wit_id ), [&]( witness_object& wit )
            {
               wit.running_version = _hardfork_versions.versions[n];
               wit.hardfork_version_vote = _hardfork_versions.versions[n];
               wit.hardfork_time_vote = _hardfork_versions.times[n];
            } );
         }
      }
   }

   if( !( skip & skip_merkle_check ) )
   {
      auto merkle_root = next_block.calculate_merkle_root();

      try
      {
         FC_ASSERT( next_block.transaction_merkle_root == merkle_root, "Merkle check failed", ("next_block.transaction_merkle_root",next_block.transaction_merkle_root)("calc",merkle_root)("next_block",next_block)("id",next_block.id()) );
      }
      catch( fc::assert_exception& e )
      {
         const auto& merkle_map = get_shared_db_merkle();
         auto itr = merkle_map.find( next_block_num );

         if( itr == merkle_map.end() || itr->second != merkle_root )
            throw e;
      }
   }

   // const witness_object& signing_witness = validate_block_header(skip, next_block);

   const auto& gprops = get_dynamic_global_properties();
   auto block_size = fc::raw::pack_size( next_block );
   FC_ASSERT( block_size <= gprops.maximum_block_size, "Block Size is too Big", ("next_block_num",next_block_num)("block_size", block_size)("max",gprops.maximum_block_size) );

   if( block_size < XGT_MIN_BLOCK_SIZE )
   {
      elog( "Block size is too small",
         ("next_block_num",next_block_num)("block_size", block_size)("min",XGT_MIN_BLOCK_SIZE)
      );
   }

   /// modify current witness so transaction evaluators can know who included the transaction,
   /// this is mostly for POW operations which must pay the current_witness
   modify( gprops, [&]( dynamic_global_property_object& dgp ){
      dgp.current_witness = next_block.witness;
   });

   required_automated_actions req_actions;
   optional_automated_actions opt_actions;
   /// parse witness version reporting
   process_header_extensions( next_block, req_actions, opt_actions );

   const auto& witness = get_witness( next_block.witness );
   const auto& hardfork_state = get_hardfork_property_object();
   FC_ASSERT( witness.running_version >= hardfork_state.current_hardfork_version,
      "Block produced by witness that is not running current hardfork",
      ("witness",witness)("next_block.witness",next_block.witness)("hardfork_state", hardfork_state)
   );

   for( const auto& trx : next_block.transactions )
   {
      /* We do not need to push the undo state for each transaction
       * because they either all apply and are valid or the
       * entire block fails to apply.  We only need an "undo" state
       * for transactions when validating broadcast transactions or
       * when building a block.
       */
      apply_transaction( trx, skip );
      ++_current_trx_in_block;
   }

   _current_trx_in_block = -1;
   _current_op_in_trx = 0;
   _current_virtual_op = 0;

   update_global_dynamic_data(next_block);
   // update_signing_witness(signing_witness, next_block);

   update_last_irreversible_block();

   create_block_summary(next_block);
   clear_expired_transactions();

   clear_null_wallet_balance();

   // account_recovery_processing();
   // expire_escrow_ratification();

   // generate_required_actions();
   // generate_optional_actions();

   // process_required_actions( req_actions );
   // process_optional_actions( opt_actions );

   // Ensure no duplicate mining rewards
   /// @since 1.1.2 reject blocks with duplicate rewards
   uint32_t head_num = head_block_num();
   if (head_num >= 907200)
   {
      std::set< wallet_name_type > rewarded_wallets;
      for( const auto& trx : next_block.transactions )
      {

         const auto& operations = trx.operations;
         for (auto& op : operations)
         {
            if ( !is_pow_operation(op) )
               continue;
            const pow_operation& o = op.template get< pow_operation >();
            const wallet_name_type& wallet_name = o.get_worker_name();
            auto it = rewarded_wallets.find(wallet_name);
            if (it != rewarded_wallets.end())
            {
               wlog("!!!!!! Wallet ${w} already rewarded, discarding duplicate operation!", ("w", wallet_name));
               continue;
            }
            rewarded_wallets.insert(wallet_name);
         }
      }
   }

   // Adjust mining difficulty
   const uint32_t frequency = XGT_MINING_RECALC_EVERY_N_BLOCKS;
   if( next_block_num == 1)
   {
      fc::sha256 initial_target = fc::sha256(XGT_MINING_TARGET_START);
      wlog("MINING DIFFICULTY - Initializing mining difficulty at ${w}", ("w",initial_target));
      const auto& gprops = get_dynamic_global_properties();
      modify( gprops, [&]( dynamic_global_property_object& dgp ) {
         dgp.mining_target = initial_target;
      });
   }
   else if( (XGT_STARTING_OFFSET + next_block_num) % frequency == 1 )
   {
      wlog("MINING DIFFICULTY - Updating mining difficulty...");


      const uint32_t prior_block_num = next_block_num - XGT_MINING_RECALC_EVERY_N_BLOCKS;
      wlog("MINING DIFFICULTY - prior_block_num ${a}", ("a",prior_block_num));
      optional<signed_block> prior_block = optional<signed_block>();
      prior_block = _block_log.read_block_by_num(prior_block_num);
      if (!prior_block)
      {
         auto itr = _fork_db.fetch_block_by_number(prior_block_num);
         auto block = itr[0]->data;
         prior_block = optional<signed_block>(block);
      }
      wlog("MINING DIFFICULTY - prior_block_num ${a} next_block_num ${b} next_block_timestamp ${d}", ("a",prior_block_num)("b",next_block_num)("c",prior_block->timestamp)("d",next_block.timestamp));

      const auto& gprops = get_dynamic_global_properties();
      fc::time_point_sec now = next_block.timestamp;
      fc::microseconds interval = now - prior_block->timestamp;
      float actual = (float)interval.to_seconds();
      float expected = XGT_MINING_RECALC_EVERY_N_BLOCKS / XGT_MINING_BLOCKS_PER_SECOND;
      wlog("MINING DIFFICULTY - Interval actual ${a} expected ${b}", ("a",actual)("b",expected));
      float ratio = expected / actual;

      // Limit the adjustment by a factor of 4 (to prevent massive changes from one target to the next)
      float adjusted_ratio = ratio;
      float reciprocal = 1.0f / XGT_MINING_ADJUSTMENT_MAX_FACTOR;
      if (adjusted_ratio < reciprocal)
         adjusted_ratio = reciprocal;
      else if (adjusted_ratio > XGT_MINING_ADJUSTMENT_MAX_FACTOR)
         adjusted_ratio = XGT_MINING_ADJUSTMENT_MAX_FACTOR;

      wlog("MINING DIFFICULTY - Updating mining difficulty ratio ${w} adjusted_ratio ${x}", ("w",ratio)("x",adjusted_ratio));

      const fc::sha256 max_target_h = fc::sha256(XGT_MINING_TARGET_MAX);
      boost::multiprecision::uint256_t max_target = hash_to_bigint(max_target_h);
      const fc::sha256 previous_target_h = gprops.mining_target;
      boost::multiprecision::uint256_t previous_target = hash_to_bigint(previous_target_h);
      boost::multiprecision::cpp_dec_float_50 previous_target_f(previous_target);
      boost::multiprecision::cpp_dec_float_50 next_target_f = previous_target_f  / adjusted_ratio;
      boost::multiprecision::uint256_t next_target(next_target_f);
      fc::sha256 next_target_h = bigint_to_hash(next_target);
      if (next_target >= max_target)
      {
         wlog( "MINING DIFFICULTY - Capping next target ${a} to be lower than ${b}",
               ("a",next_target_h)("b",max_target_h) );
         next_target = max_target;
         next_target_h = bigint_to_hash(next_target);
      }
      wlog("MINING DIFFICULTY - Updating mining difficulty previous_target ${a} next target ${b}", ("a",previous_target_h.approx_log_32())("b",next_target_h.approx_log_32()));

      modify( gprops, [&]( dynamic_global_property_object& dgp ) {
         dgp.mining_target = next_target_h;
      });
   }

   process_hardforks();

   // notify observers that the block has been applied
   notify_post_apply_block( note );

   // This moves newly irreversible blocks from the fork db to the block log
   // and commits irreversible state to the database. This should always be the
   // last call of applying a block because it is the only thing that is not
   // reversible.
   migrate_irreversible_state();
   trim_cache();
} FC_CAPTURE_LOG_AND_RETHROW( (next_block.block_num()) ) }

struct process_header_visitor
{
   process_header_visitor( const std::string& witness, required_automated_actions& req_actions, optional_automated_actions& opt_actions, database& db ) :
      _witness( witness ),
      _req_actions( req_actions ),
      _opt_actions( opt_actions ),
      _db( db ) {}

   typedef void result_type;

   const std::string& _witness;
   required_automated_actions& _req_actions;
   optional_automated_actions& _opt_actions;
   database& _db;

   void operator()( const void_t& obj ) const
   {
      //Nothing to do.
   }

   void operator()( const version& reported_version ) const
   {
      const auto& signing_witness = _db.get_witness( _witness );
      //idump( (next_block.witness)(signing_witness.running_version)(reported_version) );

      if( reported_version != signing_witness.running_version )
      {
         _db.modify( signing_witness, [&]( witness_object& wo )
         {
            wo.running_version = reported_version;
         });
      }
   }

   void operator()( const hardfork_version_vote& hfv ) const
   {
      const auto& signing_witness = _db.get_witness( _witness );
      //idump( (next_block.witness)(signing_witness.running_version)(hfv) );

      if( hfv.hf_version != signing_witness.hardfork_version_vote || hfv.hf_time != signing_witness.hardfork_time_vote )
         _db.modify( signing_witness, [&]( witness_object& wo )
         {
            wo.hardfork_version_vote = hfv.hf_version;
            wo.hardfork_time_vote = hfv.hf_time;
         });
   }

   void operator()( const required_automated_actions& req_actions ) const
   {
      std::copy( req_actions.begin(), req_actions.end(), std::back_inserter( _req_actions ) );
   }

   void operator()( const optional_automated_actions& opt_actions ) const
   {
      std::copy( opt_actions.begin(), opt_actions.end(), std::back_inserter( _opt_actions ) );
   }
};

void database::process_header_extensions( const signed_block& next_block, required_automated_actions& req_actions, optional_automated_actions& opt_actions )
{
   process_header_visitor _v( next_block.witness, req_actions, opt_actions, *this );

   for( const auto& e : next_block.extensions )
      e.visit( _v );
}

void database::apply_transaction(const signed_transaction& trx, uint32_t skip)
{
   detail::with_skip_flags( *this, skip, [&]() { _apply_transaction(trx); });
}

void database::_apply_transaction(const signed_transaction& trx)
{ try {
   transaction_notification note(trx);
   _current_trx_id = note.transaction_id;
   const transaction_id_type& trx_id = note.transaction_id;
   _current_virtual_op = 0;

   uint32_t skip = get_node_properties().skip_flags;

   if( !(skip&skip_validate) )   /* issue #505 explains why this skip_flag is disabled */
      trx.validate();

   auto& trx_idx = get_index<transaction_index>();
   const chain_id_type& chain_id = get_chain_id();
   // idump((trx_id)(skip&skip_transaction_dupe_check));
   FC_ASSERT( (skip & skip_transaction_dupe_check) ||
              trx_idx.indices().get<by_trx_id>().find(trx_id) == trx_idx.indices().get<by_trx_id>().end(),
              "Duplicate transaction check failed", ("trx_ix", trx_id) );

   if( !(skip & (skip_transaction_signatures | skip_authority_check) ) )
   {
      auto get_money    = [&]( const string& name ) { return authority( get< account_authority_object, by_account >( name ).money ); };
      auto get_recovery = [&]( const string& name ) { return authority( get< account_authority_object, by_account >( name ).recovery ); };
      auto get_social   = [&]( const string& name ) { return authority( get< account_authority_object, by_account >( name ).social ); };

      const auto& operations = trx.operations;
      try
      {
         for (auto& op : operations)
         {
            if ( is_pow_operation(op) ) {
               const pow_operation& o = op.template get< pow_operation >();
               auto wallet_name = o.get_worker_name();

               flat_set<public_key_type> key_set = trx.get_signature_keys(get_chain_id(), fc::ecc::fc_canonical);
               shared_authority money = get< account_authority_object, by_account >( wallet_name ).money;
               fc::optional<public_key_type> public_key;
               vector<public_key_type> money_key_set = money.get_keys();
               for (const public_key_type& key : money_key_set) {
                  if (key_set.find(key) != key_set.end())
                  {
                     public_key = fc::optional<public_key_type>(key);
                     break;
                  }
               }
               shared_authority recovery = get< account_authority_object, by_account >( wallet_name ).recovery;
               vector<public_key_type> recovery_key_set = recovery.get_keys();
               for (const public_key_type& key : recovery_key_set) {
                  if (key_set.find(key) != key_set.end())
                  {
                     public_key = fc::optional<public_key_type>(key);
                     break;
                  }
               }
               if (public_key == fc::optional<public_key_type>())
               {
                  wlog("!!!!!! Wallet no public key");
                  throw operation_validate_exception();
               }
            }
            else if ( is_wallet_create_operation(op) )
            {
               wlog("!!!!!! Wallet create");
               break;
            }
            else if ( is_wallet_update_operation(op) )
            {
               // TODO: DRY this out
               const wallet_update_operation& o = op.template get< wallet_update_operation >();
               shared_authority recovery = get< account_authority_object, by_account >( o.wallet ).recovery;
               vector<public_key_type> recovery_key_set = recovery.get_keys();
               flat_set<public_key_type> key_set = trx.get_signature_keys(get_chain_id(), fc::ecc::fc_canonical);
               vector<public_key_type> keys;
               keys.reserve(key_set.size());
               for (const public_key_type& key : key_set) {
                  keys.push_back(key);
               }
               string wallet_name = wallet_create_operation::get_wallet_name(keys);
               wlog("!!!!!! Wallet update wallet name ${w}", ("w",wallet_name));
               if (o.wallet == wallet_name) {
                  // Valid, don't throw error
                  wlog("!!!!!! Wallet update valid");
               } else {
                  // TODO: Invalid, throw error
                  wlog("!!!!!! Wallet update invalid");
                  throw operation_validate_exception();
               }
            }
         }
         trx.verify_authority( chain_id, get_money, get_recovery, get_social, XGT_MAX_SIG_CHECK_DEPTH,
               XGT_MAX_AUTHORITY_MEMBERSHIP,
               XGT_MAX_SIG_CHECK_WALLETS,
               fc::ecc::bip_0062 );
      }
      catch( protocol::tx_missing_money_auth& e )
      {
         if( get_shared_db_merkle().find( head_block_num() + 1 ) == get_shared_db_merkle().end() )
            throw e;
      }
   }

   //Skip all manner of expiration and TaPoS checking if we're on block 1; It's impossible that the transaction is
   //expired, and TaPoS makes no sense as no blocks exist.
   if( BOOST_LIKELY(head_block_num() > 0) )
   {
      //if( !(skip & skip_tapos_check) )
      //{
         // const auto& tapos_block_summary = get< block_summary_object >( trx.ref_block_num );
         //Verify TaPoS block summary has correct ID prefix, and that this block's time is not past the expiration
         //ilog( "trx.ref_block_prefix == ${h}", ("h", trx.ref_block_prefix) );
         // TODO: XXX: Add these back in
         // ilog( "tapos_block_summary.block_id._hash[1] == ${h}", ("h", tapos_block_summary.block_id._hash[1]) );
         // XGT_ASSERT( trx.ref_block_prefix == tapos_block_summary.block_id._hash[1], transaction_tapos_exception,
         //            "", ("trx.ref_block_prefix", trx.ref_block_prefix)
         //            ("tapos_block_summary",tapos_block_summary.block_id._hash[1]));
      //}

      fc::time_point_sec now = head_block_time();

      XGT_ASSERT( trx.expiration <= now + fc::seconds(XGT_MAX_TIME_UNTIL_EXPIRATION), transaction_expiration_exception,
                  "", ("trx.expiration",trx.expiration)("now",now)("max_til_exp",XGT_MAX_TIME_UNTIL_EXPIRATION));
      XGT_ASSERT( now < trx.expiration, transaction_expiration_exception, "", ("now",now)("trx.exp",trx.expiration) );
   }

   //Insert transaction into unique transactions database.
   if( !(skip & skip_transaction_dupe_check) )
   {
      create<transaction_object>([&](transaction_object& transaction) {
         transaction.trx_id = trx_id;
         transaction.expiration = trx.expiration;
         fc::raw::pack_to_buffer( transaction.packed_trx, trx );
      });
   }

   notify_pre_apply_transaction( note );

   //Finally process the operations
   _current_op_in_trx = 0;
   for( const operation& op : trx.operations )
   { try {
      try {
        apply_operation(op);
      } catch ( const fc::exception& e ) {
        wlog("!!!!!! Error applying operation ${w}", ("w",e));
        throw e;
      }
      ++_current_op_in_trx;
     } FC_CAPTURE_AND_RETHROW( (op) );
   }
   _current_trx_id = transaction_id_type();

   notify_post_apply_transaction( note );
} FC_CAPTURE_AND_RETHROW( (trx) ) }

void database::apply_operation(const operation& op)
{
   operation_notification note = create_operation_notification( op );
   notify_pre_apply_operation( note );

   if( _benchmark_dumper.is_enabled() )
      _benchmark_dumper.begin();

   _my->_evaluator_registry.get_evaluator( op ).apply( op );

   if( _benchmark_dumper.is_enabled() )
      _benchmark_dumper.end< true/*APPLY_CONTEXT*/ >( _my->_evaluator_registry.get_evaluator( op ).get_name( op ) );

   notify_post_apply_operation( note );
}

struct action_equal_visitor
{
   typedef bool result_type;

   const required_automated_action& action_a;

   action_equal_visitor( const required_automated_action& a ) : action_a( a ) {}

   template< typename Action >
   bool operator()( const Action& action_b )const
   {
      if( action_a.which() != required_automated_action::tag< Action >::value ) return false;

      return action_a.get< Action >() == action_b;
   }
};

void database::process_required_actions( const required_automated_actions& actions )
{
   const auto& pending_action_idx = get_index< pending_required_action_index, by_execution >();
   auto actions_itr = actions.begin();
   uint64_t total_actions_size = 0;

   while( true )
   {
      auto pending_itr = pending_action_idx.begin();

      if( actions_itr == actions.end() )
      {
         // We're done processing actions in the block.
         if( pending_itr != pending_action_idx.end() && pending_itr->execution_time <= head_block_time() )
         {
            total_actions_size += fc::raw::pack_size( pending_itr->action );
            const auto& gpo = get_dynamic_global_properties();
            uint64_t required_actions_partition_size = ( gpo.maximum_block_size * gpo.required_actions_partition_percent ) / XGT_100_PERCENT;
            FC_ASSERT( total_actions_size > required_actions_partition_size,
               "Expected action was not included in block. total_actions_size: ${as}, required_actions_partition_action: ${rs}, pending_action: ${pa}",
               ("as", total_actions_size)
               ("rs", required_actions_partition_size)
               ("pa", *pending_itr) );
         }
         break;
      }

      FC_ASSERT( pending_itr != pending_action_idx.end(),
         "Block included required action that does not exist in queue" );

      action_equal_visitor equal_visitor( pending_itr->action );
      FC_ASSERT( actions_itr->visit( equal_visitor ),
         "Unexpected action included. Expected: ${e} Observed: ${o}",
         ("e", pending_itr->action)("o", *actions_itr) );

      apply_required_action( *actions_itr );

      total_actions_size += fc::raw::pack_size( *actions_itr );

      remove( *pending_itr );
      ++actions_itr;
   }
}

void database::apply_required_action( const required_automated_action& a )
{
   required_action_notification note( a );
   notify_pre_apply_required_action( note );

   if( _benchmark_dumper.is_enabled() )
      _benchmark_dumper.begin();

   _my->_req_action_evaluator_registry.get_evaluator( a ).apply( a );

   if( _benchmark_dumper.is_enabled() )
      _benchmark_dumper.end< true/*APPLY_CONTEXT*/ >( _my->_req_action_evaluator_registry.get_evaluator( a ).get_name( a ) );

   notify_post_apply_required_action( note );
}

void database::process_optional_actions( const optional_automated_actions& actions )
{
   static const action_validate_visitor validate_visitor;

   for( auto actions_itr = actions.begin(); actions_itr != actions.end(); ++actions_itr )
   {
      actions_itr->visit( validate_visitor );

      // There is no execution check because we don't have a good way of indexing into local
      // optional actions from those contained in a block. It is the responsibility of the
      // action evaluator to prevent early execution.
      apply_optional_action( *actions_itr );
      auto action_itr = find< pending_optional_action_object, by_hash >( fc::sha256::hash( *actions_itr ) );
      if( action_itr != nullptr ) remove( *action_itr );
   }

   // This expiration is based on the timestamp of the last irreversible block. For historical
   // blocks, generation of optional actions should be disabled and the expiration can be skipped.
   // For reindexing of the first 2 million blocks, this unnecessary read consumes almost 30%
   // of runtime.
   FC_TODO( "Optimize expiration for reindex." );

   // Clear out "expired" optional_actions. If the block when an optional action was generated
   // has become irreversible then a super majority of witnesses have chosen to not include it
   // and it is safe to delete.
   const auto& pending_action_idx = get_index< pending_optional_action_index, by_execution >();
   auto pending_itr = pending_action_idx.begin();
   auto lib = fetch_block_by_number( get_dynamic_global_properties().last_irreversible_block_num );

   // This is always valid when running on mainnet because there are irreversible blocks
   // Testnet and unit tests, not so much. Could be ifdeffed with IS_TEST_NET, but seems
   // like a reasonable check and will be optimized via speculative execution.
   if( lib.valid() )
   {
      while( pending_itr != pending_action_idx.end() && pending_itr->execution_time <= lib->timestamp )
      {
         remove( *pending_itr );
         pending_itr = pending_action_idx.begin();
      }
   }
}

void database::apply_optional_action( const optional_automated_action& a )
{
   optional_action_notification note( a );
   notify_pre_apply_optional_action( note );

   if( _benchmark_dumper.is_enabled() )
      _benchmark_dumper.begin();

   _my->_opt_action_evaluator_registry.get_evaluator( a ).apply( a );

   if( _benchmark_dumper.is_enabled() )
      _benchmark_dumper.end< true/*APPLY_CONTEXT*/ >( _my->_opt_action_evaluator_registry.get_evaluator( a ).get_name( a ) );

   notify_post_apply_optional_action( note );
}

template <typename TFunction> struct fcall {};

template <typename TResult, typename... TArgs>
struct fcall<TResult(TArgs...)>
{
   using TNotification = std::function<TResult(TArgs...)>;

   fcall() = default;
   fcall(const TNotification& func, util::advanced_benchmark_dumper& dumper,
         const abstract_plugin& plugin, const std::string& item_name)
         : _func(func), _benchmark_dumper(dumper)
      {
         _name = plugin.get_name() + item_name;
      }

   void operator () (TArgs&&... args)
   {
      if (_benchmark_dumper.is_enabled())
         _benchmark_dumper.begin();

      _func(std::forward<TArgs>(args)...);

      if (_benchmark_dumper.is_enabled())
         _benchmark_dumper.end(_name);
   }

private:
   TNotification                    _func;
   util::advanced_benchmark_dumper& _benchmark_dumper;
   std::string                      _name;
};

template <typename TResult, typename... TArgs>
struct fcall<std::function<TResult(TArgs...)>>
   : public fcall<TResult(TArgs...)>
{
   typedef fcall<TResult(TArgs...)> TBase;
   using TBase::TBase;
};

template <typename TSignal, typename TNotification>
boost::signals2::connection database::connect_impl( TSignal& signal, const TNotification& func,
   const abstract_plugin& plugin, int32_t group, const std::string& item_name )
{
   fcall<TNotification> fcall_wrapper(func,_benchmark_dumper,plugin,item_name);

   return signal.connect(group, fcall_wrapper);
}

template< bool IS_PRE_OPERATION >
boost::signals2::connection database::any_apply_operation_handler_impl( const apply_operation_handler_t& func,
   const abstract_plugin& plugin, int32_t group )
{
   auto complex_func = [this, func, &plugin]( const operation_notification& o )
   {
      std::string name;

      if (_benchmark_dumper.is_enabled())
      {
         if( _my->_evaluator_registry.is_evaluator( o.op ) )
            name = _benchmark_dumper.generate_desc< IS_PRE_OPERATION >( plugin.get_name(), _my->_evaluator_registry.get_evaluator( o.op ).get_name( o.op ) );
         else
            name = util::advanced_benchmark_dumper::get_virtual_operation_name();

         _benchmark_dumper.begin();
      }

      func( o );

      if (_benchmark_dumper.is_enabled())
         _benchmark_dumper.end( name );
   };

   if( IS_PRE_OPERATION )
      return _pre_apply_operation_signal.connect(group, complex_func);
   else
      return _post_apply_operation_signal.connect(group, complex_func);
}

boost::signals2::connection database::add_pre_apply_required_action_handler( const apply_required_action_handler_t& func,
   const abstract_plugin& plugin, int32_t group )
{
   return connect_impl(_pre_apply_required_action_signal, func, plugin, group, "->required_action");
}

boost::signals2::connection database::add_post_apply_required_action_handler( const apply_required_action_handler_t& func,
   const abstract_plugin& plugin, int32_t group )
{
   return connect_impl(_post_apply_required_action_signal, func, plugin, group, "<-required_action");
}

boost::signals2::connection database::add_pre_apply_optional_action_handler( const apply_optional_action_handler_t& func,
   const abstract_plugin& plugin, int32_t group )
{
   return connect_impl(_pre_apply_optional_action_signal, func, plugin, group, "->optional_action");
}

boost::signals2::connection database::add_post_apply_optional_action_handler( const apply_optional_action_handler_t& func,
   const abstract_plugin& plugin, int32_t group )
{
   return connect_impl(_post_apply_optional_action_signal, func, plugin, group, "<-optional_action");
}

boost::signals2::connection database::add_pre_apply_operation_handler( const apply_operation_handler_t& func,
   const abstract_plugin& plugin, int32_t group )
{
   return any_apply_operation_handler_impl< true/*IS_PRE_OPERATION*/ >( func, plugin, group );
}

boost::signals2::connection database::add_post_apply_operation_handler( const apply_operation_handler_t& func,
   const abstract_plugin& plugin, int32_t group )
{
   return any_apply_operation_handler_impl< false/*IS_PRE_OPERATION*/ >( func, plugin, group );
}

boost::signals2::connection database::add_pre_apply_transaction_handler( const apply_transaction_handler_t& func,
   const abstract_plugin& plugin, int32_t group )
{
   return connect_impl(_pre_apply_transaction_signal, func, plugin, group, "->transaction");
}

boost::signals2::connection database::add_post_apply_transaction_handler( const apply_transaction_handler_t& func,
   const abstract_plugin& plugin, int32_t group )
{
   return connect_impl(_post_apply_transaction_signal, func, plugin, group, "<-transaction");
}

boost::signals2::connection database::add_pre_apply_custom_operation_handler ( const apply_custom_operation_handler_t& func,
   const abstract_plugin& plugin, int32_t group )
{
   return connect_impl(_pre_apply_custom_operation_signal, func, plugin, group, "->custom");
}

boost::signals2::connection database::add_post_apply_custom_operation_handler( const apply_custom_operation_handler_t& func,
   const abstract_plugin& plugin, int32_t group )
{
   return connect_impl(_post_apply_custom_operation_signal, func, plugin, group, "<-custom");
}

boost::signals2::connection database::add_pre_apply_block_handler( const apply_block_handler_t& func,
   const abstract_plugin& plugin, int32_t group )
{
   return connect_impl(_pre_apply_block_signal, func, plugin, group, "->block");
}

boost::signals2::connection database::add_post_apply_block_handler( const apply_block_handler_t& func,
   const abstract_plugin& plugin, int32_t group )
{
   return connect_impl(_post_apply_block_signal, func, plugin, group, "<-block");
}

boost::signals2::connection database::add_irreversible_block_handler( const irreversible_block_handler_t& func,
   const abstract_plugin& plugin, int32_t group )
{
   return connect_impl(_on_irreversible_block, func, plugin, group, "<-irreversible");
}

boost::signals2::connection database::add_pre_reindex_handler(const reindex_handler_t& func,
   const abstract_plugin& plugin, int32_t group )
{
   return connect_impl(_pre_reindex_signal, func, plugin, group, "->reindex");
}

boost::signals2::connection database::add_post_reindex_handler(const reindex_handler_t& func,
   const abstract_plugin& plugin, int32_t group )
{
   return connect_impl(_post_reindex_signal, func, plugin, group, "<-reindex");
}

boost::signals2::connection database::add_generate_optional_actions_handler(const generate_optional_actions_handler_t& func,
   const abstract_plugin& plugin, int32_t group )
{
   return connect_impl(_generate_optional_actions_signal, func, plugin, group, "->generate_optional_actions");
}

const witness_object& database::validate_block_header( uint32_t skip, const signed_block& next_block )const
{ try {
   FC_ASSERT( head_block_id() == next_block.previous, "", ("head_block_id",head_block_id())("next.prev",next_block.previous) );
   // TODO: XXX: Figure out what to do with this
   // FC_ASSERT( head_block_time() < next_block.timestamp, "", ("head_block_time",head_block_time())("next",next_block.timestamp)("blocknum",next_block.block_num()) );
   const witness_object& witness = get_witness( next_block.witness );

   if( !(skip&skip_witness_signature) )
      FC_ASSERT( next_block.validate_signee( witness.signing_key, fc::ecc::bip_0062 ) );

   return witness;
} FC_CAPTURE_AND_RETHROW() }

void database::create_block_summary(const signed_block& next_block)
{ try {
   block_summary_id_type sid( next_block.block_num() & 0xffff );
   modify( get< block_summary_object >( sid ), [&](block_summary_object& p) {
         p.block_id = next_block.id();
   });
} FC_CAPTURE_AND_RETHROW() }

void database::update_global_dynamic_data( const signed_block& b )
{ try {
   const dynamic_global_property_object& _dgp =
      get_dynamic_global_properties();

   // uint32_t missed_blocks = 0;
   // if( head_block_time() != fc::time_point_sec() )
   // {
   //    missed_blocks = get_slot_at_time( b.timestamp );
   //    assert( missed_blocks != 0 );
   //    missed_blocks--;

   //    // log number of missed blocks, if any
   //    if (missed_blocks > 0)
   //       wlog( "database::update_global_dynamic_data number of missed blocks ${a}", ("a", missed_blocks) );

   //    for( uint32_t i = 0; i < missed_blocks; ++i )
   //    {
   //       const auto& witness_missed = get_witness( get_scheduled_witness( i + 1 ) );
   //       if( witness_missed.owner != b.witness )
   //       {
   //          modify( witness_missed, [&]( witness_object& w )
   //          {
   //             w.total_missed++;
   //          } );
   //       }
   //    }
   // }

   // dynamic global properties updating
   modify( _dgp, [&]( dynamic_global_property_object& dgp )
   {
      // // This is constant time assuming 100% participation. It is O(B) otherwise (B = Num blocks between update)
      // for( uint32_t i = 0; i < missed_blocks + 1; i++ )
      // {
      //    dgp.participation_count -= dgp.recent_slots_filled.hi & 0x8000000000000000ULL ? 1 : 0;
      //    dgp.recent_slots_filled = ( dgp.recent_slots_filled << 1 ) + ( i == 0 ? 1 : 0 );
      //    dgp.participation_count += ( i == 0 ? 1 : 0 );
      // }

      dgp.head_block_number = b.block_num();
      dgp.head_block_id = b.id();
      dgp.time = b.timestamp;
   } );

   if( !(get_node_properties().skip_flags & skip_undo_history_check) )
   {
      XGT_ASSERT( _dgp.head_block_number - _dgp.last_irreversible_block_num  < XGT_MAX_UNDO_HISTORY, undo_database_exception,
            "The database does not have enough undo history to support a blockchain with so many missed blocks. "
            "Please add a checkpoint if you would like to continue applying blocks beyond this point.",
            ("last_irreversible_block_num",_dgp.last_irreversible_block_num)("head", _dgp.head_block_number)
            ("max_undo",XGT_MAX_UNDO_HISTORY) );
   }
} FC_CAPTURE_AND_RETHROW() }

void database::update_signing_witness(const witness_object& signing_witness, const signed_block& new_block)
{ try {
   modify( signing_witness, [&]( witness_object& _wit )
   {
      _wit.last_confirmed_block_num = new_block.block_num();
   } );
} FC_CAPTURE_AND_RETHROW() }

void database::update_last_irreversible_block()
{ try {
   const dynamic_global_property_object& dpo = get_dynamic_global_properties();
   auto old_last_irreversible = dpo.last_irreversible_block_num;

   // TODO: XXX: We need to determine what we consider an irreversible block. Last `n` blocks?
   modify( dpo, [&]( dynamic_global_property_object& _dpo )
   {
      if ( head_block_num() > XGT_MAX_WITNESSES )
         _dpo.last_irreversible_block_num = head_block_num() - XGT_MAX_WITNESSES;
   } );

   for( uint32_t i = old_last_irreversible; i <= dpo.last_irreversible_block_num; ++i )
   {
      notify_irreversible_block( i );
   }
} FC_CAPTURE_AND_RETHROW() }

void database::migrate_irreversible_state()
{
   // This method should happen atomically. We cannot prevent unclean shutdown in the middle
   // of the call, but all side effects happen at the end to minize the chance that state
   // invariants will be violated.
   try
   {
      const dynamic_global_property_object& dpo = get_dynamic_global_properties();

      auto fork_head = _fork_db.head();
      if( fork_head )
      {
         FC_ASSERT( fork_head->num == dpo.head_block_number, "Fork Head: ${f} Chain Head: ${c}", ("f",fork_head->num)("c", dpo.head_block_number) );
      }

      if( !( get_node_properties().skip_flags & skip_block_log ) )
      {
         // output to block log based on new last irreverisible block num
         const auto& tmp_head = _block_log.head();
         uint64_t log_head_num = 0;
         vector< item_ptr > blocks_to_write;

         if( tmp_head )
            log_head_num = tmp_head->block_num();

         if( log_head_num < dpo.last_irreversible_block_num )
         {
            // Check for all blocks that we want to write out to the block log but don't write any
            // unless we are certain they all exist in the fork db
            while( log_head_num < dpo.last_irreversible_block_num )
            {
               item_ptr block_ptr = _fork_db.fetch_block_on_main_branch_by_number( log_head_num+1 );
               FC_ASSERT( block_ptr, "Current fork in the fork database does not contain the last_irreversible_block" );
               blocks_to_write.push_back( block_ptr );
               log_head_num++;
            }

            for( auto block_itr = blocks_to_write.begin(); block_itr != blocks_to_write.end(); ++block_itr )
            {
               _block_log.append( block_itr->get()->data );
            }

            _block_log.flush();
         }
      }

      // This deletes blocks from the fork db
      _fork_db.set_max_size( dpo.head_block_number - dpo.last_irreversible_block_num + 1 );

      // This deletes undo state
      commit( dpo.last_irreversible_block_num );
   }
   FC_CAPTURE_AND_RETHROW()
}

void database::clear_expired_transactions()
{
   //Look for expired transactions in the deduplication list, and remove them.
   //Transactions must have expired by at least two forking windows in order to be removed.
   auto& transaction_idx = get_index< transaction_index >();
   const auto& dedupe_index = transaction_idx.indices().get< by_expiration >();
   while( ( !dedupe_index.empty() ) && ( head_block_time() > dedupe_index.begin()->expiration ) )
      remove( *dedupe_index.begin() );
}

template< typename xtt_balance_object_type, class balance_operator_type >
void database::adjust_xtt_balance( const wallet_name_type& name, const asset& delta, bool check_account,
   balance_operator_type balance_operator )
{
   asset_symbol_type liquid_symbol = delta.symbol;
   const xtt_balance_object_type* bo = find< xtt_balance_object_type, by_name_liquid_symbol >( boost::make_tuple( name, liquid_symbol ) );
   // Note that XTT related code, being post-20-hf needs no hf-guard to do balance checks.
   if( bo == nullptr )
   {
      // No balance object related to the XTT means '0' balance. Check delta to avoid creation of negative balance.
      FC_ASSERT( delta.amount.value >= 0, "Insufficient XTT ${xtt} funds", ("xtt", delta.symbol) );
      // No need to create object with '0' balance (see comment above).
      if( delta.amount.value == 0 )
         return;

      if( check_account )
         get_account( name );

      create< xtt_balance_object_type >( [&]( xtt_balance_object_type& xtt_balance )
      {
         xtt_balance.initialize_assets( liquid_symbol );
         xtt_balance.name = name;
         balance_operator.add_to_balance( xtt_balance );
         xtt_balance.validate();
      } );
   }
   else
   {
      bool is_all_zero = false;
      int64_t result = balance_operator.get_combined_balance( bo, &is_all_zero );
      // Check result to avoid negative balance storing.
      FC_ASSERT( result >= 0, "Insufficient XTT ${xtt} funds", ( "xtt", delta.symbol ) );

      modify( *bo, [&]( xtt_balance_object_type& xtt_balance )
      {
         balance_operator.add_to_balance( xtt_balance );
      } );
   }
}

void database::modify_balance( const wallet_object& a, const asset& delta, bool check_balance )
{
   modify( a, [&]( wallet_object& acnt )
   {
      switch( delta.symbol.asset_num )
      {
         case XGT_ASSET_NUM_XGT:
            acnt.balance += delta;
            if( check_balance )
            {
               FC_ASSERT( acnt.balance.amount.value >= 0, "Insufficient XGT funds" );
            }
            break;
         default:
            FC_ASSERT( false, "invalid symbol" );
      }
   } );
}

void database::set_index_delegate( const std::string& n, index_delegate&& d )
{
   _index_delegate_map[ n ] = std::move( d );
}

const index_delegate& database::get_index_delegate( const std::string& n )
{
   return _index_delegate_map.at( n );
}

bool database::has_index_delegate( const std::string& n )
{
   return _index_delegate_map.find( n ) != _index_delegate_map.end();
}

const index_delegate_map& database::index_delegates()
{
   return _index_delegate_map;
}

struct xtt_regular_balance_operator
{
   xtt_regular_balance_operator( const asset& delta ) : delta(delta) {}

   void add_to_balance( account_regular_balance_object& xtt_balance )
   {
      xtt_balance.liquid += delta;
   }
   int64_t get_combined_balance( const account_regular_balance_object* bo, bool* is_all_zero )
   {
      asset result = bo->liquid + delta;
      *is_all_zero = result.amount.value == 0;
      return result.amount.value;
   }

   asset delta;
};

void database::adjust_balance( const wallet_object& a, const asset& delta )
{
   if ( delta.amount < 0 )
   {
      asset available = get_balance( a, delta.symbol );
      FC_ASSERT( available >= -delta,
         "Account ${acc} does not have sufficient funds for balance adjustment. Required: ${r}, Available: ${a}",
            ("acc", a.name)("r", delta)("a", available) );
   }

   if( delta.symbol.space() == asset_symbol_type::xtt_nai_space )
   {
      if( a.name == XGT_NULL_WALLET )
      {
         adjust_supply( -delta );
      }
      else
      {
         // No account object modification for XTT balance, hence separate handling here.
         // Note that XTT related code, being post-20-hf needs no hf-guard to do balance checks.
         xtt_regular_balance_operator balance_operator( delta );
         adjust_xtt_balance< account_regular_balance_object >( a.name, delta, false/*check_account*/, balance_operator );
      }
   }
   else
   {
      modify_balance( a, delta, true/*check_balance*/ );
   }
}

void database::adjust_balance( const wallet_name_type& name, const asset& delta )
{
   if ( delta.amount < 0 )
   {
      asset available = get_balance( name, delta.symbol );
      FC_ASSERT( available >= -delta,
         "Account ${acc} does not have sufficient funds for balance adjustment. Required: ${r}, Available: ${a}",
            ("acc", name)("r", delta)("a", available) );
   }

   if( delta.symbol.space() == asset_symbol_type::xtt_nai_space )
   {
      if( name == XGT_NULL_WALLET )
      {
         adjust_supply( -delta );
      }
      else
      {
         // No account object modification for XTT balance, hence separate handling here.
         // Note that XTT related code, being post-20-hf needs no hf-guard to do balance checks.
         xtt_regular_balance_operator balance_operator( delta );
         adjust_xtt_balance< account_regular_balance_object >( name, delta, false/*check_account*/, balance_operator );
      }
   }
   else
   {
      modify_balance( get_account( name ), delta, true/*check_balance*/ );
   }
}

void database::adjust_supply( const asset& delta )
{
   if( delta.symbol.space() == asset_symbol_type::xtt_nai_space )
   {
      const auto& xtt = get< xtt_token_object, by_symbol >( delta.symbol );
      auto xtt_new_supply = xtt.current_supply + delta.amount;
      FC_ASSERT( xtt_new_supply >= 0 );
      modify( xtt, [xtt_new_supply]( xtt_token_object& xtt )
      {
         xtt.current_supply = xtt_new_supply;
      });
      return;
   }
}


asset database::get_balance( const wallet_object& a, asset_symbol_type symbol )const
{
   switch( symbol.asset_num )
   {
      case XGT_ASSET_NUM_XGT:
         return a.balance;
      default:
      {
         FC_ASSERT( symbol.space() == asset_symbol_type::xtt_nai_space, "Invalid symbol: ${s}", ("s", symbol) );
         auto key = boost::make_tuple( a.name, symbol );
         const account_regular_balance_object* arbo = find< account_regular_balance_object, by_name_liquid_symbol >( key );
         if( arbo == nullptr )
         {
            return asset(0, symbol);
         }
         else
         {
            return arbo->liquid;
         }
      }
   }
}

asset database::get_balance( const wallet_name_type& name, asset_symbol_type symbol )const
{
   if ( symbol.space() == asset_symbol_type::xtt_nai_space )
   {
      auto key = boost::make_tuple( name, symbol );
      const account_regular_balance_object* arbo = find< account_regular_balance_object, by_name_liquid_symbol >( key );

      if( arbo == nullptr )
      {
         return asset( 0, symbol );
      }
      else
      {
         return arbo->liquid;
      }
   }
   return get_balance( get_account( name ), symbol );
}

void database::generate_required_actions()
{

}

void database::generate_optional_actions()
{
   static const generate_optional_actions_notification note;
   XGT_TRY_NOTIFY( _generate_optional_actions_signal, note );
}

void database::init_hardforks()
{
   _hardfork_versions.times[ 0 ] = fc::time_point_sec( XGT_GENESIS_TIME );
   _hardfork_versions.versions[ 0 ] = hardfork_version( 0, 0 );

   const auto& hardforks = get_hardfork_property_object();
   FC_ASSERT( hardforks.last_hardfork <= XGT_NUM_HARDFORKS, "Chain knows of more hardforks than configuration", ("hardforks.last_hardfork",hardforks.last_hardfork)("XGT_NUM_HARDFORKS",XGT_NUM_HARDFORKS) );
   FC_ASSERT( _hardfork_versions.versions[ hardforks.last_hardfork ] <= XGT_BLOCKCHAIN_VERSION, "Blockchain version is older than last applied hardfork" );
   FC_ASSERT( XGT_BLOCKCHAIN_HARDFORK_VERSION >= XGT_BLOCKCHAIN_VERSION );
   FC_ASSERT( XGT_BLOCKCHAIN_HARDFORK_VERSION == _hardfork_versions.versions[ XGT_NUM_HARDFORKS ] );
}

void database::process_hardforks()
{
   try
   {
      // If there are upcoming hardforks and the next one is later, do nothing
      const auto& hardforks = get_hardfork_property_object();

      while( _hardfork_versions.versions[ hardforks.last_hardfork ] < hardforks.next_hardfork
         && hardforks.next_hardfork_time <= head_block_time() )
      {
         if( hardforks.last_hardfork < XGT_NUM_HARDFORKS ) {
            apply_hardfork( hardforks.last_hardfork + 1 );
         }
         else
            throw unknown_hardfork_exception();
      }
   }
   FC_CAPTURE_AND_RETHROW()
}

bool database::has_hardfork( uint32_t hardfork )const
{
   return get_hardfork_property_object().processed_hardforks.size() > hardfork;
}

uint32_t database::get_hardfork()const
{
   return get_hardfork_property_object().processed_hardforks.size() - 1;
}

void database::set_hardfork( uint32_t hardfork, bool apply_now )
{
   auto const& hardforks = get_hardfork_property_object();

   for( uint32_t i = hardforks.last_hardfork + 1; i <= hardfork && i <= XGT_NUM_HARDFORKS; i++ )
   {
      modify( hardforks, [&]( hardfork_property_object& hpo )
      {
         hpo.next_hardfork = _hardfork_versions.versions[i];
         hpo.next_hardfork_time = head_block_time();
      } );

      if( apply_now )
         apply_hardfork( i );
   }
}

void database::apply_hardfork( uint32_t hardfork )
{
   operation hardfork_vop = hardfork_operation( hardfork );

   pre_push_virtual_operation( hardfork_vop );

   switch( hardfork )
   {
      default:
         break;
   }

   modify( get_hardfork_property_object(), [&]( hardfork_property_object& hfp )
   {
      FC_ASSERT( hardfork == hfp.last_hardfork + 1, "Hardfork being applied out of order", ("hardfork",hardfork)("hfp.last_hardfork",hfp.last_hardfork) );
      FC_ASSERT( hfp.processed_hardforks.size() == hardfork, "Hardfork being applied out of order" );
      hfp.processed_hardforks.push_back( _hardfork_versions.times[ hardfork ] );
      hfp.last_hardfork = hardfork;
      hfp.current_hardfork_version = _hardfork_versions.versions[ hardfork ];
      FC_ASSERT( hfp.processed_hardforks[ hfp.last_hardfork ] == _hardfork_versions.times[ hfp.last_hardfork ], "Hardfork processing failed sanity check..." );
   } );

   post_push_virtual_operation( hardfork_vop );
}


/**
 * Verifies all supply invariantes check out
 */
void database::validate_invariants()const
{
   // try
   // {
   //    asset total_supply = asset( 0, XGT_SYMBOL );

   //    const auto& escrow_idx = get_index< escrow_index >().indices().get< by_id >();

   //    for( auto itr = escrow_idx.begin(); itr != escrow_idx.end(); ++itr )
   //    {
   //       total_supply += itr->xgt_balance;

   //       if( itr->pending_fee.symbol == XGT_SYMBOL )
   //          total_supply += itr->pending_fee;
   //       else
   //          FC_ASSERT( false, "found escrow pending fee that is not XGT" );
   //    }

   //    const auto& xtt_ico_idx = get_index< xtt_ico_index, by_id >();

   //    for ( auto itr = xtt_ico_idx.begin(); itr != xtt_ico_idx.end(); ++itr )
   //    {
   //       if ( get< xtt_token_object, by_symbol >( itr->symbol ).phase <= xtt_phase::launch_failed )
   //          total_supply += asset( itr->contributed.amount - itr->processed_contributions, XGT_SYMBOL );
   //       else
   //          total_supply += asset( itr->contributed.amount, XGT_SYMBOL );
   //    }

   //    // TODO: XXX: Assert these or something like them?
   //    //FC_ASSERT( gpo.current_supply == total_supply, "", ("gpo.current_supply",gpo.current_supply)("total_supply",total_supply) );
   //    //FC_ASSERT( gpo.virtual_supply >= gpo.current_supply );
   //    //int64_t max_vote_denom = gpo.target_votes_per_period * XGT_VOTING_ENERGY_REGENERATION_SECONDS;
   //    //FC_ASSERT( max_vote_denom > 0, "target_votes_per_period overflowed" );
   // }
   // FC_CAPTURE_LOG_AND_RETHROW( (head_block_num()) );
}

namespace {
   template <typename index_type, typename lambda>
   void add_from_balance_index(const index_type& balance_idx, lambda callback )
   {
      auto it = balance_idx.begin();
      auto end = balance_idx.end();
      for( ; it != end; ++it )
      {
         const auto& balance = *it;
         callback( balance );
      }
   }
}

/**
 * XTT version of validate_invariants.
 */
void database::validate_xtt_invariants()const
{
   try
   {
      // Get total balances.
      typedef struct {
         asset liquid;
         asset pending_liquid;
      } TCombinedBalance;
      typedef std::map< asset_symbol_type, TCombinedBalance > TCombinedSupplyMap;
      TCombinedSupplyMap theMap;

      // - Process regular balances
      const auto& balance_idx = get_index< account_regular_balance_index, by_id >();
      add_from_balance_index( balance_idx, [ &theMap ] ( const account_regular_balance_object& regular )
      {
         asset zero_liquid = asset( 0, regular.liquid.symbol );
         auto insertInfo = theMap.emplace( regular.liquid.symbol,
            TCombinedBalance( { regular.liquid, zero_liquid } ) );
         if( insertInfo.second == false )
            {
            TCombinedBalance& existing_balance = insertInfo.first->second;
            existing_balance.liquid += regular.liquid;
            }
      });

      // - Escrow - no support of XTT is expected.

      // Do the verification of total balances.
      auto itr = get_index< xtt_token_index, by_id >().begin();
      auto end = get_index< xtt_token_index, by_id >().end();
      for( ; itr != end; ++itr )
      {
         const xtt_token_object& xtt = *itr;
         auto totalIt = theMap.find( xtt.liquid_symbol );
         // Check liquid XTT supply.
         asset total_liquid_supply = totalIt == theMap.end() ? asset(0, xtt.liquid_symbol) :
            ( totalIt->second.liquid + totalIt->second.pending_liquid );
         FC_ASSERT( asset(xtt.current_supply, xtt.liquid_symbol) == total_liquid_supply,
                    "", ("xtt current_supply",xtt.current_supply)("total_liquid_supply",total_liquid_supply) );
      }
   }
   FC_CAPTURE_LOG_AND_RETHROW( (head_block_num()) );
}

void database::retally_comment_children()
{
   const auto& cidx = get_index< comment_index >().indices();

   // Clear children counts
   for( auto itr = cidx.begin(); itr != cidx.end(); ++itr )
   {
      modify( *itr, [&]( comment_object& c )
      {
         c.children = 0;
      });
   }

   for( auto itr = cidx.begin(); itr != cidx.end(); ++itr )
   {
      if( itr->parent_author != XGT_ROOT_POST_PARENT )
      {
// Low memory nodes only need immediate child count, full nodes track total children
#ifdef IS_LOW_MEM
         modify( get_comment( itr->parent_author, itr->parent_permlink ), [&]( comment_object& c )
         {
            c.children++;
         });
#else
         const comment_object* parent = &get_comment( itr->parent_author, itr->parent_permlink );
         while( parent )
         {
            modify( *parent, [&]( comment_object& c )
            {
               c.children++;
            });

            if( parent->parent_author != XGT_ROOT_POST_PARENT )
               parent = &get_comment( parent->parent_author, parent->parent_permlink );
            else
               parent = nullptr;
         }
#endif
      }
   }
}

optional< chainbase::database::session >& database::pending_transaction_session()
{
   return _pending_tx_session;
}

} } //xgt::chain
