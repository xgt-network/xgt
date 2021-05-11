#pragma once

#include <xgt/chain/database.hpp>
#include <xgt/protocol/asset_symbol.hpp>

namespace xgt { namespace chain {

   void replenish_nai_pool( database& db );
   void remove_from_nai_pool( database &db, const asset_symbol_type& a );

} } // xgt::chain
