#include <xgt/protocol/xtt_util.hpp>
#include <xgt/protocol/authority.hpp>

namespace xgt { namespace protocol { namespace xtt {

namespace unit_target {

bool is_contributor( const unit_target_type& unit_target )
{
   return unit_target == XTT_DESTINATION_FROM || unit_target == XTT_DESTINATION_FROM_VESTING;
}

bool is_wallet_name_type( const unit_target_type& unit_target )
{
   if ( is_contributor( unit_target ) )
      return false;
   return true;
}

wallet_name_type get_unit_target_account( const unit_target_type& unit_target )
{
   FC_ASSERT( !is_contributor( unit_target ),  "Cannot derive an account name from a contributor special destination." );

   if ( is_valid_wallet_name( unit_target ) )
      return wallet_name_type( unit_target );

   // This is a special unit target destination in the form of $alice.vesting
   FC_ASSERT( unit_target.size() > std::strlen( XTT_DESTINATION_ACCOUNT_PREFIX ) + std::strlen( XTT_DESTINATION_VESTING_SUFFIX ),
      "Unit target '${target}' is malformed", ("target", unit_target) );
   std::string str_name = unit_target;
   auto pos = str_name.find( XTT_DESTINATION_ACCOUNT_PREFIX );
   FC_ASSERT( pos != std::string::npos && pos == 0, "Expected XTT destination account prefix '${prefix}' for unit target '${target}'.",
      ("prefix", XTT_DESTINATION_ACCOUNT_PREFIX)("target", unit_target) );

   std::size_t suffix_len = std::strlen( XTT_DESTINATION_VESTING_SUFFIX );
   FC_ASSERT( !str_name.compare( str_name.size() - suffix_len, suffix_len, XTT_DESTINATION_VESTING_SUFFIX ),
      "Expected XTT destination vesting suffix '${suffix}' for unit target '${target}'.",
         ("suffix", XTT_DESTINATION_VESTING_SUFFIX)("target", unit_target) );

   std::size_t prefix_len = std::strlen( XTT_DESTINATION_ACCOUNT_PREFIX );
   wallet_name_type unit_target_account = str_name.substr( prefix_len, str_name.size() - suffix_len - prefix_len );
   FC_ASSERT( is_valid_wallet_name( unit_target_account ), "The derived unit target account name '${name}' is invalid.",
      ("name", unit_target_account) );

   return unit_target_account;
}

} // xgt::protocol::xtt::unit_target

} } } // xgt::protocol::xtt
