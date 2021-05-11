
#pragma once

#include <xgt/schema/abstract_schema.hpp>
#include <xgt/schema/schema_impl.hpp>

#include <xgt/protocol/asset_symbol.hpp>

namespace xgt { namespace schema { namespace detail {

//////////////////////////////////////////////
// asset_symbol_type                        //
//////////////////////////////////////////////

struct schema_asset_symbol_type_impl
   : public abstract_schema
{
   XGT_SCHEMA_CLASS_BODY( schema_asset_symbol_type_impl )
};

}

template<>
struct schema_reflect< xgt::protocol::asset_symbol_type >
{
   typedef detail::schema_asset_symbol_type_impl           schema_impl_type;
};

} }
