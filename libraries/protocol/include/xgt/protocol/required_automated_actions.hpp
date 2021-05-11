#pragma once

#include <xgt/protocol/xgt_required_actions.hpp>

#include <xgt/protocol/operation_util.hpp>

namespace xgt { namespace protocol {

   /** NOTE: do not change the order of any actions or it will trigger a hardfork.
    */
   typedef fc::static_variant<
            xtt_ico_launch_action,
            xtt_ico_evaluation_action,
            xtt_token_launch_action,
            xtt_refund_action,
            example_required_action
         > required_automated_action;

} } // xgt::protocol

XGT_DECLARE_OPERATION_TYPE( xgt::protocol::required_automated_action );

FC_REFLECT_TYPENAME( xgt::protocol::required_automated_action );
