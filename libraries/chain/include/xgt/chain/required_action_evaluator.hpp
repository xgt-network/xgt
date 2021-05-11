#pragma once

#include <xgt/protocol/xgt_required_actions.hpp>

#include <xgt/chain/evaluator.hpp>

namespace xgt { namespace chain {

using namespace xgt::protocol;

XGT_DEFINE_ACTION_EVALUATOR( example_required, required_automated_action )

XGT_DEFINE_ACTION_EVALUATOR( xtt_ico_launch, required_automated_action )
XGT_DEFINE_ACTION_EVALUATOR( xtt_ico_evaluation, required_automated_action )
XGT_DEFINE_ACTION_EVALUATOR( xtt_token_launch, required_automated_action )
XGT_DEFINE_ACTION_EVALUATOR( xtt_refund, required_automated_action )

} } //xgt::chain
