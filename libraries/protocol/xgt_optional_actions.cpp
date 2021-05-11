#include <xgt/protocol/validation.hpp>
#include <xgt/protocol/xgt_optional_actions.hpp>
#include <xgt/protocol/xtt_util.hpp>

namespace xgt { namespace protocol {

void example_optional_action::validate()const
{
   validate_wallet_name( wallet );
}

} } //xgt::protocol
