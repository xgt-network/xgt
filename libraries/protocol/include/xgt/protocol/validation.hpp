#pragma once

#include <xgt/protocol/base.hpp>
#include <xgt/protocol/block_header.hpp>
#include <xgt/protocol/asset.hpp>

#include <fc/utf8.hpp>

namespace xgt { namespace protocol {

inline bool is_asset_type( asset asset, asset_symbol_type symbol )
{
   return asset.symbol == symbol;
}

inline void validate_wallet_name( const string& name )
{
   FC_ASSERT( is_valid_wallet_name( name ), "Account name ${n} is invalid", ("n", name) );
}

inline void validate_permlink( const string& permlink )
{
   FC_ASSERT( permlink.size() < XGT_MAX_PERMLINK_LENGTH, "permlink is too long" );
   FC_ASSERT( fc::is_utf8( permlink ), "permlink not formatted in UTF8" );
}

inline void validate_xtt_symbol( const asset_symbol_type& symbol )
{
   symbol.validate();
   FC_ASSERT( symbol.space() == asset_symbol_type::xtt_nai_space, "legacy symbol used instead of NAI" );
}

} }
