#pragma once

#include <xgt/chain/xgt_fwd.hpp>

#include <fc/fixed_string.hpp>

#include <xgt/protocol/authority.hpp>
#include <xgt/protocol/xgt_operations.hpp>

#include <xgt/chain/xgt_object_types.hpp>
#include <xgt/chain/witness_objects.hpp>
#include <xgt/chain/shared_authority.hpp>
#include <xgt/chain/util/energybar.hpp>

#include <numeric>

namespace xgt { namespace chain {

   using xgt::protocol::authority;

   class wallet_object : public object< wallet_object_type, wallet_object >
   {
         XGT_STD_ALLOCATOR_CONSTRUCTOR( wallet_object )
         public:

         id_type           id;

         wallet_name_type  name;
         public_key_type   memo_key;

         time_point_sec    last_account_update;

         time_point_sec    created;
         bool              mined = true;
         wallet_name_type  recovery_account;
         wallet_name_type  reset_account = XGT_NULL_WALLET;
         time_point_sec    last_account_recovery;
         uint32_t          comment_count = 0;
         uint32_t          lifetime_vote_count = 0;
         uint32_t          post_count = 0;

         bool              can_vote = true;
         util::energybar   energybar;

         asset             balance = asset( 0, XGT_SYMBOL );  ///< total liquid shares held by this account

         uint16_t          witnesses_voted_for = 0;

         time_point_sec    last_post;
         time_point_sec    last_root_post = fc::time_point_sec::min();
         time_point_sec    last_post_edit;
         time_point_sec    last_vote_time;
         uint32_t          post_bandwidth = 0;
   };

   class account_metadata_object : public object< account_metadata_object_type, account_metadata_object >
   {
      XGT_STD_ALLOCATOR_CONSTRUCTOR( account_metadata_object )
      public:

      id_type           id;
      wallet_id_type    account;
      std::string     json_metadata;
      std::string     social_json_metadata;
   };

   class account_authority_object : public object< account_authority_object_type, account_authority_object >
   {
      XGT_STD_ALLOCATOR_CONSTRUCTOR( account_authority_object )

      public:
         id_type           id;

         wallet_name_type  account;

         shared_authority  recovery; ///< used for backup control, can set recovery or money
         shared_authority  money;    ///< used for all monetary operations, can set money or social
         shared_authority  social;   ///< used for voting and social

         time_point_sec    last_recovery_update;
   };

   class recovery_authority_history_object : public object< recovery_authority_history_object_type, recovery_authority_history_object >
   {
      XGT_STD_ALLOCATOR_CONSTRUCTOR( recovery_authority_history_object )

      public:
         id_type           id;

         wallet_name_type  account;
         shared_authority  previous_recovery_authority;
         time_point_sec    last_valid_time;
   };

   class account_recovery_request_object : public object< account_recovery_request_object_type, account_recovery_request_object >
   {
      XGT_STD_ALLOCATOR_CONSTRUCTOR( account_recovery_request_object )

      public:
         id_type           id;

         wallet_name_type  account_to_recover;
         shared_authority  new_recovery_authority;
         time_point_sec    expires;
   };

   class change_recovery_account_request_object : public object< change_recovery_account_request_object_type, change_recovery_account_request_object >
   {
      XGT_STD_ALLOCATOR_CONSTRUCTOR( change_recovery_account_request_object )

      public:
         id_type           id;

         wallet_name_type  account_to_recover;
         wallet_name_type  recovery_account;
         time_point_sec    effective_on;
   };

   /**
   * @ingroup object_index
   */
   typedef multi_index_container<
      wallet_object,
         indexed_by<
            ordered_unique< tag< by_id >,
            member< wallet_object, wallet_id_type, &wallet_object::id > >,
         ordered_unique< tag< by_name >,
            member< wallet_object, wallet_name_type, &wallet_object::name > >
      >
   > wallet_index;

   struct by_account;

   typedef multi_index_container <
      account_metadata_object,
      indexed_by<
         ordered_unique< tag< by_id >,
            member< account_metadata_object, account_metadata_id_type, &account_metadata_object::id > >,
         ordered_unique< tag< by_account >,
            member< account_metadata_object, wallet_id_type, &account_metadata_object::account > >
      >
   > account_metadata_index;

   typedef multi_index_container <
      recovery_authority_history_object,
      indexed_by <
         ordered_unique< tag< by_id >,
            member< recovery_authority_history_object, recovery_authority_history_id_type, &recovery_authority_history_object::id > >,
         ordered_unique< tag< by_account >,
            composite_key< recovery_authority_history_object,
               member< recovery_authority_history_object, wallet_name_type, &recovery_authority_history_object::account >,
               member< recovery_authority_history_object, time_point_sec, &recovery_authority_history_object::last_valid_time >,
               member< recovery_authority_history_object, recovery_authority_history_id_type, &recovery_authority_history_object::id >
            >,
            composite_key_compare< std::less< wallet_name_type >, std::less< time_point_sec >, std::less< recovery_authority_history_id_type > >
         >
      >
   > recovery_authority_history_index;

   struct by_last_recovery_update;

   typedef multi_index_container <
      account_authority_object,
      indexed_by <
         ordered_unique< tag< by_id >,
            member< account_authority_object, account_authority_id_type, &account_authority_object::id > >,
         ordered_unique< tag< by_account >,
            composite_key< account_authority_object,
               member< account_authority_object, wallet_name_type, &account_authority_object::account >,
               member< account_authority_object, account_authority_id_type, &account_authority_object::id >
            >,
            composite_key_compare< std::less< wallet_name_type >, std::less< account_authority_id_type > >
         >,
         ordered_unique< tag< by_last_recovery_update >,
            composite_key< account_authority_object,
               member< account_authority_object, time_point_sec, &account_authority_object::last_recovery_update >,
               member< account_authority_object, account_authority_id_type, &account_authority_object::id >
            >,
            composite_key_compare< std::greater< time_point_sec >, std::less< account_authority_id_type > >
         >
      >
   > account_authority_index;

   struct by_expiration;

   typedef multi_index_container <
      account_recovery_request_object,
      indexed_by <
         ordered_unique< tag< by_id >,
            member< account_recovery_request_object, account_recovery_request_id_type, &account_recovery_request_object::id > >,
         ordered_unique< tag< by_account >,
            member< account_recovery_request_object, wallet_name_type, &account_recovery_request_object::account_to_recover >
         >,
         ordered_unique< tag< by_expiration >,
            composite_key< account_recovery_request_object,
               member< account_recovery_request_object, time_point_sec, &account_recovery_request_object::expires >,
               member< account_recovery_request_object, wallet_name_type, &account_recovery_request_object::account_to_recover >
            >,
            composite_key_compare< std::less< time_point_sec >, std::less< wallet_name_type > >
         >
      >
   > account_recovery_request_index;

   struct by_effective_date;

   typedef multi_index_container <
      change_recovery_account_request_object,
      indexed_by <
         ordered_unique< tag< by_id >,
            member< change_recovery_account_request_object, change_recovery_account_request_id_type, &change_recovery_account_request_object::id > >,
         ordered_unique< tag< by_account >,
            member< change_recovery_account_request_object, wallet_name_type, &change_recovery_account_request_object::account_to_recover >
         >,
         ordered_unique< tag< by_effective_date >,
            composite_key< change_recovery_account_request_object,
               member< change_recovery_account_request_object, time_point_sec, &change_recovery_account_request_object::effective_on >,
               member< change_recovery_account_request_object, wallet_name_type, &change_recovery_account_request_object::account_to_recover >
            >,
            composite_key_compare< std::less< time_point_sec >, std::less< wallet_name_type > >
         >
      >
   > change_recovery_account_request_index;
} }

namespace mira {

template<> struct is_static_length< xgt::chain::wallet_object > : public boost::true_type {};
template<> struct is_static_length< xgt::chain::change_recovery_account_request_object > : public boost::true_type {};

} // mira

FC_REFLECT( xgt::chain::wallet_object,
             (id)(name)(memo_key)(last_account_update)
             (created)(mined)
             (recovery_account)(last_account_recovery)(reset_account)
             (comment_count)(lifetime_vote_count)(post_count)(can_vote)(energybar)
             (balance)
             (witnesses_voted_for)
             (last_post)(last_root_post)(last_post_edit)(last_vote_time)(post_bandwidth)
          )

CHAINBASE_SET_INDEX_TYPE( xgt::chain::wallet_object, xgt::chain::wallet_index )

FC_REFLECT( xgt::chain::account_metadata_object,
             (id)(account)(json_metadata)(social_json_metadata) )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::account_metadata_object, xgt::chain::account_metadata_index )

FC_REFLECT( xgt::chain::account_authority_object,
             (id)(account)(recovery)(money)(social)(last_recovery_update)
)
CHAINBASE_SET_INDEX_TYPE( xgt::chain::account_authority_object, xgt::chain::account_authority_index )

FC_REFLECT( xgt::chain::recovery_authority_history_object,
             (id)(account)(previous_recovery_authority)(last_valid_time)
          )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::recovery_authority_history_object, xgt::chain::recovery_authority_history_index )

FC_REFLECT( xgt::chain::account_recovery_request_object,
             (id)(account_to_recover)(new_recovery_authority)(expires)
          )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::account_recovery_request_object, xgt::chain::account_recovery_request_index )

FC_REFLECT( xgt::chain::change_recovery_account_request_object,
             (id)(account_to_recover)(recovery_account)(effective_on)
          )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::change_recovery_account_request_object, xgt::chain::change_recovery_account_request_index )
