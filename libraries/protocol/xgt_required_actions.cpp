#include <xgt/protocol/validation.hpp>
#include <xgt/protocol/xgt_required_actions.hpp>

namespace xgt { namespace protocol {

void example_required_action::validate()const
{
   validate_wallet_name( account );
}

bool operator==( const example_required_action& lhs, const example_required_action& rhs )
{
   return lhs.account == rhs.account;
}

void xtt_ico_launch_action::validate() const
{
   validate_wallet_name( control_account );
   validate_xtt_symbol( symbol );
}

bool operator==( const xtt_ico_launch_action& lhs, const xtt_ico_launch_action& rhs )
{
   return
      lhs.control_account == rhs.control_account &&
      lhs.symbol == rhs.symbol;
}

void xtt_ico_evaluation_action::validate() const
{
   validate_wallet_name( control_account );
   validate_xtt_symbol( symbol );
}

bool operator==( const xtt_ico_evaluation_action& lhs, const xtt_ico_evaluation_action& rhs )
{
   return
      lhs.control_account == rhs.control_account &&
      lhs.symbol == rhs.symbol;
}

void xtt_token_launch_action::validate() const
{
   validate_wallet_name( control_account );
   validate_xtt_symbol( symbol );
}

bool operator==( const xtt_token_launch_action& lhs, const xtt_token_launch_action& rhs )
{
   return
      lhs.control_account == rhs.control_account &&
      lhs.symbol == rhs.symbol;
}

void xtt_refund_action::validate() const
{
   validate_wallet_name( contributor );
   validate_xtt_symbol( symbol );
}

bool operator==( const xtt_refund_action& lhs, const xtt_refund_action& rhs )
{
   return
      lhs.symbol == rhs.symbol &&
      lhs.contributor == rhs.contributor &&
      lhs.contribution_id == rhs.contribution_id &&
      lhs.refund == rhs.refund;
}

} } //xgt::protocol
