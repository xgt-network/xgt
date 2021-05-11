#pragma once

#include <xgt/protocol/xgt_optional_actions.hpp>

#include <xgt/protocol/operation_util.hpp>

namespace xgt { namespace protocol {

   /** NOTE: do not change the order of any actions or it will trigger a hardfork.
    */
   typedef fc::static_variant<
            example_optional_action
         > optional_automated_action;

} } // xgt::protocol

XGT_DECLARE_OPERATION_TYPE( xgt::protocol::optional_automated_action );

FC_REFLECT_TYPENAME( xgt::protocol::optional_automated_action );
