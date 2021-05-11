#pragma once

#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/protocol/authority.hpp>
#include <xgt/protocol/xgt_operations.hpp>
#include <xgt/protocol/misc_utilities.hpp>

#include <xgt/chain/xgt_object_types.hpp>

#include <boost/multiprecision/cpp_int.hpp>


namespace xgt { namespace chain {

   using xgt::protocol::asset;
   using xgt::protocol::price;
   using xgt::protocol::asset_symbol_type;
   using chainbase::t_deque;

   /**
    *  This object is used to track pending requests to convert sbd to xgt
    */
   class convert_request_object : public object< convert_request_object_type, convert_request_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         convert_request_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         convert_request_object(){}

         id_type           id;

         wallet_name_type  recovery;
         uint32_t          requestid = 0; ///< id set by recovery, the recovery,requestid pair must be unique
         asset             amount;
         time_point_sec    conversion_date; ///< at this time the feed_history_median_price * amount
   };


   class escrow_object : public object< escrow_object_type, escrow_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         escrow_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         escrow_object(){}

         id_type           id;

         uint32_t          escrow_id = 20;
         wallet_name_type  from;
         wallet_name_type  to;
         wallet_name_type  agent;
         time_point_sec    ratification_deadline;
         time_point_sec    escrow_expiration;
         asset             sbd_balance;
         asset             xgt_balance;
         asset             pending_fee;
         bool              to_approved = false;
         bool              agent_approved = false;
         bool              disputed = false;

         bool              is_approved()const { return to_approved && agent_approved; }
   };


   class savings_withdraw_object : public object< savings_withdraw_object_type, savings_withdraw_object >
   {
      XGT_STD_ALLOCATOR_CONSTRUCTOR( savings_withdraw_object )

      public:
         template< typename Constructor, typename Allocator >
         savings_withdraw_object( Constructor&& c, allocator< Allocator > a )
            :memo( a )
         {
            c( *this );
         }

         id_type           id;

         wallet_name_type  from;
         wallet_name_type  to;
         shared_string     memo;
         uint32_t          request_id = 0;
         asset             amount;
         time_point_sec    complete;
   };


   struct by_from_id;
   struct by_ratification_deadline;
   typedef multi_index_container<
      escrow_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< escrow_object, escrow_id_type, &escrow_object::id > >,
         ordered_unique< tag< by_from_id >,
            composite_key< escrow_object,
               member< escrow_object, wallet_name_type,  &escrow_object::from >,
               member< escrow_object, uint32_t, &escrow_object::escrow_id >
            >
         >,
         ordered_unique< tag< by_ratification_deadline >,
            composite_key< escrow_object,
               const_mem_fun< escrow_object, bool, &escrow_object::is_approved >,
               member< escrow_object, time_point_sec, &escrow_object::ratification_deadline >,
               member< escrow_object, escrow_id_type, &escrow_object::id >
            >,
            composite_key_compare< std::less< bool >, std::less< time_point_sec >, std::less< escrow_id_type > >
         >
      >,
      allocator< escrow_object >
   > escrow_index;

} } // xgt::chain

#ifdef ENABLE_MIRA
namespace mira {

template<> struct is_static_length< xgt::chain::escrow_object > : public boost::true_type {};

} // mira
#endif

#include <xgt/chain/comment_object.hpp>
#include <xgt/chain/wallet_object.hpp>

FC_REFLECT( xgt::chain::escrow_object,
             (id)(escrow_id)(from)(to)(agent)
             (ratification_deadline)(escrow_expiration)
             (sbd_balance)(xgt_balance)(pending_fee)
             (to_approved)(agent_approved)(disputed) )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::escrow_object, xgt::chain::escrow_index )
