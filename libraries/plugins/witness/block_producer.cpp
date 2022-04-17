#include <xgt/plugins/witness/block_producer.hpp>

#include <xgt/protocol/base.hpp>
#include <xgt/protocol/config.hpp>
#include <xgt/protocol/version.hpp>
#include <xgt/protocol/operations.hpp>

#include <xgt/chain/database_exceptions.hpp>
#include <xgt/chain/db_with.hpp>
#include <xgt/chain/pending_required_action_object.hpp>
#include <xgt/chain/pending_optional_action_object.hpp>
#include <xgt/chain/witness_objects.hpp>

#include <fc/macros.hpp>

#include <boost/scope_exit.hpp>

namespace xgt { namespace plugins { namespace witness {

chain::signed_block block_producer::generate_block(
   fc::time_point_sec when,
   const chain::wallet_name_type& witness_recovery,
   const fc::ecc::private_key& block_signing_private_key,
   fc::optional< xgt::chain::signed_transaction > trx,
   uint32_t skip
)
{
   chain::signed_block result;
   try
   {
      _db.set_producing( true );
      xgt::chain::detail::with_skip_flags(
         _db,
         skip,
         [&]()
         {
            try
            {
               result = _generate_block( when, witness_recovery, block_signing_private_key, trx );
            }
            FC_CAPTURE_AND_RETHROW( (witness_recovery) )
         });
      _db.set_producing( false );
   }
   catch( fc::exception& e )
   {
      _db.set_producing( false );
      throw e;
   }

   return result;
}

chain::signed_block block_producer::_generate_block(
   fc::time_point_sec when,
   const chain::wallet_name_type& witness,
   const fc::ecc::private_key& block_signing_private_key,
   fc::optional< xgt::chain::signed_transaction > block_reward
)
{ try {
   uint32_t skip = _db.get_node_properties().skip_flags;
   // const auto& witness_obj = _db.get_witness( witness );

   // wlog( "!!! block_producer::_generate_block witness ${b}", ("b", witness) );
   // wlog( "!!! block_producer::_generate_block witness_obj.signing_key ${b}", ("b", block_signing_private_key.get_public_key()) );
   // wlog( "!!! block_producer::_generate_block block_signing_private_key.get_public_key() ${b}", ("b", block_signing_private_key.get_public_key()) );

   if( !(skip & chain::database::skip_witness_signature) )
   {
      // TODO: XXX: Check this
      // wlog( "!!! WARNING: witness_obj.signing_key ${a} != block_signing_private_key.get_public_key() ${b}", ("a", witness_obj.signing_key)("b", block_signing_private_key.get_public_key()) );
      // FC_ASSERT( witness_obj.signing_key == block_signing_private_key.get_public_key() );
   }

   chain::signed_block pending_block;

   pending_block.previous = _db.head_block_id();
   pending_block.timestamp = when;
   pending_block.witness = witness;

   // wlog( "!!! block_producer::_generate_block pending_block.previous ${b}", ("b", pending_block.previous) );

   adjust_hardfork_version_vote( _db.get_witness( witness ), pending_block );

   apply_pending_transactions( witness, when, pending_block, block_reward );

   // We have temporarily broken the invariant that
   // _pending_tx_session is the result of applying _pending_tx, as
   // _pending_tx now consists of the set of postponed transactions.
   // However, the push_block() call below will re-create the
   // _pending_tx_session.

   if( !(skip & chain::database::skip_witness_signature) )
      pending_block.sign( block_signing_private_key, fc::ecc::bip_0062 );

   // TODO:  Move this to _push_block() so session is restored.
   if( !(skip & chain::database::skip_block_size_check) )
   {
      FC_ASSERT( fc::raw::pack_size(pending_block) <= XGT_MAX_BLOCK_SIZE );
   }

   // TODO: XXX: This seems to be an issue for mining
   _db.push_block( pending_block, skip );

   return pending_block;
} FC_LOG_AND_RETHROW() }

void block_producer::adjust_hardfork_version_vote(const chain::witness_object& witness, chain::signed_block& pending_block)
{
   using namespace xgt::protocol;

   if( witness.running_version != XGT_BLOCKCHAIN_VERSION )
      pending_block.extensions.insert( block_header_extensions( XGT_BLOCKCHAIN_VERSION ) );

   const auto& hfp = _db.get_hardfork_property_object();
   const auto& hf_versions = _db.get_hardfork_versions();

   if( hfp.current_hardfork_version < XGT_BLOCKCHAIN_VERSION // Binary is newer hardfork than has been applied
      && ( witness.hardfork_version_vote != hf_versions.versions[ hfp.last_hardfork + 1 ] || witness.hardfork_time_vote != hf_versions.times[ hfp.last_hardfork + 1 ] ) ) // Witness vote does not match binary configuration
   {
      // Make vote match binary configuration
      pending_block.extensions.insert( block_header_extensions( hardfork_version_vote( hf_versions.versions[ hfp.last_hardfork + 1 ], hf_versions.times[ hfp.last_hardfork + 1 ] ) ) );
   }
   else if( hfp.current_hardfork_version == XGT_BLOCKCHAIN_VERSION // Binary does not know of a new hardfork
            && witness.hardfork_version_vote > XGT_BLOCKCHAIN_VERSION ) // Voting for hardfork in the future, that we do not know of...
   {
      // Make vote match binary configuration. This is vote to not apply the new hardfork.
      pending_block.extensions.insert( block_header_extensions( hardfork_version_vote( hf_versions.versions[ hfp.last_hardfork ], hf_versions.times[ hfp.last_hardfork ] ) ) );
   }
}

void block_producer::apply_pending_transactions(
   const chain::wallet_name_type& witness_recovery,
   fc::time_point_sec when,
   chain::signed_block& pending_block,
   fc::optional< xgt::chain::signed_transaction > block_reward
)
{
   size_t total_block_size = fc::raw::pack_size( pending_block );
   total_block_size += sizeof( uint32_t ); // Transaction vector length
   total_block_size += sizeof( uint32_t ); // Required automated actions vector length
   total_block_size += sizeof( uint32_t ); // Optional automated actions vector length
   const auto& gpo = _db.get_dynamic_global_properties();
   uint64_t maximum_block_size = gpo.maximum_block_size; //XGT_MAX_BLOCK_SIZE;
   uint64_t maximum_transaction_partition_size = maximum_block_size -  ( maximum_block_size * gpo.required_actions_partition_percent ) / XGT_100_PERCENT;

   //
   // The following code throws away existing pending_tx_session and
   // rebuilds it by re-applying pending transactions.
   //
   // This rebuild is necessary because pending transactions' validity
   // and semantics may have changed since they were received, because
   // time-based semantics are evaluated based on the current block
   // time.  These changes can only be reflected in the database when
   // the value of the "when" variable is known, which means we need to
   // re-apply pending transactions in this method.
   //
   _db.pending_transaction_session().reset();
   _db.pending_transaction_session() = _db.start_undo_session();

   //FC_TODO( "Safe to remove after HF20 occurs because no more pre HF20 blocks will be generated" );
   /// modify current witness so transaction evaluators can know who included the transaction
   _db.modify(
          _db.get_dynamic_global_properties(),
          [&]( chain::dynamic_global_property_object& dgp )
          {
             dgp.current_witness = witness_recovery;
          });


   uint64_t postponed_tx_count = 0;


   // postpone transaction if it would make block too big

   if (block_reward)
   {
      uint64_t new_total_size = total_block_size + fc::raw::pack_size( *block_reward );
      if (new_total_size >= maximum_transaction_partition_size)
      {
          postponed_tx_count++;
      }
      else
      {
          try
          {
              auto temp_session = _db.start_undo_session();
              _db.apply_transaction(*block_reward, _db.get_node_properties().skip_flags);
              temp_session.squash();

              total_block_size = new_total_size;
              pending_block.transactions.push_back(*block_reward);
          }
          catch (const fc::exception& e)
          {
            wlog("Block reward pow op transaction failed: ${e}", ("e", e));
            throw e;
          }
      }
   }

   // pop pending state (reset to head block state)
   for( const chain::signed_transaction& tx : _db._pending_tx )
   {
      // Only include transactions that have not expired yet for currently generating block,
      // this should clear problem transactions and allow block production to continue

      if( postponed_tx_count > XGT_BLOCK_GENERATION_POSTPONED_TX_LIMIT )
         break;

      if( tx.expiration < when )
         continue;

      if (block_reward) {
         bool contains_pow = false;
         for (auto& op : tx.operations) {
            if (protocol::is_pow_operation(op)) {
               contains_pow = true;
            }
         }
         if (contains_pow) {
            wlog("Skipping pow tx's from others");
            continue;
         }
      }

      uint64_t new_total_size = total_block_size + fc::raw::pack_size( tx );

      // postpone transaction if it would make block too big
      if( new_total_size >= maximum_transaction_partition_size )
      {
         postponed_tx_count++;
         continue;
      }

      try
      {
         auto temp_session = _db.start_undo_session();
         _db.apply_transaction( tx, _db.get_node_properties().skip_flags );
         temp_session.squash();

         total_block_size = new_total_size;
         pending_block.transactions.push_back( tx );
      }
      catch ( const fc::exception& e )
      {
         // Do nothing, transaction will not be re-applied
         //wlog( "Transaction was not processed while generating block due to ${e}", ("e", e) );
         //wlog( "The transaction was ${t}", ("t", tx) );
      }
   }
   if( postponed_tx_count > 0 )
   {
      wlog( "Postponed ${n} transactions due to block size limit", ("n", _db._pending_tx.size() - pending_block.transactions.size()) );
   }

   _db.update_global_dynamic_data( pending_block );

   const auto& pending_required_action_idx = _db.get_index< chain::pending_required_action_index, chain::by_execution >();
   auto pending_required_itr = pending_required_action_idx.begin();
   chain::required_automated_actions required_actions;

   while( pending_required_itr != pending_required_action_idx.end() && pending_required_itr->execution_time <= when )
   {
      uint64_t new_total_size = total_block_size + fc::raw::pack_size( pending_required_itr->action );

      // required_actions_partizion_size is a lower bound of requirement.
      // If we have extra space to include actions we should use it.
      if( new_total_size > maximum_block_size )
         break;

      try
      {
         auto temp_session = _db.start_undo_session();
         _db.apply_required_action( pending_required_itr->action );
         temp_session.squash();
         total_block_size = new_total_size;
         required_actions.push_back( pending_required_itr->action );

#ifdef ENABLE_MIRA
         auto old = pending_required_itr++;
         if( !( pending_required_itr != pending_required_action_idx.end() && pending_required_itr->execution_time <= when ) )
         {
            pending_required_itr = pending_required_action_idx.iterator_to( *old );
            ++pending_required_itr;
         }
#else
         ++pending_required_itr;
#endif
      }
      catch( fc::exception& e )
      {
         FC_RETHROW_EXCEPTION( e, warn, "A required automatic action was rejected. ${a} ${e}", ("a", pending_required_itr->action)("e", e.to_detail_string()) );
      }
   }

   if( required_actions.size() )
   {
      pending_block.extensions.insert( required_actions );
   }

   const auto& pending_optional_action_idx = _db.get_index< chain::pending_optional_action_index, chain::by_execution >();
   auto pending_optional_itr = pending_optional_action_idx.begin();
   chain::optional_automated_actions optional_actions;
   vector< const chain::pending_optional_action_object* > attempted_actions;

   while( pending_optional_itr != pending_optional_action_idx.end() && pending_optional_itr->execution_time <= when )
   {
      uint64_t new_total_size = total_block_size + fc::raw::pack_size( pending_optional_itr->action );

      if( new_total_size > maximum_block_size )
         break;

      attempted_actions.push_back( &(*pending_optional_itr) );

      try
      {
         auto temp_session = _db.start_undo_session();
         _db.apply_optional_action( pending_optional_itr->action );
         temp_session.squash();
         total_block_size = new_total_size;
         optional_actions.push_back( pending_optional_itr->action );
      }
      catch( fc::exception& ) {}

      ++pending_optional_itr;
   }

   for( const auto* o : attempted_actions )
   {
      _db.remove( *o );
   }

   if( optional_actions.size() )
   {
      pending_block.extensions.insert( optional_actions );
   }

   _db.pending_transaction_session().reset();

   pending_block.transaction_merkle_root = pending_block.calculate_merkle_root();
}

} } } // xgt::plugins::witness
