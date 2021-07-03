#pragma once

#include <fc/time.hpp>

#include <xgt/chain/database.hpp>

namespace xgt { namespace plugins { namespace chain {

class abstract_block_producer {
public:
   virtual ~abstract_block_producer() = default;

   virtual xgt::chain::signed_block generate_block(
      fc::time_point_sec when,
      const xgt::chain::wallet_name_type& witness_recovery,
      const fc::ecc::private_key& block_signing_private_key,
      const xgt::chain::signed_transaction& block_reward,
      uint32_t skip = xgt::chain::database::skip_nothing) = 0;
};

} } } // xgt::plugins::chain
