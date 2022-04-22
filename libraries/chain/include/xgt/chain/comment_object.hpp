#pragma once

#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/protocol/authority.hpp>
#include <xgt/protocol/xgt_operations.hpp>

#include <xgt/chain/xgt_object_types.hpp>
#include <xgt/chain/witness_objects.hpp>


namespace xgt { namespace chain {

   using chainbase::t_vector;
   using chainbase::t_flat_map;

   struct strcmp_less
   {
      bool operator()( const shared_string& a, const shared_string& b )const
      {
         return less( a.c_str(), b.c_str() );
      }

      private:
         inline bool less( const char* a, const char* b )const
         {
            return std::strcmp( a, b ) < 0;
         }
   };

   class comment_object : public object < comment_object_type, comment_object >
   {
      XGT_STD_ALLOCATOR_CONSTRUCTOR( comment_object )

      public:
         template< typename Constructor, typename Allocator >
         comment_object( Constructor&& c, allocator< Allocator > a ) :
            category( a ),
            parent_permlink( a ),
            permlink( a )
         {
            c( *this );
         }

         id_type           id;

         shared_string     category;
         wallet_name_type  parent_author;
         shared_string     parent_permlink;
         wallet_name_type  author;
         shared_string     permlink;

         time_point_sec    last_update;
         time_point_sec    created;
         time_point_sec    active; ///< the last time this post was "touched" by voting or reply

         uint16_t          depth = 0; ///< used to track max nested depth
         uint32_t          children = 0; ///< used to track the total number of children, grandchildren, etc...

         int32_t           net_votes = 0;

         id_type           root_comment;

         bool              allow_replies = true;      /// allows a post to disable replies.
         bool              allow_votes   = true;      /// allows a post to receive votes;
   };

   class comment_content_object : public object< comment_content_object_type, comment_content_object >
   {
      XGT_STD_ALLOCATOR_CONSTRUCTOR( comment_content_object )

      public:
         template< typename Constructor, typename Allocator >
         comment_content_object( Constructor&& c, allocator< Allocator > a ) :
            title( a ), body( a ), json_metadata( a )
         {
            c( *this );
         }

         id_type           id;

         comment_id_type   comment;

         shared_string     title;
         shared_string     body;
         shared_string     json_metadata;
   };

   class comment_vote_object : public object< comment_vote_object_type, comment_vote_object>
   {
      XGT_STD_ALLOCATOR_CONSTRUCTOR( comment_vote_object )

      public:
         template< typename Constructor, typename Allocator >
         comment_vote_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type           id;

         wallet_id_type    voter;
         comment_id_type   comment;
         uint64_t          weight = 0; ///< defines the score this vote receives. 0 if a negative vote or changed votes.
         time_point_sec    last_update; ///< The time of the last update of the vote
         int8_t            num_changes = 0;
         asset_symbol_type symbol = XGT_SYMBOL;
   };

   struct by_comment_voter_symbol;
   struct by_comment_symbol_voter;
   struct by_voter_comment_symbol;
   struct by_voter_symbol_comment;

   typedef multi_index_container<
      comment_vote_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< comment_vote_object, comment_vote_id_type, &comment_vote_object::id > >,
         ordered_unique< tag< by_comment_voter_symbol >,
            composite_key< comment_vote_object,
               member< comment_vote_object, comment_id_type, &comment_vote_object::comment >,
               member< comment_vote_object, wallet_id_type, &comment_vote_object::voter >,
               member< comment_vote_object, asset_symbol_type, &comment_vote_object::symbol >
            >
         >,
         ordered_unique< tag< by_comment_symbol_voter >,
            composite_key< comment_vote_object,
               member< comment_vote_object, comment_id_type, &comment_vote_object::comment >,
               member< comment_vote_object, asset_symbol_type, &comment_vote_object::symbol >,
               member< comment_vote_object, wallet_id_type, &comment_vote_object::voter >
            >
         >,
         ordered_unique< tag< by_voter_comment_symbol >,
            composite_key< comment_vote_object,
               member< comment_vote_object, wallet_id_type, &comment_vote_object::voter >,
               member< comment_vote_object, comment_id_type, &comment_vote_object::comment >,
               member< comment_vote_object, asset_symbol_type, &comment_vote_object::symbol >
            >
         >,
         ordered_unique< tag< by_voter_symbol_comment >,
            composite_key< comment_vote_object,
               member< comment_vote_object, wallet_id_type, &comment_vote_object::voter >,
               member< comment_vote_object, asset_symbol_type, &comment_vote_object::symbol >,
               member< comment_vote_object, comment_id_type, &comment_vote_object::comment >
            >
         >
      >,
      allocator< comment_vote_object >
   > comment_vote_index;


   struct by_permlink; /// author, perm
   struct by_root;
   struct by_parent;
   struct by_last_update; /// parent_auth, last_update
   struct by_author_last_update;

   /**
    * @ingroup object_index
    */
   typedef multi_index_container<
      comment_object,
      indexed_by<
         /// CONSENSUS INDICES - used by evaluators
         ordered_unique< tag< by_id >, member< comment_object, comment_id_type, &comment_object::id > >,
         ordered_unique< tag< by_permlink >, /// used by consensus to find posts referenced in ops
            composite_key< comment_object,
               member< comment_object, wallet_name_type, &comment_object::author >,
               member< comment_object, shared_string, &comment_object::permlink >
            >,
            composite_key_compare< std::less< wallet_name_type >, strcmp_less >
         >,
         ordered_unique< tag< by_root >,
            composite_key< comment_object,
               member< comment_object, comment_id_type, &comment_object::root_comment >,
               member< comment_object, comment_id_type, &comment_object::id >
            >
         >,
         ordered_unique< tag< by_parent >, /// used by consensus to find posts referenced in ops
            composite_key< comment_object,
               member< comment_object, wallet_name_type, &comment_object::parent_author >,
               member< comment_object, shared_string, &comment_object::parent_permlink >,
               member< comment_object, comment_id_type, &comment_object::id >
            >,
            composite_key_compare< std::less< wallet_name_type >, strcmp_less, std::less< comment_id_type > >
         >
         /// NON_CONSENSUS INDICIES - used by APIs
#ifndef IS_LOW_MEM
         ,
         ordered_unique< tag< by_last_update >,
            composite_key< comment_object,
               member< comment_object, wallet_name_type, &comment_object::parent_author >,
               member< comment_object, time_point_sec, &comment_object::last_update >,
               member< comment_object, comment_id_type, &comment_object::id >
            >,
            composite_key_compare< std::less< wallet_name_type >, std::greater< time_point_sec >, std::less< comment_id_type > >
         >,
         ordered_unique< tag< by_author_last_update >,
            composite_key< comment_object,
               member< comment_object, wallet_name_type, &comment_object::author >,
               member< comment_object, time_point_sec, &comment_object::last_update >,
               member< comment_object, comment_id_type, &comment_object::id >
            >,
            composite_key_compare< std::less< wallet_name_type >, std::greater< time_point_sec >, std::less< comment_id_type > >
         >
#endif
      >,
      allocator< comment_object >
   > comment_index;

   struct by_comment;

   typedef multi_index_container<
      comment_content_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< comment_content_object, comment_content_id_type, &comment_content_object::id > >,
         ordered_unique< tag< by_comment >, member< comment_content_object, comment_id_type, &comment_content_object::comment > >
      >,
      allocator< comment_content_object >
   > comment_content_index;

   struct by_comment_symbol;

} } // xgt::chain

namespace mira {

template<> struct is_static_length< xgt::chain::comment_vote_object > : public boost::true_type {};

} // mira

FC_REFLECT( xgt::chain::comment_object,
             (id)(author)(permlink)
             (category)(parent_author)(parent_permlink)
             (last_update)(created)(active)
             (depth)(children)
             (net_votes)(root_comment)
             (allow_replies)(allow_votes)
          )

CHAINBASE_SET_INDEX_TYPE( xgt::chain::comment_object, xgt::chain::comment_index )

FC_REFLECT( xgt::chain::comment_content_object,
            (id)(comment)(title)(body)(json_metadata) )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::comment_content_object, xgt::chain::comment_content_index )

FC_REFLECT( xgt::chain::comment_vote_object,
             (id)(voter)(comment)(weight)(last_update)(num_changes)(symbol)
          )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::comment_vote_object, xgt::chain::comment_vote_index )


namespace helpers
{
   using xgt::chain::shared_string;

   template <>
   class index_statistic_provider<xgt::chain::comment_index>
   {
   public:
      typedef xgt::chain::comment_index IndexType;
      index_statistic_info gather_statistics(const IndexType& index, bool onlyStaticInfo) const
      {
         index_statistic_info info;
         gather_index_static_data(index, &info);

         if(onlyStaticInfo == false)
         {
            for(const auto& o : index)
            {
               info._item_additional_allocation += o.category.capacity()*sizeof(shared_string::value_type);
               info._item_additional_allocation += o.parent_permlink.capacity()*sizeof(shared_string::value_type);
               info._item_additional_allocation += o.permlink.capacity()*sizeof(shared_string::value_type);
            }
         }

         return info;
      }
   };

   template <>
   class index_statistic_provider<xgt::chain::comment_content_index>
   {
   public:
      typedef xgt::chain::comment_content_index IndexType;

      index_statistic_info gather_statistics(const IndexType& index, bool onlyStaticInfo) const
      {
         index_statistic_info info;
         gather_index_static_data(index, &info);

         if(onlyStaticInfo == false)
         {
            for(const auto& o : index)
            {
               info._item_additional_allocation += o.title.capacity()*sizeof(shared_string::value_type);
               info._item_additional_allocation += o.body.capacity()*sizeof(shared_string::value_type);
               info._item_additional_allocation += o.json_metadata.capacity()*sizeof(shared_string::value_type);
            }
         }

         return info;
      }
   };

} /// namespace helpers
