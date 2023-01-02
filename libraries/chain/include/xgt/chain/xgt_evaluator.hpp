#pragma once

#include <xgt/protocol/xgt_operations.hpp>

#include <xgt/chain/evaluator.hpp>

namespace xgt { namespace chain {

using namespace xgt::protocol;

XGT_DEFINE_EVALUATOR( wallet_create )
XGT_DEFINE_EVALUATOR( wallet_update )
XGT_DEFINE_EVALUATOR( transfer )
XGT_DEFINE_EVALUATOR( witness_update )
XGT_DEFINE_EVALUATOR( comment )
XGT_DEFINE_EVALUATOR( comment_options )
XGT_DEFINE_EVALUATOR( delete_comment )
XGT_DEFINE_EVALUATOR( vote )
XGT_DEFINE_EVALUATOR( custom )
XGT_DEFINE_EVALUATOR( custom_json )
XGT_DEFINE_EVALUATOR( pow )
XGT_DEFINE_EVALUATOR( report_over_production )
XGT_DEFINE_EVALUATOR( escrow_transfer )
XGT_DEFINE_EVALUATOR( escrow_approve )
XGT_DEFINE_EVALUATOR( escrow_dispute )
XGT_DEFINE_EVALUATOR( escrow_release )
XGT_DEFINE_EVALUATOR( xtt_setup )
XGT_DEFINE_EVALUATOR( xtt_setup_ico_tier )
XGT_DEFINE_EVALUATOR( xtt_set_setup_parameters )
XGT_DEFINE_EVALUATOR( xtt_set_runtime_parameters )
XGT_DEFINE_EVALUATOR( xtt_create )
XGT_DEFINE_EVALUATOR( xtt_contribute )
XGT_DEFINE_EVALUATOR( contract_create )
XGT_DEFINE_EVALUATOR( contract_invoke )
XGT_DEFINE_EVALUATOR( contract_deploy )
XGT_DEFINE_EVALUATOR( contract_call )

} } // xgt::chain
