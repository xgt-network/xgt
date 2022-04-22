#pragma once

#include <xgt/chain/xgt_object_types.hpp>

namespace xgt { namespace plugins { namespace wallet_by_key {

using namespace std;
using namespace xgt::chain;

#ifndef XGT_WALLET_BY_KEY_SPACE_ID
#define XGT_WALLET_BY_KEY_SPACE_ID 11
#endif

enum wallet_by_key_object_types
{
   key_lookup_object_type = ( XGT_WALLET_BY_KEY_SPACE_ID << 8 )
};

class key_lookup_object : public object< key_lookup_object_type, key_lookup_object >
{
   public:
      key_lookup_object() {}

      id_type           id;

      public_key_type   key;
      wallet_name_type  account;
};

typedef key_lookup_object::id_type key_lookup_id_type;

struct by_key;

typedef multi_index_container<
   key_lookup_object,
   indexed_by<
      ordered_unique< tag< by_id >, member< key_lookup_object, key_lookup_id_type, &key_lookup_object::id > >,
      ordered_unique< tag< by_key >,
         composite_key< key_lookup_object,
            member< key_lookup_object, public_key_type, &key_lookup_object::key >
         >
      >
   >
> key_lookup_index;

} } } // xgt::plugins::wallet_by_key


FC_REFLECT( xgt::plugins::wallet_by_key::key_lookup_object, (id)(key)(account) )
CHAINBASE_SET_INDEX_TYPE( xgt::plugins::wallet_by_key::key_lookup_object, xgt::plugins::wallet_by_key::key_lookup_index )
