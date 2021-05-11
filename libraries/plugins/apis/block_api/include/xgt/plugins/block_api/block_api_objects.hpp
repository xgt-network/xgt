#pragma once

#include <xgt/chain/wallet_object.hpp>
#include <xgt/chain/block_summary_object.hpp>
#include <xgt/chain/comment_object.hpp>
#include <xgt/chain/global_property_object.hpp>
#include <xgt/chain/history_object.hpp>
#include <xgt/chain/xgt_objects.hpp>
#include <xgt/chain/transaction_object.hpp>
#include <xgt/chain/witness_objects.hpp>
#include <xgt/chain/database.hpp>

namespace xgt { namespace plugins { namespace block_api {

using namespace xgt::chain;

struct api_signed_block_object : public signed_block
{
   api_signed_block_object( const signed_block& block ) : signed_block( block )
   {
      block_id = id();
      signing_key = signee();
      transaction_ids.reserve( transactions.size() );
      for( const signed_transaction& tx : transactions )
         transaction_ids.push_back( tx.id() );
   }
   api_signed_block_object() {}

   block_id_type                 block_id;
   public_key_type               signing_key;
   vector< transaction_id_type > transaction_ids;
};

} } } // xgt::plugins::database_api

FC_REFLECT_DERIVED( xgt::plugins::block_api::api_signed_block_object, (xgt::protocol::signed_block),
                     (block_id)
                     (signing_key)
                     (transaction_ids)
                  )
