#pragma once

#include <xgt/chain/wallet_object.hpp>
#include <xgt/chain/block_summary_object.hpp>
#include <xgt/chain/comment_object.hpp>
#include <xgt/chain/global_property_object.hpp>
#include <xgt/chain/history_object.hpp>
#include <xgt/chain/xgt_objects.hpp>
#include <xgt/chain/xtt_objects.hpp>
#include <xgt/chain/transaction_object.hpp>
#include <xgt/chain/witness_objects.hpp>
#include <xgt/chain/database.hpp>
#include <xgt/chain/xtt_objects/account_balance_object.hpp>

namespace xgt { namespace plugins { namespace database_api {

using namespace xgt::chain;

typedef change_recovery_account_request_object api_change_recovery_account_request_object;
typedef block_summary_object                   api_block_summary_object;
typedef dynamic_global_property_object         api_dynamic_global_property_object;
typedef escrow_object                          api_escrow_object;
typedef witness_vote_object                    api_witness_vote_object;

struct api_comment_object
{
   api_comment_object( const comment_object& o, const database& db ):
      id( o.id ),
      category( to_string( o.category ) ),
      parent_author( o.parent_author ),
      parent_permlink( to_string( o.parent_permlink ) ),
      author( o.author ),
      permlink( to_string( o.permlink ) ),
      last_update( o.last_update ),
      created( o.created ),
      active( o.active ),
      depth( o.depth ),
      children( o.children ),
      net_votes( o.net_votes ),
      allow_replies( o.allow_replies ),
      allow_votes( o.allow_votes )
   {
      const auto root = db.find( o.root_comment );

      if( root != nullptr )
      {
         root_author = root->author;
         root_permlink = to_string( root->permlink );
      }
   }

   api_comment_object(){}

   comment_id_type  id;
   string           category;
   string           parent_author;
   string           parent_permlink;
   string           author;
   string           permlink;

   string           title;
   string           body;
   string           json_metadata;
   time_point_sec   last_update;
   time_point_sec   created;
   time_point_sec   active;

   uint8_t          depth = 0;
   uint32_t         children = 0;

   int32_t          net_votes = 0;

   wallet_name_type root_author;
   string           root_permlink;

   bool             allow_replies = false;
   bool             allow_votes = false;
};

struct api_comment_vote_object
{
   api_comment_vote_object( const comment_vote_object& cv, const database& db ) :
      id( cv.id ),
      weight( cv.weight ),
      last_update( cv.last_update ),
      num_changes( cv.num_changes )
   {
      voter = db.get( cv.voter ).name;
      auto comment = db.get( cv.comment );
      author = comment.author;
      permlink = to_string( comment.permlink );
   }

   comment_vote_id_type id;

   wallet_name_type    voter;
   wallet_name_type    author;
   string              permlink;
   uint64_t            weight = 0;
   time_point_sec      last_update;
   int8_t              num_changes = 0;
};

struct api_wallet_object
{
   api_wallet_object( const wallet_object& a, const database& db ) :
      id( a.id ),
      name( a.name ),
      en_address( a.en_address ),
      memo_key( a.memo_key ),
      last_account_update( a.last_account_update ),
      created( a.created ),
      mined( a.mined ),
      recovery_account( a.recovery_account ),
      reset_account( a.reset_account ),
      last_account_recovery( a.last_account_recovery ),
      comment_count( a.comment_count ),
      lifetime_vote_count( a.lifetime_vote_count ),
      post_count( a.post_count ),
      can_vote( a.can_vote ),
      energybar( a.energybar ),
      balance( a.balance ),
      witnesses_voted_for( a.witnesses_voted_for ),
      last_post( a.last_post ),
      last_root_post( a.last_root_post ),
      last_post_edit( a.last_post_edit ),
      last_vote_time( a.last_vote_time ),
      post_bandwidth( a.post_bandwidth )
   {
      const auto& auth = db.get< account_authority_object, by_account >( name );
      recovery = authority( auth.recovery );
      money = authority( auth.money );
      social = authority( auth.social );
      last_recovery_update = auth.last_recovery_update;

      const auto* maybe_meta = db.find< account_metadata_object, by_account >( id );
      if( maybe_meta )
      {
         json_metadata = to_string( maybe_meta->json_metadata );
         social_json_metadata = to_string( maybe_meta->social_json_metadata );
      }

      const auto& by_control_account_index = db.get_index<xtt_token_index>().indices().get<by_control_account>();
      auto xtt_obj_itr = by_control_account_index.find( name );
      is_xtt = xtt_obj_itr != by_control_account_index.end();
   }


   api_wallet_object(){}

   wallet_id_type    id;

   wallet_name_type  name;
   en_address_type   en_address;
   authority         recovery;
   authority         money;
   authority         social;
   public_key_type   memo_key;
   string            json_metadata;
   string            social_json_metadata;

   time_point_sec    last_recovery_update;
   time_point_sec    last_account_update;

   time_point_sec    created;
   bool              mined = false;
   wallet_name_type  recovery_account;
   wallet_name_type  reset_account;
   time_point_sec    last_account_recovery;
   uint32_t          comment_count = 0;
   uint32_t          lifetime_vote_count = 0;
   uint32_t          post_count = 0;

   bool              can_vote = false;
   util::energybar   energybar;

   asset             balance;

   uint16_t          witnesses_voted_for = 0;

   time_point_sec    last_post;
   time_point_sec    last_root_post;
   time_point_sec    last_post_edit;
   time_point_sec    last_vote_time;
   uint32_t          post_bandwidth = 0;

   bool              is_xtt = false;
};

struct api_recovery_authority_history_object
{
   api_recovery_authority_history_object( const recovery_authority_history_object& o ) :
      id( o.id ),
      account( o.account ),
      previous_recovery_authority( authority( o.previous_recovery_authority ) ),
      last_valid_time( o.last_valid_time )
   {}

   api_recovery_authority_history_object() {}

   recovery_authority_history_id_type  id;

   wallet_name_type                account;
   authority                       previous_recovery_authority;
   time_point_sec                  last_valid_time;
};

struct api_account_recovery_request_object
{
   api_account_recovery_request_object( const account_recovery_request_object& o ) :
      id( o.id ),
      account_to_recover( o.account_to_recover ),
      new_recovery_authority( authority( o.new_recovery_authority ) ),
      expires( o.expires )
   {}

   api_account_recovery_request_object() {}

   account_recovery_request_id_type id;
   wallet_name_type                 account_to_recover;
   authority                        new_recovery_authority;
   time_point_sec                   expires;
};

struct api_account_history_object
{

};

struct api_witness_object
{
   api_witness_object( const witness_object& w ) :
      id( w.id ),
      owner( w.owner ),
      created( w.created ),
      url( to_string( w.url ) ),
      total_missed( w.total_missed ),
      last_aslot( w.last_aslot ),
      last_confirmed_block_num( w.last_confirmed_block_num ),
      pow_worker( w.pow_worker ),
      signing_key( w.signing_key ),
      props( w.props ),
      votes( w.votes ),
      virtual_last_update( w.virtual_last_update ),
      virtual_position( w.virtual_position ),
      virtual_scheduled_time( w.virtual_scheduled_time ),
      last_work( w.last_work ),
      running_version( w.running_version ),
      hardfork_version_vote( w.hardfork_version_vote ),
      hardfork_time_vote( w.hardfork_time_vote )
   {}

   api_witness_object() {}

   witness_id_type   id;
   wallet_name_type  owner;
   time_point_sec    created;
   string            url;
   uint32_t          total_missed = 0;
   uint64_t          last_aslot = 0;
   uint64_t          last_confirmed_block_num = 0;
   uint64_t          pow_worker = 0;
   public_key_type   signing_key;
   chain_properties  props;
   share_type        votes;
   fc::uint128       virtual_last_update;
   fc::uint128       virtual_position;
   fc::uint128       virtual_scheduled_time;
   digest_type       last_work;
   version           running_version;
   hardfork_version  hardfork_version_vote;
   time_point_sec    hardfork_time_vote;
};

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

struct api_hardfork_property_object
{
   api_hardfork_property_object( const hardfork_property_object& h ) :
      id( h.id ),
      last_hardfork( h.last_hardfork ),
      current_hardfork_version( h.current_hardfork_version ),
      next_hardfork( h.next_hardfork ),
      next_hardfork_time( h.next_hardfork_time )
   {
      size_t n = h.processed_hardforks.size();
      processed_hardforks.reserve( n );

      for( size_t i = 0; i < n; i++ )
         processed_hardforks.push_back( h.processed_hardforks[i] );
   }

   api_hardfork_property_object() {}

   hardfork_property_id_type     id;
   vector< fc::time_point_sec >  processed_hardforks;
   uint32_t                      last_hardfork;
   protocol::hardfork_version    current_hardfork_version;
   protocol::hardfork_version    next_hardfork;
   fc::time_point_sec            next_hardfork_time;
};

struct api_xtt_token_object
{
   api_xtt_token_object( const xtt_token_object& token, const database& db ) : token( token )
   {
      const xtt_ico_object* ico = db.find< chain::xtt_ico_object, chain::by_symbol >( token.liquid_symbol );
      if ( ico != nullptr )
         this->ico = *ico;
   }

   xtt_token_object                token;
   fc::optional< xtt_ico_object >  ico;
};

typedef uint64_t api_id_type;

struct api_xtt_account_balance_object
{
   api_xtt_account_balance_object() = default;

   api_xtt_account_balance_object( const account_regular_balance_object& b, const database& db ) :
      id( b.id ),
      name( b.name ),
      liquid( b.liquid ),
      energybar( b.energybar ),
      last_vote_time( b.last_vote_time )
   {
      pending_liquid = asset( 0, liquid.symbol );
   }

   api_id_type         id;
   wallet_name_type    name;
   asset               liquid;

   util::energybar     energybar;

   fc::time_point_sec  last_vote_time;

   asset               pending_liquid;
};

} } } // xgt::plugins::database_api

FC_REFLECT( xgt::plugins::database_api::api_comment_object,
             (id)(author)(permlink)
             (category)(parent_author)(parent_permlink)
             (title)(body)(json_metadata)(last_update)(created)(active)
             (depth)(children)
             (net_votes)
             (root_author)(root_permlink)
             (allow_replies)(allow_votes)
          )

FC_REFLECT( xgt::plugins::database_api::api_comment_vote_object,
             (id)(voter)(author)(permlink)(weight)(last_update)(num_changes)
          )

FC_REFLECT( xgt::plugins::database_api::api_wallet_object,
             (id)(name)(en_address)(recovery)(money)(social)(memo_key)(json_metadata)(social_json_metadata)(last_recovery_update)(last_account_update)
             (created)(mined)
             (recovery_account)(last_account_recovery)(reset_account)
             (comment_count)(lifetime_vote_count)(post_count)(can_vote)(energybar)
             (balance)
             (witnesses_voted_for)
             (last_post)(last_root_post)(last_post_edit)(last_vote_time)
             (post_bandwidth)
             (is_xtt)
          )

FC_REFLECT( xgt::plugins::database_api::api_recovery_authority_history_object,
             (id)
             (account)
             (previous_recovery_authority)
             (last_valid_time)
          )

FC_REFLECT( xgt::plugins::database_api::api_account_recovery_request_object,
             (id)
             (account_to_recover)
             (new_recovery_authority)
             (expires)
          )

FC_REFLECT( xgt::plugins::database_api::api_witness_object,
             (id)
             (owner)
             (created)
             (url)(votes)(virtual_last_update)(virtual_position)(virtual_scheduled_time)(total_missed)
             (last_aslot)(last_confirmed_block_num)(pow_worker)(signing_key)
             (props)
             (last_work)
             (running_version)
             (hardfork_version_vote)(hardfork_time_vote)
          )

FC_REFLECT_DERIVED( xgt::plugins::database_api::api_signed_block_object, (xgt::protocol::signed_block),
                     (block_id)
                     (signing_key)
                     (transaction_ids)
                  )

FC_REFLECT( xgt::plugins::database_api::api_hardfork_property_object,
            (id)
            (processed_hardforks)
            (last_hardfork)
            (current_hardfork_version)
            (next_hardfork)
            (next_hardfork_time)
          )

FC_REFLECT( xgt::plugins::database_api::api_xtt_token_object,
   (token)
   (ico)
)

FC_REFLECT( xgt::plugins::database_api::api_xtt_account_balance_object,
            (id)
            (name)
            (liquid)
            (energybar)
            (last_vote_time)
            (pending_liquid)
          )
