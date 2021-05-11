#pragma once
#include <string>
#include <xgt/protocol/types.hpp>

#define XTT_DESTINATION_PREFIX         "$"
#define XTT_DESTINATION_ACCOUNT_PREFIX XTT_DESTINATION_PREFIX "!"
#define XTT_DESTINATION_VESTING_SUFFIX ".vesting"
#define XTT_DESTINATION_FROM           unit_target_type( XTT_DESTINATION_PREFIX "from" )
#define XTT_DESTINATION_FROM_VESTING   unit_target_type( XTT_DESTINATION_PREFIX "from" XTT_DESTINATION_VESTING_SUFFIX )
#define XTT_DESTINATION_REWARDS        unit_target_type( XTT_DESTINATION_PREFIX "rewards" )
#define XTT_DESTINATION_VESTING        unit_target_type( XTT_DESTINATION_PREFIX "vesting" )

namespace xgt { namespace protocol { namespace xtt {

namespace unit_target {

bool is_contributor( const unit_target_type& unit_target );

bool is_wallet_name_type( const unit_target_type& unit_target );

wallet_name_type get_unit_target_account( const unit_target_type& unit_target );

} // xgt::protocol::xtt::unit_target

} } } // xgt::protocol::xtt
