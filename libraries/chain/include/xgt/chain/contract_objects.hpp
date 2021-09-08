#pragma once

#include <boost/multiprecision/cpp_int.hpp>

#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/protocol/authority.hpp>
#include <xgt/protocol/xgt_operations.hpp>

#include <xgt/chain/xgt_object_types.hpp>

namespace xgt { namespace chain {

   using uint256_t = boost::multiprecision::uint256_t;

   class contract_object : public object< contract_object_type, contract_object >
   {
      XGT_STD_ALLOCATOR_CONSTRUCTOR( contract_object )

      template< typename Constructor, typename Allocator >
      contract_object( Constructor&& c, allocator< Allocator > a )
      {
         c( *this );
      }

      contract_id_type id;
      wallet_name_type owner; // Creator of wallet
      wallet_name_type wallet; // New wallet associated with contract
      contract_hash_type contract_hash;
      vector<char> code;
   };

   struct by_id;
   struct by_owner;
   struct by_wallet;
   struct by_contract_hash;

   typedef multi_index_container<
      contract_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< contract_object, contract_id_type, &contract_object::id > >,
         ordered_unique< tag< by_owner >, member< contract_object, wallet_name_type, &contract_object::owner > >,
         ordered_unique< tag< by_wallet >, member< contract_object, wallet_name_type, &contract_object::wallet > >,
         ordered_unique< tag< by_contract_hash >, member< contract_object, contract_hash_type, &contract_object::contract_hash > >
      >,
      allocator< contract_object >
   > contract_index;

   // class contract_log_object : public object< contract_log_object_type, contract_log_object >
   // {
   //    XGT_STD_ALLOCATOR_CONSTRUCTOR( contract_log_object )

   //    template< typename Constructor, typename Allocator >
   //    contract_log_object( Constructor&& c, allocator< Allocator > a )
   //    {
   //       c( *this );
   //    }

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
   //    >,
   //    allocator< contract_log_object >
   // > contract_log_index;

   class contract_receipt_object : public object< contract_receipt_object_type, contract_receipt_object >
   {
      XGT_STD_ALLOCATOR_CONSTRUCTOR( contract_receipt_object )

      template< typename Constructor, typename Allocator >
      contract_receipt_object( Constructor&& c, allocator< Allocator > a )
      {
         c( *this );
      }

      contract_receipt_id_type id;
      contract_hash_type contract_hash;
      wallet_name_type caller;
      vector< vector<char> > args;
      // uint64_t energy_used; // TODO: Verify this
   };

   typedef multi_index_container<
      contract_receipt_object,
      indexed_by<
         ordered_unique< tag< by_contract_hash >, member< contract_receipt_object, contract_hash_type, &contract_receipt_object::contract_hash > >
      >,
      allocator< contract_receipt_object >
   > contract_receipt_index;

    class contract_storage_object : public object< contract_storage_object_type, contract_storage_object >
    {
       XGT_STD_ALLOCATOR_CONSTRUCTOR( contract_storage_object )

       template< typename Constructor, typename Allocator >
       contract_storage_object( Constructor&& c, allocator< Allocator > a )
       {
          c( *this );
       }

       contract_storage_id_type id;
       contract_hash_type contract;
       wallet_name_type caller;
       vector<uint8_t> data;
    };

    struct by_contract;
    struct by_caller;

    typedef multi_index_container<
       contract_storage_object,
       indexed_by<
          ordered_unique< tag< by_contract >, member< contract_storage_object, contract_hash_type, &contract_storage_object::contract > >,
          ordered_unique< tag< by_caller >, member< contract_storage_object, wallet_name_type, &contract_storage_object::caller > >
       >,
       allocator< contract_storage_object >
    > contract_storage_index;

} }

FC_REFLECT( xgt::chain::contract_object,
      (owner)
      (code)
      )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::contract_object, xgt::chain::contract_index )

// FC_REFLECT( xgt::chain::contract_log_object,
//       (id)
//       (contract_id)
//       (owner)
//       (level)
//       (topics)
//       (data) )
// CHAINBASE_SET_INDEX_TYPE( xgt::chain::contract_log_object, xgt::chain::contract_log_index )

FC_REFLECT( xgt::chain::contract_receipt_object,
      (contract_hash)
      (caller)
      (args)
      // (energy_used)
      )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::contract_receipt_object, xgt::chain::contract_receipt_index )

 FC_REFLECT( xgt::chain::contract_storage_object,
      (id)
      (contract)
      (caller)
      (data) )
 CHAINBASE_SET_INDEX_TYPE( xgt::chain::contract_storage_object, xgt::chain::contract_storage_index )
