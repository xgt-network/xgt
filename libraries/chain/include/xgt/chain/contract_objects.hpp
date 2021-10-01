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
      contract_hash_type contract_hash;
      vector<char> code;
   };

   struct by_id;
   struct by_contract_hash;
   struct by_owner_and_contract_hash;

   typedef multi_index_container<
      contract_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< contract_object, contract_id_type, &contract_object::id > >,
         ordered_unique< tag< by_contract_hash >, member< contract_object, contract_hash_type, &contract_object::contract_hash > >,
         ordered_unique< tag< by_owner_and_contract_hash >,
            composite_key< contract_object,
               member< contract_object, wallet_name_type, &contract_object::owner >,
               member< contract_object, contract_hash_type, &contract_object::contract_hash >
            >
         >
      >,
      allocator< contract_object >
   > contract_index;

   class contract_log_object : public object< contract_log_object_type, contract_log_object >
   {
      XGT_STD_ALLOCATOR_CONSTRUCTOR( contract_log_object )

      template< typename Constructor, typename Allocator >
      contract_log_object( Constructor&& c, allocator< Allocator > a )
      {
         c( *this );
      }

      contract_log_id_type id;
      contract_hash_type contract_hash;
      wallet_name_type owner;
      std::vector<uint256_t> topics;
      vector<uint8_t> data;
   };

   typedef multi_index_container<
      contract_log_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< contract_log_object, contract_log_id_type, &contract_log_object::id > >
      >,
      allocator< contract_log_object >
   > contract_log_index;

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
    struct by_contract_and_caller;

    typedef multi_index_container<
       contract_storage_object,
       indexed_by<
          ordered_unique< tag< by_contract >, member< contract_storage_object, contract_hash_type, &contract_storage_object::contract > >,
          ordered_unique< tag< by_caller >, member< contract_storage_object, wallet_name_type, &contract_storage_object::caller > >,
          ordered_unique< tag< by_contract_and_caller >,
             composite_key< contract_storage_object,
                member< contract_storage_object, contract_hash_type, &contract_storage_object::contract >,
                member< contract_storage_object, wallet_name_type, &contract_storage_object::caller >
             >,
             composite_key_compare< std::less< contract_hash_type >, std::less< wallet_name_type > >
          >
       >,
       allocator< contract_storage_object >
    > contract_storage_index;

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
      (data) )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::contract_log_object, xgt::chain::contract_log_index )

 FC_REFLECT( xgt::chain::contract_storage_object,
      (id)
      (contract)
      (caller)
      (data) )
 CHAINBASE_SET_INDEX_TYPE( xgt::chain::contract_storage_object, xgt::chain::contract_storage_index )
