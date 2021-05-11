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

void initialize_core_indexes( database& db )
{
   XGT_ADD_CORE_INDEX(db, dynamic_global_property_index);
   XGT_ADD_CORE_INDEX(db, wallet_index);
   XGT_ADD_CORE_INDEX(db, account_metadata_index);
   XGT_ADD_CORE_INDEX(db, account_authority_index);
   XGT_ADD_CORE_INDEX(db, witness_index);
   XGT_ADD_CORE_INDEX(db, transaction_index);
   XGT_ADD_CORE_INDEX(db, block_summary_index);
   XGT_ADD_CORE_INDEX(db, comment_index);
   XGT_ADD_CORE_INDEX(db, comment_content_index);
   XGT_ADD_CORE_INDEX(db, comment_vote_index);
   XGT_ADD_CORE_INDEX(db, witness_vote_index);
   XGT_ADD_CORE_INDEX(db, operation_index);
   XGT_ADD_CORE_INDEX(db, account_history_index);
   XGT_ADD_CORE_INDEX(db, hardfork_property_index);
   XGT_ADD_CORE_INDEX(db, recovery_authority_history_index);
   XGT_ADD_CORE_INDEX(db, account_recovery_request_index);
   XGT_ADD_CORE_INDEX(db, change_recovery_account_request_index);
   XGT_ADD_CORE_INDEX(db, escrow_index);
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

index_info::index_info() {}
index_info::~index_info() {}

abstract_object::abstract_object() {}
abstract_object::~abstract_object() {}

} }
