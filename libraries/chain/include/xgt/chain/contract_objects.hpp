#pragma once

#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/protocol/authority.hpp>
#include <xgt/protocol/xgt_operations.hpp>

#include <xgt/chain/xgt_object_types.hpp>

namespace xgt { namespace chain {

   class contract_object : public object< contract_object_type, contract_object >
   {
      public:
      contract_object() = default;

      contract_id_type id;
      wallet_name_type owner; // Creator of wallet
      contract_hash_type contract_hash;
      wallet_name_type wallet;
      vector<char> code;
   };

   struct by_id;
   struct by_contract_hash;
   struct by_wallet;
   struct by_owner_and_contract_hash;

   typedef multi_index_container<
      contract_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< contract_object, contract_id_type, &contract_object::id > >,
         ordered_unique< tag< by_wallet >, member< contract_object, wallet_name_type, &contract_object::wallet > >,
         ordered_unique< tag< by_owner >, member< contract_object, wallet_name_type, &contract_object::owner > >,
         ordered_unique< tag< by_contract_hash >, member< contract_object, contract_hash_type, &contract_object::contract_hash > >
      >
   > contract_index;

   // class contract_log_object : public object< contract_log_object_type, contract_log_object >
   // {
   //    public:
   //    contract_log_object() = default;

   //    contract_log_id_type id;
   //    contract_id_type contract_id;
   //    wallet_name_type owner;
   //    uint8_t level;
   //    std::tuple< uint256_t, uint256_t, uint256_t, uint256_t > topics;
   //    vector<uint8_t> data;
   // };

   // struct by_contract_id;
   // // TODO: by_topic, by_level

   // // TODO: block id, transaction id, transaction op index?
   // // TODO: Receipt object
   // // https://github.com/ethereum/go-ethereum/tree/master/core/types

   // typedef multi_index_container<
   //    contract_log_object,
   //    indexed_by<
   //       ordered_unique< tag< by_contract_id >, member< contract_log_object, contract_id_type, &contract_log_object::contract_id > >
   //    >
   // > contract_log_index;

   class contract_receipt_object : public object< contract_receipt_object_type, contract_receipt_object >
   {
      public:
      contract_receipt_object() = default;

      contract_log_id_type id;
      contract_hash_type contract_hash;
      wallet_name_type owner;
      std::vector<fc::sha256> topics;
      vector<uint8_t> data;
   };

   typedef multi_index_container<
      contract_log_object,
      indexed_by<
         ordered_unique< tag< by_contract_hash >, member< contract_receipt_object, contract_hash_type, &contract_receipt_object::contract_hash > >
      >
   > contract_receipt_index;

   // class contract_storage_object : public object< contract_storage_object_type, contract_storage_object >
   // {
   //    public:
   //    contract_storage_object() = default;

   //    contract_storage_id_type id;
   //    contract_id_type contract_id;
   //    wallet_name_type owner; // TODO: caller
   //    vector<uint8_t> data;
   // };

   // struct by_contract_id;
   // // TODO: By storage id and by owner id?

   // typedef multi_index_container<
   //    contract_storage_object,
   //    indexed_by<
   //       ordered_unique< tag< by_contract_id >, member< contract_storage_object, contract_id_type, &contract_storage_object::contract_id > >
   //    >
   // > contract_storage_index;

} }

FC_REFLECT( xgt::chain::contract_object,
      (id)
      (owner)
      (contract_hash)
      (code)
      )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::contract_object, xgt::chain::contract_index )

FC_REFLECT( xgt::chain::contract_log_object,
      (id)
      (contract_hash)
      (owner)
      (topics)
      (data)
      )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::contract_log_object, xgt::chain::contract_log_index )

 FC_REFLECT( xgt::chain::contract_storage_object,
      (id)
      (contract)
      (caller)
      (data) )
 CHAINBASE_SET_INDEX_TYPE( xgt::chain::contract_storage_object, xgt::chain::contract_storage_index )
