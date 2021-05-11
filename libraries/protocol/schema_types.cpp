#include <xgt/schema/abstract_schema.hpp>
#include <xgt/schema/schema_impl.hpp>

#include <xgt/protocol/schema_types.hpp>

namespace xgt { namespace schema { namespace detail {

//////////////////////////////////////////////
// wallet_name_type                         //
//////////////////////////////////////////////

XGT_SCHEMA_DEFINE_CLASS_METHODS( schema_wallet_name_type_impl )

void schema_wallet_name_type_impl::get_deps( std::vector< std::shared_ptr< abstract_schema > >& deps )
{
}

void schema_wallet_name_type_impl::get_str_schema( std::string& s )
{
   if( str_schema != "" )
   {
      s = str_schema;
      return;
   }

   std::vector< std::shared_ptr< abstract_schema > > deps;
   get_deps( deps );

   std::string my_name;
   get_name( my_name );
   fc::mutable_variant_object mvo;
   mvo("name", my_name)
      ("type", "wallet_name_type")
      ;

   str_schema = fc::json::to_string( mvo );
   s = str_schema;
   return;
}

//////////////////////////////////////////////
// asset_symbol_type                        //
//////////////////////////////////////////////

XGT_SCHEMA_DEFINE_CLASS_METHODS( schema_asset_symbol_type_impl )

void schema_asset_symbol_type_impl::get_deps( std::vector< std::shared_ptr< abstract_schema > >& deps )
{
}

void schema_asset_symbol_type_impl::get_str_schema( std::string& s )
{
   if( str_schema != "" )
   {
      s = str_schema;
      return;
   }

   std::vector< std::shared_ptr< abstract_schema > > deps;
   get_deps( deps );

   std::string my_name;
   get_name( my_name );
   fc::mutable_variant_object mvo;
   mvo("name", my_name)
      ("type", "asset_symbol_type")
      ;

   str_schema = fc::json::to_string( mvo );
   s = str_schema;
   return;
}

} } }
