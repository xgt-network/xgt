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

void initialize_core_indexes2( database& db )
{
   XGT_ADD_CORE_INDEX(db, comment_vote_index);
   XGT_ADD_CORE_INDEX(db, witness_vote_index);
   XGT_ADD_CORE_INDEX(db, operation_index);
   XGT_ADD_CORE_INDEX(db, account_history_index);
   XGT_ADD_CORE_INDEX(db, hardfork_property_index);
   XGT_ADD_CORE_INDEX(db, recovery_authority_history_index);
   XGT_ADD_CORE_INDEX(db, account_recovery_request_index);
   XGT_ADD_CORE_INDEX(db, change_recovery_account_request_index);
   XGT_ADD_CORE_INDEX(db, escrow_index);
}

} }
