
#pragma once

#include <xgt/schema/abstract_schema.hpp>
#include <xgt/schema/schema_impl.hpp>

#include <xgt/protocol/types.hpp>

namespace xgt {
   namespace schema {
      namespace detail {

         //////////////////////////////////////////////
         // wallet_name_type                         //
         //////////////////////////////////////////////

         struct schema_wallet_name_type_impl : public abstract_schema
         {
            XGT_SCHEMA_CLASS_BODY( schema_wallet_name_type_impl )
         };

      }

      template<> struct schema_reflect< xgt::protocol::wallet_name_type >
      {
         typedef detail::schema_wallet_name_type_impl schema_impl_type;
      };

   }
}

namespace fc {

   template<> struct get_typename< xgt::protocol::wallet_name_type >
   {
      static const char* name()
      {
         return "xgt::protocol::wallet_name_type";
      }
   };

}

namespace xgt {
   namespace schema {
      namespace detail {

         //////////////////////////////////////////////
         // address_ripemd160_type                   //
         //////////////////////////////////////////////

         struct schema_address_ripemd160_type_impl : public abstract_schema
         {
            XGT_SCHEMA_CLASS_BODY( schema_address_ripemd160_type_impl )
         };

      }

   }
}
