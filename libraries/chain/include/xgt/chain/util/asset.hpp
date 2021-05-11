#pragma once

#include <xgt/protocol/asset.hpp>

namespace xgt { namespace chain { namespace util {

using xgt::protocol::asset;
using xgt::protocol::price;

inline asset to_sbd( const price& p, const asset& xgt )
{
   FC_ASSERT( xgt.symbol == XGT_SYMBOL );
   if( p.is_null() )
      return asset( 0, SBD_SYMBOL );
   return xgt * p;
}

inline asset to_xgt( const price& p, const asset& sbd )
{
   FC_ASSERT( sbd.symbol == SBD_SYMBOL );
   if( p.is_null() )
      return asset( 0, XGT_SYMBOL );
   return sbd * p;
}

} } }
