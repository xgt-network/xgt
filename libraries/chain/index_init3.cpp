#include <xgt/chain/xgt_object_types.hpp>

#include <xgt/chain/index.hpp>

#include <xgt/chain/block_summary_object.hpp>
#include <xgt/chain/history_object.hpp>
#include <xgt/chain/pending_required_action_object.hpp>
#include <xgt/chain/pending_optional_action_object.hpp>
#include <xgt/chain/xtt_objects.hpp>
#include <xgt/chain/xgt_objects.hpp>
#include <xgt/chain/transaction_object.hpp>

namespace xgt { namespace chain {

void initialize_core_indexes3( database& db )
{
   XGT_ADD_CORE_INDEX(db, pending_required_action_index);
   XGT_ADD_CORE_INDEX(db, pending_optional_action_index);
   XGT_ADD_CORE_INDEX(db, xtt_token_index);
   XGT_ADD_CORE_INDEX(db, account_regular_balance_index);
   XGT_ADD_CORE_INDEX(db, nai_pool_index);
   XGT_ADD_CORE_INDEX(db, xtt_contribution_index);
   XGT_ADD_CORE_INDEX(db, xtt_ico_index);
   XGT_ADD_CORE_INDEX(db, xtt_ico_tier_index);
   XGT_ADD_CORE_INDEX(db, contract_index);
}

} }
