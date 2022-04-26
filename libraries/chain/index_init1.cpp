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
}

} }
