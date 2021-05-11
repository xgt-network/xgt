#pragma once

#include <xgt/chain/xgt_object_types.hpp>

#ifndef XGT_WITNESS_SPACE_ID
#define XGT_WITNESS_SPACE_ID 19
#endif

namespace xgt { namespace chain {
struct by_account;
} }

namespace xgt { namespace plugins { namespace witness {

using namespace xgt::chain;

enum witness_object_types
{
   witness_custom_op_object_type = ( XGT_WITNESS_SPACE_ID << 8 )
};

class witness_custom_op_object : public object< witness_custom_op_object_type, witness_custom_op_object >
{
   public:
      template< typename Constructor, typename Allocator >
      witness_custom_op_object( Constructor&& c, allocator< Allocator > a )
      {
         c( *this );
      }

      witness_custom_op_object() {}

      id_type              id;
      wallet_name_type     account;
      uint32_t             count = 0;
};

typedef multi_index_container<
   witness_custom_op_object,
   indexed_by<
      ordered_unique< tag< by_id >, member< witness_custom_op_object, witness_custom_op_object::id_type, &witness_custom_op_object::id > >,
      ordered_unique< tag< by_account >, member< witness_custom_op_object, wallet_name_type, &witness_custom_op_object::account > >
   >,
   allocator< witness_custom_op_object >
> witness_custom_op_index;

} } }

FC_REFLECT( xgt::plugins::witness::witness_custom_op_object,
   (id)
   (account)
   (count)
   )
CHAINBASE_SET_INDEX_TYPE( xgt::plugins::witness::witness_custom_op_object, xgt::plugins::witness::witness_custom_op_index )
