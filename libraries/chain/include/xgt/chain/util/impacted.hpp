#pragma once

#include <fc/container/flat.hpp>
#include <xgt/protocol/operations.hpp>
#include <xgt/protocol/transaction.hpp>

#include <fc/string.hpp>

namespace xgt { namespace app {

using namespace fc;

void operation_get_impacted_accounts(
   const xgt::protocol::operation& op,
   fc::flat_set<protocol::wallet_name_type>& result );

void transaction_get_impacted_accounts(
   const xgt::protocol::transaction& tx,
   fc::flat_set<protocol::wallet_name_type>& result
   );

} } // xgt::app
