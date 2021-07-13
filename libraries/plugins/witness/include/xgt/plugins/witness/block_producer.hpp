#pragma once

#include <xgt/chain/xgt_fwd.hpp>
#include <fc/time.hpp>

#include <xgt/plugins/chain/abstract_block_producer.hpp>
#include <xgt/plugins/chain/chain_plugin.hpp>

namespace xgt { namespace plugins { namespace witness {

class block_producer : public chain::abstract_block_producer {
public:
   block_producer( chain::database& db ) : _db( db ) {}

   /**
    * This function contains block generation logic.
    *
    * Calling this function invokes a lockless write and therefore
    * should be avoided. Instead, one should call chain_plugin::generate_block().
    */
   chain::signed_block generate_block(
      fc::time_point_sec when,
      const chain::wallet_name_type& witness_recovery,
      const fc::ecc::private_key& block_signing_private_key,
      fc::optional< xgt::chain::signed_transaction > trx,
      uint32_t skip = chain::database::skip_nothing
   );

private:
   chain::database& _db;

   chain::signed_block _generate_block(
      fc::time_point_sec when,
      const chain::wallet_name_type& witness_recovery,
      const fc::ecc::private_key& block_signing_private_key,
      fc::optional< xgt::chain::signed_transaction > block_reward
   );

   void adjust_hardfork_version_vote( const chain::witness_object& witness, chain::signed_block& pending_block );

   void apply_pending_transactions(
      const chain::wallet_name_type& witness_recovery,
      fc::time_point_sec when,
      chain::signed_block& pending_block,
      fc::optional< xgt::chain::signed_transaction > trx
   );
};

} } } // xgt::plugins::witness
