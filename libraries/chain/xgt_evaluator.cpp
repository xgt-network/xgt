#include <xgt/chain/xgt_fwd.hpp>

#include <boost/multiprecision/cpp_int.hpp>
#include <xgt/chain/xgt_evaluator.hpp>
#include <xgt/chain/database.hpp>
#include <xgt/chain/custom_operation_interpreter.hpp>
#include <xgt/chain/xgt_objects.hpp>
#include <xgt/chain/xtt_objects.hpp>
#include <xgt/chain/witness_objects.hpp>
#include <xgt/chain/contract_objects.hpp>
#include <xgt/chain/block_summary_object.hpp>
#include <xgt/chain/xtt_objects/account_balance_object.hpp>

#include <keccak256.h>
#include <keccak.h>
#include <keccak.c>
#include <machine.hpp>

#include <xgt/chain/util/energybar.hpp>

#include <fc/macros.hpp>
#include <fc/crypto/rand.hpp>
#include <fc/crypto/base58.hpp>
#include <fc/crypto/ripemd160.hpp>
#include <xgt/protocol/types.hpp>
#include "rlpvalue/include/rlpvalue.h"
#include "rlpvalue/include/rlpvalue.h"
#include "rlpvalue/lib/rlpvalue.cpp"
#include "rlpvalue/lib/rlpvalue_get.cpp"
#include "rlpvalue/lib/rlpvalue_write.cpp"
#include "rlpvalue/test/utilstrencodings.cpp"

#include <functional>

#ifndef IS_LOW_MEM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic push
#if !defined( __clang__ )
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#include <diff_match_patch.h>
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#include <boost/locale/encoding_utf.hpp>

using boost::locale::conv::utf_to_utf;
using uint256_t = boost::multiprecision::uint256_t;

std::wstring utf8_to_wstring(const std::string& str)
{
    return utf_to_utf<wchar_t>(str.c_str(), str.c_str() + str.size());
}

std::string wstring_to_utf8(const std::wstring& str)
{
    return utf_to_utf<char>(str.c_str(), str.c_str() + str.size());
}

#endif

#include <fc/uint128.hpp>
#include <fc/utf8.hpp>

#include <limits>

namespace xgt { namespace chain {
   using fc::uint128_t;

inline void validate_permlink_0_1( const string& permlink )
{
   FC_ASSERT( permlink.size() > XGT_MIN_PERMLINK_LENGTH && permlink.size() < XGT_MAX_PERMLINK_LENGTH, "Permlink is not a valid size." );

   for( const auto& c : permlink )
   {
      switch( c )
      {
         case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i':
         case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
         case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': case '0':
         case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
         case '-':
            break;
         default:
            FC_ASSERT( false, "Invalid permlink character: ${s}", ("s", std::string() + c ) );
      }
   }
}

template< bool force_canon >
void copy_legacy_chain_properties( chain_properties& dest, const legacy_chain_properties& src )
{
   dest.account_creation_fee = src.account_creation_fee.to_asset< force_canon >();
   dest.maximum_block_size = src.maximum_block_size;
}

void witness_update_evaluator::do_apply( const witness_update_operation& o )
{
   _db.get_account( o.owner ); // verify owner exists

   FC_ASSERT( o.props.account_creation_fee.symbol.is_canon() );
   FC_TODO( "Move to validate() after HF20" );
   FC_ASSERT( o.props.account_creation_fee.amount <= XGT_MAX_WALLET_CREATION_FEE, "account_creation_fee greater than maximum account creation fee" );

   FC_TODO( "Check and move this to validate after HF 20" );
   if( o.props.maximum_block_size > XGT_SOFT_MAX_BLOCK_SIZE )
      wlog( "NOTIFYALERT! max block size exceeds soft limit in replay" );
   FC_ASSERT( o.props.maximum_block_size <= XGT_SOFT_MAX_BLOCK_SIZE, "Max block size cannot be more than 2MiB" );

   const auto* witness = _db.find_witness(o.owner);
   if( witness != nullptr )
   {
      _db.modify( *witness, [&]( witness_object& w ) {
         from_string( w.url, o.url );
         w.signing_key        = o.block_signing_key;
         copy_legacy_chain_properties< false >( w.props, o.props );
      });
   }
   else
   {
      _db.create< witness_object >( [&]( witness_object& w ) {
         w.owner              = o.owner;
         from_string( w.url, o.url );
         w.signing_key        = o.block_signing_key;
         w.created            = _db.head_block_time();
         copy_legacy_chain_properties< false >( w.props, o.props );
      });
   }
}

struct witness_properties_change_flags
{
   uint32_t account_creation_changed       : 1;
   uint32_t max_block_changed              : 1;
   uint32_t key_changed                    : 1;
   uint32_t url_changed                    : 1;
};

void verify_authority_accounts_exist(
   const database& db,
   const authority& auth,
   const wallet_name_type& auth_account,
   authority::classification auth_class)
{
   for( const std::pair< wallet_name_type, weight_type >& aw : auth.wallet_auths )
   {
      const wallet_object* a = db.find_account( aw.first );
      FC_ASSERT( a != nullptr, "New ${ac} authority on account ${aa} references non-existing account ${aref}",
         ("aref", aw.first)("ac", auth_class)("aa", auth_account) );
   }
}

void initialize_wallet_object( wallet_object& acc, const wallet_name_type& name, const public_key_type& key,
   const dynamic_global_property_object& props, bool mined, const wallet_name_type& recovery_account, uint32_t hardfork, uint32_t nonce, std::string en_address = "" )
{
   wlog("?????? initialize_wallet_object name ${n}", ("n", name));
   wlog("?????? initialize_wallet_object key ${n}", ("n", key));
   wlog("?????? initialize_wallet_object created ${n}", ("n", props.time));
   acc.name = name;
   acc.en_address = en_address == "" ? fc::ripemd160::hex_digest(name) : en_address;
   acc.memo_key = key;
   acc.created = props.time;
   acc.nonce = nonce;
   //acc.energybar.last_update_time = props.time.sec_since_epoch();
   //acc.mined = mined;

   //FC_TODO( "If after HF 20, there are no temp account creations, the HF check can be removed." )
   //if( recovery_account != XGT_TEMP_WALLET )
   //{
   //    acc.recovery_account = recovery_account;
   //}
   //else
   //{
   //   acc.recovery_account = "xgt";
   //}
}

void wallet_create_evaluator::do_apply( const wallet_create_operation& o )
{
   wlog("?????? wallet_create_evaluator");
   const auto& props = _db.get_dynamic_global_properties();
   const string wallet_name = o.get_wallet_name();
   wlog("?????? wallet_create_evaluator wallet_name ${w}", ("w",wallet_name));
   wlog("?????? wallet_create_evaluator creator ${w}", ("w",o.creator));
   wlog("?????? wallet_create_evaluator new_wallet_name ${w}", ("w",o.new_wallet_name));

   //const auto& creator = _db.get_account( o.creator );

   // FC_ASSERT( creator.balance >= o.fee, "Insufficient balance to create account.", ( "creator.balance", creator.balance )( "required", o.fee ) );

   // const witness_schedule_object& wso = _db.get_witness_schedule_object();

   // FC_TODO( "Move to validate() after HF20" );
   // FC_ASSERT( o.fee <= asset( XGT_MAX_WALLET_CREATION_FEE, XGT_SYMBOL ), "Account creation fee cannot be too large" );
   /*FC_ASSERT( o.fee == wso.median_props.account_creation_fee, "Must pay the exact account creation fee. paid: ${p} fee: ${f}",
               ("p", o.fee)
               ("f", wso.median_props.account_creation_fee) ); */

   //FC_TODO( "Check and move to validate post HF20" );
   //validate_auth_size( o.recovery );
   //validate_auth_size( o.money );
   //validate_auth_size( o.social );

   //verify_authority_accounts_exist( _db, o.recovery, wallet_name, authority::recovery );
   //verify_authority_accounts_exist( _db, o.money, wallet_name, authority::money );
   //verify_authority_accounts_exist( _db, o.social, wallet_name, authority::social );

   //_db.adjust_balance( creator, -o.fee );
   //_db.adjust_balance( _db.get< wallet_object, by_name >( XGT_NULL_WALLET ), o.fee );

   _db.create< wallet_object >( [&]( wallet_object& acc )
   {
      initialize_wallet_object( acc, wallet_name, o.memo_key, props, false /*mined*/, o.creator, _db.get_hardfork(), 0 );
   });

   _db.create< account_authority_object >( [&]( account_authority_object& auth )
   {
      auth.account = wallet_name;
      auth.recovery = o.recovery;
      auth.money = o.money;
      auth.social = o.social;
      auth.en_address = fc::ripemd160::hex_digest(wallet_name);
      auth.last_recovery_update = fc::time_point_sec::min();
   });
}


void wallet_update_evaluator::do_apply( const wallet_update_operation& o )
{
   wlog("!!!!!! WALLET UPDATE");
   FC_ASSERT( o.wallet != XGT_TEMP_WALLET, "Cannot update temp account." );

   if( o.social )
      o.social->validate();

   wlog("!!!!!! WALLET UPDATE 2");

   // Upsert
   // TODO: VALIDATE
   {
      const auto* wallet = _db.find_account( o.wallet );
      if (wallet == nullptr)
      {
         const auto& props = _db.get_dynamic_global_properties();
         _db.create< wallet_object >( [&]( wallet_object& acc )
         {
            initialize_wallet_object( acc, o.wallet, *o.memo_key, props, false /*mined*/, XGT_INIT_MINER_NAME, _db.get_hardfork(), 0 );
         });

         _db.create< account_authority_object >( [&]( account_authority_object& auth )
         {
            auth.account = o.wallet;
            auth.recovery = *o.recovery;
            auth.money = *o.money;
            auth.social = *o.social;
            auth.last_recovery_update = fc::time_point_sec::min();
         });
      }
   }

   {
      const auto& wallet = _db.get_account( o.wallet );
      const auto& wallet_auth = _db.get< account_authority_object, by_account >( o.wallet );

      if( o.recovery )
         validate_auth_size( *o.recovery );
      if( o.money )
         validate_auth_size( *o.money );
      if( o.social )
         validate_auth_size( *o.social );

      wlog("!!!!!! WALLET UPDATE 3");

      if( o.recovery )
      {
#ifndef IS_TEST_NET
         FC_ASSERT( _db.head_block_time() - wallet_auth.last_recovery_update > XGT_RECOVERY_UPDATE_LIMIT, "Recovery authority can only be updated once an hour." );
#endif

         verify_authority_accounts_exist( _db, *o.recovery, o.wallet, authority::recovery );

         _db.update_recovery_authority( wallet, *o.recovery );
      }
      if( o.money )
         verify_authority_accounts_exist( _db, *o.money, o.wallet, authority::money );
      if( o.social )
         verify_authority_accounts_exist( _db, *o.social, o.wallet, authority::social );

      wlog("!!!!!! WALLET UPDATE 4");

      _db.modify( wallet, [&]( wallet_object& acc )
      {
         if( o.memo_key && *o.memo_key != public_key_type() )
               acc.memo_key = *o.memo_key;

         acc.last_account_update = _db.head_block_time();
      });

      #ifndef IS_LOW_MEM
      if( o.json_metadata.size() > 0 || o.social_json_metadata.size() > 0 )
      {
         _db.modify( _db.get< account_metadata_object, by_account >( wallet.id ), [&]( account_metadata_object& meta )
         {
            if ( o.json_metadata.size() > 0 )
               from_string( meta.json_metadata, o.json_metadata );

            if ( o.social_json_metadata.size() > 0 )
               from_string( meta.social_json_metadata, o.social_json_metadata );
         });
      }
      #endif

      wlog("!!!!!! WALLET UPDATE 5");

      if( o.money || o.social )
      {
         _db.modify( wallet_auth, [&]( account_authority_object& auth)
         {
            if( o.money )  auth.money  = *o.money;
            if( o.social ) auth.social = *o.social;
         });
      }

      wlog("!!!!!! WALLET UPDATE 6");
   }
}

void delete_comment_evaluator::do_apply( const delete_comment_operation& o )
{
   const auto& comment = _db.get_comment( o.author, o.permlink );
   // TODO: XXX: Revisit this
   FC_ASSERT( comment.children == 0, "Cannot delete a comment with replies." );

   const auto& vote_idx = _db.get_index< comment_vote_index, by_comment_voter_symbol >();

   auto vote_itr = vote_idx.lower_bound( comment_id_type(comment.id) );
   while( vote_itr != vote_idx.end() && vote_itr->comment == comment.id ) {
      const auto& cur_vote = *vote_itr;
      ++vote_itr;
      _db.remove(cur_vote);
   }

   /// this loop can be skiped for validate-only nodes as it is merely gathering stats for indicies
   if( comment.parent_author != XGT_ROOT_POST_PARENT )
   {
      auto parent = &_db.get_comment( comment.parent_author, comment.parent_permlink );
      auto now = _db.head_block_time();
      while( parent )
      {
         _db.modify( *parent, [&]( comment_object& p ){
            p.children--;
            p.active = now;
         });
         parent = nullptr;
      }
   }

   _db.remove( comment );
}

void comment_options_evaluator::do_apply( const comment_options_operation& o )
{
   const auto& comment = _db.get_comment( o.author, o.permlink );

   FC_ASSERT( comment.allow_votes >= o.allow_votes, "Voting cannot be re-enabled." );

   _db.modify( comment, [&]( comment_object& c ) {
       c.allow_votes           = o.allow_votes;
   });
}

void comment_evaluator::do_apply( const comment_operation& o )
{ try {
   FC_ASSERT( o.title.size() + o.body.size() + o.json_metadata.size(), "Cannot update comment because nothing appears to be changing." );

   const auto& by_permlink_idx = _db.get_index< comment_index >().indices().get< by_permlink >();
   auto itr = by_permlink_idx.find( boost::make_tuple( o.author, o.permlink ) );

   const auto& auth = _db.get_account( o.author ); /// prove it exists

   comment_id_type id;

   const comment_object* parent = nullptr;
   if( o.parent_author != XGT_ROOT_POST_PARENT )
   {
      parent = &_db.get_comment( o.parent_author, o.parent_permlink );
      FC_ASSERT( parent->depth < XGT_MAX_COMMENT_DEPTH, "Comment is nested ${x} posts deep, maximum depth is ${y}.", ("x",parent->depth)("y",XGT_MAX_COMMENT_DEPTH) );
   }

   FC_ASSERT( fc::is_utf8( o.json_metadata ), "JSON Metadata must be UTF-8" );

   auto now = _db.head_block_time();

   if ( itr == by_permlink_idx.end() )
   {
      if( o.parent_author != XGT_ROOT_POST_PARENT )
      {
         FC_ASSERT( _db.get( parent->root_comment ).allow_replies, "The parent comment has disabled replies." );
      }

      if( o.parent_author == XGT_ROOT_POST_PARENT )
         FC_ASSERT( ( now - auth.last_root_post ) > XGT_MIN_ROOT_COMMENT_INTERVAL, "You may only post once every 5 minutes.", ("now",now)("last_root_post", auth.last_root_post) );
      else
         FC_ASSERT( (now - auth.last_post) >= XGT_MIN_REPLY_INTERVAL_HF20, "You may only comment once every 3 seconds.", ("now",now)("auth.last_post",auth.last_post) );

      uint64_t post_bandwidth = auth.post_bandwidth;

      _db.modify( auth, [&]( wallet_object& a ) {
         if( o.parent_author == XGT_ROOT_POST_PARENT )
         {
            a.last_root_post = now;
            a.post_bandwidth = uint32_t( post_bandwidth );
         }
         a.last_post = now;
         a.last_post_edit = now;
         a.post_count++;
      });

      const auto& new_comment = _db.create< comment_object >( [&]( comment_object& com )
      {
         validate_permlink_0_1( o.parent_permlink );
         validate_permlink_0_1( o.permlink );

         com.author = o.author;
         from_string( com.permlink, o.permlink );
         com.last_update = _db.head_block_time();
         com.created = com.last_update;
         com.active = com.last_update;

         if ( o.parent_author == XGT_ROOT_POST_PARENT )
         {
            com.parent_author = "";
            from_string( com.parent_permlink, o.parent_permlink );
            from_string( com.category, o.parent_permlink );
            com.root_comment = com.id;
         }
         else
         {
            com.parent_author = parent->author;
            com.parent_permlink = parent->permlink;
            com.depth = parent->depth + 1;
            com.category = parent->category;
            com.root_comment = parent->root_comment;
         }
      });

      id = new_comment.id;

   #ifndef IS_LOW_MEM
      _db.create< comment_content_object >( [&]( comment_content_object& con )
      {
         con.comment = id;

         from_string( con.title, o.title );
         if( o.body.size() < 1024*1024*128 )
         {
            from_string( con.body, o.body );
         }
         from_string( con.json_metadata, o.json_metadata );
      });
   #endif


/// this loop can be skiped for validate-only nodes as it is merely gathering stats for indicies
      auto now = _db.head_block_time();
      while( parent ) {
         _db.modify( *parent, [&]( comment_object& p ){
            p.children++;
            p.active = now;
         });
#ifndef IS_LOW_MEM
         if( parent->parent_author != XGT_ROOT_POST_PARENT )
            parent = &_db.get_comment( parent->parent_author, parent->parent_permlink );
         else
#endif
            parent = nullptr;
      }

   }
   else // start edit case
   {
      const auto& comment = *itr;

      FC_ASSERT( now - auth.last_post_edit >= XGT_MIN_COMMENT_EDIT_INTERVAL, "Can only perform one comment edit per block." );

      _db.modify( comment, [&]( comment_object& com )
      {
         com.last_update   = _db.head_block_time();
         com.active        = com.last_update;
         std::function< bool( const shared_string& a, const string& b ) > equal;
         equal = []( const shared_string& a, const string& b ) -> bool { return a.size() == b.size() && std::strcmp( a.c_str(), b.c_str() ) == 0; };

         if( !parent )
         {
            FC_ASSERT( com.parent_author == wallet_name_type(), "The parent of a comment cannot change." );
            FC_ASSERT( equal( com.parent_permlink, o.parent_permlink ), "The permlink of a comment cannot change." );
         }
         else
         {
            FC_ASSERT( com.parent_author == o.parent_author, "The parent of a comment cannot change." );
            FC_ASSERT( equal( com.parent_permlink, o.parent_permlink ), "The permlink of a comment cannot change." );
         }
      });

      _db.modify( auth, [&]( wallet_object& a )
      {
         a.last_post_edit = now;
      });
   #ifndef IS_LOW_MEM
      _db.modify( _db.get< comment_content_object, by_comment >( comment.id ), [&]( comment_content_object& con )
      {
         if( o.title.size() )         from_string( con.title, o.title );
         if( o.json_metadata.size() )
            from_string( con.json_metadata, o.json_metadata );

         if( o.body.size() ) {
            try {
            diff_match_patch<std::wstring> dmp;
            auto patch = dmp.patch_fromText( utf8_to_wstring(o.body) );
            if( patch.size() ) {
               auto result = dmp.patch_apply( patch, utf8_to_wstring( to_string( con.body ) ) );
               auto patched_body = wstring_to_utf8(result.first);
               if( !fc::is_utf8( patched_body ) ) {
                  idump(("invalid utf8")(patched_body));
                  from_string( con.body, fc::prune_invalid_utf8(patched_body) );
               } else { from_string( con.body, patched_body ); }
            }
            else { // replace
               from_string( con.body, o.body );
            }
            } catch ( ... ) {
               from_string( con.body, o.body );
            }
         }
      });
   #endif



   } // end EDIT case

} FC_CAPTURE_AND_RETHROW( (o) ) }

void escrow_transfer_evaluator::do_apply( const escrow_transfer_operation& o )
{
   try
   {
      const auto& from_account = _db.get_account(o.from);
      _db.get_account(o.to);
      _db.get_account(o.agent);

      FC_ASSERT( o.ratification_deadline > _db.head_block_time(), "The escorw ratification deadline must be after head block time." );
      FC_ASSERT( o.escrow_expiration > _db.head_block_time(), "The escrow expiration must be after head block time." );

      asset xgt_spent = o.xgt_amount;
      if( o.fee.symbol == XGT_SYMBOL )
         xgt_spent += o.fee;

      _db.adjust_balance( from_account, -xgt_spent );

      _db.create<escrow_object>([&]( escrow_object& esc )
      {
         esc.escrow_id              = o.escrow_id;
         esc.from                   = o.from;
         esc.to                     = o.to;
         esc.agent                  = o.agent;
         esc.ratification_deadline  = o.ratification_deadline;
         esc.escrow_expiration      = o.escrow_expiration;
         esc.xgt_balance            = o.xgt_amount;
         esc.pending_fee            = o.fee;
      });
   }
   FC_CAPTURE_AND_RETHROW( (o) )
}

void escrow_approve_evaluator::do_apply( const escrow_approve_operation& o )
{
   try
   {

      const auto& escrow = _db.get_escrow( o.from, o.escrow_id );

      FC_ASSERT( escrow.to == o.to, "Operation 'to' (${o}) does not match escrow 'to' (${e}).", ("o", o.to)("e", escrow.to) );
      FC_ASSERT( escrow.agent == o.agent, "Operation 'agent' (${a}) does not match escrow 'agent' (${e}).", ("o", o.agent)("e", escrow.agent) );
      FC_ASSERT( escrow.ratification_deadline >= _db.head_block_time(), "The escrow ratification deadline has passed. Escrow can no longer be ratified." );

      bool reject_escrow = !o.approve;

      if( o.who == o.to )
      {
         FC_ASSERT( !escrow.to_approved, "Account 'to' (${t}) has already approved the escrow.", ("t", o.to) );

         if( !reject_escrow )
         {
            _db.modify( escrow, [&]( escrow_object& esc )
            {
               esc.to_approved = true;
            });
         }
      }
      if( o.who == o.agent )
      {
         FC_ASSERT( !escrow.agent_approved, "Account 'agent' (${a}) has already approved the escrow.", ("a", o.agent) );

         if( !reject_escrow )
         {
            _db.modify( escrow, [&]( escrow_object& esc )
            {
               esc.agent_approved = true;
            });
         }
      }

      if( reject_escrow )
      {
         _db.adjust_balance( o.from, escrow.xgt_balance );
         _db.adjust_balance( o.from, escrow.pending_fee );

         _db.remove( escrow );
      }
      else if( escrow.to_approved && escrow.agent_approved )
      {
         _db.adjust_balance( o.agent, escrow.pending_fee );

         _db.modify( escrow, [&]( escrow_object& esc )
         {
            esc.pending_fee.amount = 0;
         });
      }
   }
   FC_CAPTURE_AND_RETHROW( (o) )
}

void escrow_dispute_evaluator::do_apply( const escrow_dispute_operation& o )
{
   try
   {
      _db.get_account( o.from ); // Verify from account exists

      const auto& e = _db.get_escrow( o.from, o.escrow_id );
      FC_ASSERT( _db.head_block_time() < e.escrow_expiration, "Disputing the escrow must happen before expiration." );
      FC_ASSERT( e.to_approved && e.agent_approved, "The escrow must be approved by all parties before a dispute can be raised." );
      FC_ASSERT( !e.disputed, "The escrow is already under dispute." );
      FC_ASSERT( e.to == o.to, "Operation 'to' (${o}) does not match escrow 'to' (${e}).", ("o", o.to)("e", e.to) );
      FC_ASSERT( e.agent == o.agent, "Operation 'agent' (${a}) does not match escrow 'agent' (${e}).", ("o", o.agent)("e", e.agent) );

      _db.modify( e, [&]( escrow_object& esc )
      {
         esc.disputed = true;
      });
   }
   FC_CAPTURE_AND_RETHROW( (o) )
}

void escrow_release_evaluator::do_apply( const escrow_release_operation& o )
{
   try
   {
      _db.get_account(o.from); // Verify from account exists

      const auto& e = _db.get_escrow( o.from, o.escrow_id );
      FC_ASSERT( e.xgt_balance >= o.xgt_amount, "Release amount exceeds escrow balance. Amount: ${a}, Balance: ${b}", ("a", o.xgt_amount)("b", e.xgt_balance) );
      FC_ASSERT( e.to == o.to, "Operation 'to' (${o}) does not match escrow 'to' (${e}).", ("o", o.to)("e", e.to) );
      FC_ASSERT( e.agent == o.agent, "Operation 'agent' (${a}) does not match escrow 'agent' (${e}).", ("o", o.agent)("e", e.agent) );
      FC_ASSERT( o.receiver == e.from || o.receiver == e.to, "Funds must be released to 'from' (${f}) or 'to' (${t})", ("f", e.from)("t", e.to) );
      FC_ASSERT( e.to_approved && e.agent_approved, "Funds cannot be released prior to escrow approval." );

      // If there is a dispute regardless of expiration, the agent can release funds to either party
      if( e.disputed )
      {
         FC_ASSERT( o.who == e.agent, "Only 'agent' (${a}) can release funds in a disputed escrow.", ("a", e.agent) );
      }
      else
      {
         FC_ASSERT( o.who == e.from || o.who == e.to, "Only 'from' (${f}) and 'to' (${t}) can release funds from a non-disputed escrow", ("f", e.from)("t", e.to) );

         if( e.escrow_expiration > _db.head_block_time() )
         {
            // If there is no dispute and escrow has not expired, either party can release funds to the other.
            if( o.who == e.from )
            {
               FC_ASSERT( o.receiver == e.to, "Only 'from' (${f}) can release funds to 'to' (${t}).", ("f", e.from)("t", e.to) );
            }
            else if( o.who == e.to )
            {
               FC_ASSERT( o.receiver == e.from, "Only 'to' (${t}) can release funds to 'from' (${t}).", ("f", e.from)("t", e.to) );
            }
         }
      }
      // If escrow expires and there is no dispute, either party can release funds to either party.

      _db.adjust_balance( o.receiver, o.xgt_amount );

      _db.modify( e, [&]( escrow_object& esc )
      {
         esc.xgt_balance -= o.xgt_amount;
      });

      if( e.xgt_balance.amount == 0 )
      {
         _db.remove( e );
      }
   }
   FC_CAPTURE_AND_RETHROW( (o) )
}

// TODO: Revisit this
fc::ecc::private_key generate_random_private_key()
{
   const size_t buflen = 128;
   char buf[buflen];
   fc::rand_bytes(buf, buflen);
   fc::sha256 hashed = fc::sha256::hash(buf, buflen);
   return fc::ecc::private_key::regenerate(hashed);
}

void transfer_evaluator::do_apply( const transfer_operation& o )
{
   const auto& wallet_idx = _db.get_index< chain::wallet_index >().indices().get< chain::by_name >();
   auto wallet_it = wallet_idx.find( o.to );
   bool has_wallet = (wallet_it != wallet_idx.end());
   if (!has_wallet)
   {
      fc::ecc::private_key recovery_privkey = generate_random_private_key();
      fc::ecc::public_key recovery_pubkey = recovery_privkey.get_public_key();
      fc::ecc::private_key money_privkey = generate_random_private_key();
      fc::ecc::public_key money_pubkey = money_privkey.get_public_key();
      fc::ecc::private_key social_privkey = generate_random_private_key();
      fc::ecc::public_key social_pubkey = social_privkey.get_public_key();
      fc::ecc::private_key memo_privkey = generate_random_private_key();
      fc::ecc::public_key memo_pubkey = memo_privkey.get_public_key();
      std::vector<public_key_type> recovery_pubkeys = {recovery_pubkey};
      const auto& props = _db.get_dynamic_global_properties();

      _db.create< wallet_object >( [&]( wallet_object& w )
      {
         initialize_wallet_object( w, o.to, memo_pubkey, props, false /*mined*/, XGT_INIT_MINER_NAME, _db.get_hardfork(), 0 );
      });

      _db.create< account_authority_object >( [&]( account_authority_object& auth )
      {
         auth.account = o.to;
         auth.recovery.weight_threshold = 1;
         auth.recovery.add_authority((public_key_type)recovery_pubkey, 1);
         auth.money.weight_threshold = 1;
         auth.money.add_authority((public_key_type)money_pubkey, 1);
         auth.social.weight_threshold = 1;
         auth.social.add_authority((public_key_type)social_pubkey, 1);
         auth.last_recovery_update = fc::time_point_sec::min();
      });
   }

   _db.adjust_balance( o.from, -o.amount );
   _db.adjust_balance( o.to, o.amount );
}


template< typename VoteType >
struct vote_context
{
   vote_context(
      const VoteType& o,
      const comment_object& c,
      const dynamic_global_property_object& d,
      wallet_id_type v_id,
      int64_t r,
      asset_symbol_type s,
      int32_t mrp,
      int32_t vprp
   ) :
      op( o ),
      comment( c ),
      dgpo( d ),
      voter_id( v_id ),
      symbol( s ),
      energy_regen_period( mrp ),
      votes_per_regen_period( vprp )
   {}

   vote_context(
      const VoteType& o,
      const comment_object& c,
      const dynamic_global_property_object& d,
      wallet_id_type v_id
   ) :
      op( o ),
      comment( c ),
      dgpo( d ),
      voter_id( v_id )
   {}

   const VoteType&                        op;
   const comment_object&                  comment;
   const dynamic_global_property_object&  dgpo;
   wallet_id_type                         voter_id;
   asset_symbol_type                      symbol;
   int32_t                                energy_regen_period = 0;
   int32_t                                votes_per_regen_period = 0;
};

template< typename ContextType, typename AccountType >
void generic_vote_evaluator(
   const ContextType& ctx,
   const AccountType& voter,
   database& _db )
{
   const auto& comment_vote_idx = _db.get_index< comment_vote_index, by_comment_voter_symbol >();

   auto now = _db.head_block_time();
   auto itr = comment_vote_idx.find( boost::make_tuple( ctx.comment.id, ctx.voter_id, ctx.symbol ) );

   if( itr == comment_vote_idx.end() )
      _db.create< comment_vote_object >( [&]( comment_vote_object& cvo )
      {
         cvo.voter = ctx.voter_id;
         cvo.comment = ctx.comment.id;
         cvo.last_update = now;
      });
   else
      _db.modify( *itr, [&]( comment_vote_object& cvo )
      {
         cvo.last_update = now;
      });

   itr = comment_vote_idx.find( boost::make_tuple( ctx.comment.id, ctx.voter_id, ctx.symbol ) );

   // Lazily delete vote
   if( itr != comment_vote_idx.end() && itr->num_changes == -1 )
   {
      FC_TODO( "This looks suspicious. We might not be deleting vote objects that we should be on nodes that are configured to clear votes" );
      FC_ASSERT( false, "Cannot vote again on a comment after payout." );

      _db.remove( *itr );
      itr = comment_vote_idx.end();
   }

   _db.modify( voter, [&]( AccountType& a )
   {
      //util::update_energybar( _db.get_dynamic_global_properties(), a, ctx.energy_regen_period, true );
      //a.voting_manabar.use_mana( consumed_rshares );
      a.last_vote_time = _db.head_block_time();
   });

   if( itr == comment_vote_idx.end() )
   {
      _db.create< comment_vote_object >( [&]( comment_vote_object& cv )
      {
         cv.voter       = ctx.voter_id;
         cv.comment     = ctx.comment.id;
         cv.last_update = _db.head_block_time();
         cv.symbol      = ctx.symbol;

         cv.weight = 0;
      });
   }
   else
   {
      _db.modify( *itr, [&]( comment_vote_object& cv )
      {
         cv.last_update = _db.head_block_time();
         cv.weight = 0;
         cv.num_changes += 1;
      });
   }
}

void vote_evaluator::do_apply( const vote_operation& o )
{
   FC_TODO( "Remove after XTT Hardfork" );

   const auto& comment = _db.get_comment( o.author, o.permlink );
   const auto& voter   = _db.get_account( o.voter );
   const auto& dgpo    = _db.get_dynamic_global_properties();

   vote_context< vote_operation > ctx( o, comment, dgpo, voter.id );

   _db.modify( comment, [&]( comment_object& c )
   {
      // TODO: XXX: Add these back in
      //c.total_vote_weight = rshare_ctx.total_vote_weight;
      //c.net_votes = rshare_ctx.net_votes;
   });
}

void custom_evaluator::do_apply( const custom_operation& o )
{
   database& d = db();
   if( d.is_producing() )
      FC_ASSERT( o.data.size() <= XGT_CUSTOM_OP_DATA_MAX_LENGTH,
         "Operation data must be less than ${bytes} bytes.", ("bytes", XGT_CUSTOM_OP_DATA_MAX_LENGTH) );

   FC_ASSERT( o.required_auths.size() <= XGT_MAX_AUTHORITY_MEMBERSHIP,
      "Authority membership exceeded. Max: ${max} Current: ${n}", ("max", XGT_MAX_AUTHORITY_MEMBERSHIP)("n", o.required_auths.size()) );
}

void custom_json_evaluator::do_apply( const custom_json_operation& o )
{
   database& d = db();

   // ilog( "custom_json_evaluator   is_producing = ${p}    operation = ${o}", ("p", d.is_producing())("o", o) );

   if( d.is_producing() )
      FC_ASSERT( o.json.length() <= XGT_CUSTOM_OP_DATA_MAX_LENGTH,
         "Operation JSON must be less than ${bytes} bytes.", ("bytes", XGT_CUSTOM_OP_DATA_MAX_LENGTH) );

   size_t num_auths = o.required_auths.size() + o.required_social_auths.size();
   FC_ASSERT( num_auths <= XGT_MAX_AUTHORITY_MEMBERSHIP,
      "Authority membership exceeded. Max: ${max} Current: ${n}", ("max", XGT_MAX_AUTHORITY_MEMBERSHIP)("n", num_auths) );

   std::shared_ptr< custom_operation_interpreter > eval = d.get_custom_json_evaluator( o.id );
   if( !eval )
   {
      // ilog( "Accepting, no evaluator registered" );
      return;
   }

   try
   {
      eval->apply( o );
   }
   catch( const fc::exception& e )
   {
      if( d.is_producing() )
      {
         // ilog( "Re-throwing exception ${e}", ("e", e) );
         throw e;
      }
      // ilog( "Suppressing exception ${e}", ("e", e) );
   }
   catch(...)
   {
      // elog( "Unexpected exception applying custom json evaluator." );
   }
}


void pow_evaluator::do_apply( const pow_operation& o )
{
   database& db = this->db();

   uint32_t head_num = db.head_block_num();
   if (head_num >= 2116800)
      return;

   const auto& dgp = db.get_dynamic_global_properties();
   uint32_t target_pow = db.get_pow_summary_target();

   // TODO: May need a check to verify it is sha2_pow before continuing
   const auto& work = o.work.get< sha2_pow >();
   fc::optional<block_id_type> previous_block_id = db.previous_block_id();
   if (previous_block_id.valid())
      FC_ASSERT( work.prev_block == *previous_block_id, "POW op not for last block" );
   auto recent_block_num = protocol::block_header::num_from_id( work.input.prev_block );
   FC_ASSERT( recent_block_num > dgp.last_irreversible_block_num,
      "POW done for block older than last irreversible block num" );
   FC_ASSERT( work.pow_summary < target_pow, "Insufficient work difficulty. Work: ${w}, Target: ${t}", ("w",work.pow_summary)("t", target_pow) );
   wallet_name_type worker_account = work.input.worker_account;

   FC_ASSERT( o.props.maximum_block_size >= XGT_MIN_BLOCK_SIZE_LIMIT * 2, "Voted maximum block size is too small." );

   db.modify( dgp, [&]( dynamic_global_property_object& p )
   {
      p.total_pow++;
      p.num_pow_witnesses++;
   });

   // TODO: Check for 0
   int halvings = (XGT_STARTING_OFFSET + dgp.head_block_number) / XGT_MINING_REWARD_HALVING_INTERVAL;
   // TODO: Assert no overflow
   long divisor = 1L << halvings;
   asset base_reward = XGT_MINING_REWARD;
   double value = base_reward.amount.value * (1.0 / static_cast<double>(divisor));
   long price = static_cast<long>(floor(value));
   asset reward = asset(price, base_reward.symbol);
   //wlog("!!!!!! Mining reward for ${w} amount ${r}", ("w",worker_account)("r",reward));

   const wallet_object* w = db.find_account( worker_account );
   if (w == nullptr)
   {
      wlog( "Wallet does not exist for worker account ${w}", ("w",worker_account) );
      if ( !o.new_recovery_key.valid() )
      {
         wlog( "Wallet new recovery key is not valid for wallet ${w}", ("w",worker_account) );
         return;
      }

      db.create< wallet_object >( [&]( wallet_object& acc )
      {
         initialize_wallet_object( acc, worker_account, *o.new_recovery_key, dgp, true /*mined*/, wallet_name_type(), _db.get_hardfork(), 0 );
         // ^ empty recovery account parameter means highest voted witness at time of recovery
      });

      db.create< account_authority_object >( [&]( account_authority_object& auth )
      {
         auth.account = worker_account;
         auth.recovery = authority( 1, *o.new_recovery_key, 1);
         auth.money = auth.recovery;
         auth.social = auth.recovery;
      });

      db.create< witness_object >( [&]( witness_object& w )
      {
          w.owner             = worker_account;
          copy_legacy_chain_properties< true >( w.props, o.props );
          w.signing_key       = *o.new_recovery_key;
          w.pow_worker        = dgp.total_pow;
      });
   }

   const witness_object* cur_witness = db.find_witness( worker_account );
   if (cur_witness == nullptr)
   {
      wlog( "Witness does not exist for worker account ${w}", ("w",worker_account) );
      return;
   }

   db.modify(*cur_witness, [&]( witness_object& w )
   {
       copy_legacy_chain_properties< true >( w.props, o.props );
       w.pow_worker        = dgp.total_pow;
   });

   db.adjust_balance(worker_account, reward);
}

// TODO: Remove this
void report_over_production_evaluator::do_apply( const report_over_production_operation& o )
{
}

fc::ripemd160 make_contract_hash(const wallet_name_type& owner, const vector<char> code)
{
  string s(code.begin(), code.end());
  string s2 = owner;
  string s3(s2 + s);
  return fc::ripemd160::hash(s3);
}

// Forward declaration
machine::chain_adapter make_chain_adapter(chain::database& _db, wallet_name_type owner, wallet_name_type caller, wallet_name_type contract_wallet, contract_hash_type contract_hash, map<fc::sha256, fc::sha256>& storage);

std::map< uint64_t, std::function<int64_t(machine::machine& m)> > energy_cost {
 // [] == scoped variables, () == params, -> optional return type, {} == function body
 {machine::stop_opcode, [](machine::machine& m){ return 0; }},
 {machine::add_opcode, [](machine::machine& m){ return 3; }},
 {machine::mul_opcode, [](machine::machine& m){ return 5; }},
 {machine::sub_opcode, [](machine::machine& m){ return 3; }},
 {machine::div_opcode, [](machine::machine& m){ return 5; }},
 {machine::sdiv_opcode, [](machine::machine& m){ return 5; }},
 {machine::mod_opcode, [](machine::machine& m){ return 5; }},
 {machine::smod_opcode, [](machine::machine& m){ return 5; }},
 {machine::addmod_opcode, [](machine::machine& m){ return 8; }},
 {machine::mulmod_opcode, [](machine::machine& m){ return 8; }},
 {machine::exp_opcode, [](machine::machine& m){ return 0; }}, // TODO -- Exp -- variable
 {machine::signextend_opcode, [](machine::machine& m){ return 5; }},
 {machine::lt_opcode, [](machine::machine& m){ return 3; }},
 {machine::gt_opcode, [](machine::machine& m){ return 3; }},
 {machine::slt_opcode, [](machine::machine& m){ return 3; }},
 {machine::sgt_opcode, [](machine::machine& m){ return 3; }},
 {machine::eq_opcode, [](machine::machine& m){ return 3; }},
 {machine::iszero_opcode, [](machine::machine& m){ return 3; }},
 {machine::and_opcode, [](machine::machine& m){ return 3; }},
 {machine::or_opcode, [](machine::machine& m){ return 3; }},
 {machine::xor_opcode, [](machine::machine& m){ return 3; }},
 {machine::not_opcode, [](machine::machine& m){ return 3; }},
 {machine::byte_opcode, [](machine::machine& m){ return 3; }},
 {machine::shl_opcode, [](machine::machine& m){ return 3; }},
 {machine::shr_opcode, [](machine::machine& m){ return 3; }},
 {machine::sar_opcode, [](machine::machine& m){ return 3; }},
 {machine::sha3_opcode, [](machine::machine& m){ return 0; }}, // TODO -- SHA3 -- variable
 {machine::address_opcode, [](machine::machine& m){ return 2; }},
 {machine::balance_opcode, [](machine::machine& m){ return 700; }},
 {machine::origin_opcode, [](machine::machine& m){ return 2; }},
 {machine::caller_opcode, [](machine::machine& m){ return 2; }},
 {machine::callvalue_opcode, [](machine::machine& m){ return 2; }},
 {machine::calldataload_opcode, [](machine::machine& m){ return 3; }},
 {machine::calldatasize_opcode, [](machine::machine& m){ return 2; }},
 {machine::calldatacopy_opcode, [](machine::machine& m){ return 0; }}, // TODO -- calldatacoy -- variab; le
 {machine::codesize_opcode, [](machine::machine& m){ return 2; }},
 {machine::codecopy_opcode, [](machine::machine& m){ return 0; }}, // TODO -- codecopy -- variab; le
 {machine::energyprice_opcode, [](machine::machine& m){ return 2; }},
 {machine::extcodesize_opcode, [](machine::machine& m){ return 700; }},
 {machine::extcodecopy_opcode, [](machine::machine& m){ return 0; }}, // TODO -- extcodecopy -- variab; le
 {machine::returndatasize_opcode, [](machine::machine& m){ return 2; }},
 {machine::returndatacopy_opcode, [](machine::machine& m){ return 0; }}, // TODO -- returndatacopy -- variab; le
 {machine::extcodehash_opcode, [](machine::machine& m){ return 700; }},
 {machine::blockhash_opcode, [](machine::machine& m){ return 20; }},
 {machine::coinbase_opcode, [](machine::machine& m){ return 2; }},
 {machine::timestamp_opcode, [](machine::machine& m){ return 2; }},
 {machine::number_opcode, [](machine::machine& m){ return 2; }},
 {machine::difficulty_opcode, [](machine::machine& m){ return 2; }},
 {machine::energylimit_opcode, [](machine::machine& m){ return 2; }},
 {machine::selfbalance_opcode, [](machine::machine& m){ return 0; }}, // TODO not yet implemented
 {machine::pop_opcode, [](machine::machine& m){ return 3; }},
 {machine::mload_opcode, [](machine::machine& m){ return 3; }},
 {machine::mstore_opcode, [](machine::machine& m){ return 3; }},
 {machine::mstore8_opcode, [](machine::machine& m){ return 3; }},
 {machine::sload_opcode, [](machine::machine& m){ return 800; }},
 {machine::sstore_opcode, [](machine::machine& m){ return 0; }}, // TODO -- sstore -- variab; le
 {machine::jump_opcode, [](machine::machine& m){ return 8; }},
 {machine::jumpi_opcode, [](machine::machine& m){ return 10; }},
 {machine::pc_opcode, [](machine::machine& m){ return 2; }},
 {machine::msize_opcode, [](machine::machine& m){ return 2; }},
 {machine::energy_opcode, [](machine::machine& m){ return 2; }},
 {machine::jumpdest_opcode, [](machine::machine& m){ return 1; }},
 {machine::push1_opcode, [](machine::machine& m){ return 3; }},
 {machine::push2_opcode, [](machine::machine& m){ return 3; }},
 {machine::push3_opcode, [](machine::machine& m){ return 3; }},
 {machine::push4_opcode, [](machine::machine& m){ return 3; }},
 {machine::push5_opcode, [](machine::machine& m){ return 3; }},
 {machine::push6_opcode, [](machine::machine& m){ return 3; }},
 {machine::push7_opcode, [](machine::machine& m){ return 3; }},
 {machine::push8_opcode, [](machine::machine& m){ return 3; }},
 {machine::push9_opcode, [](machine::machine& m){ return 3; }},
 {machine::push10_opcode, [](machine::machine& m){ return 3; }},
 {machine::push11_opcode, [](machine::machine& m){ return 3; }},
 {machine::push12_opcode, [](machine::machine& m){ return 3; }},
 {machine::push13_opcode, [](machine::machine& m){ return 3; }},
 {machine::push14_opcode, [](machine::machine& m){ return 3; }},
 {machine::push15_opcode, [](machine::machine& m){ return 3; }},
 {machine::push16_opcode, [](machine::machine& m){ return 3; }},
 {machine::push17_opcode, [](machine::machine& m){ return 3; }},
 {machine::push18_opcode, [](machine::machine& m){ return 3; }},
 {machine::push19_opcode, [](machine::machine& m){ return 3; }},
 {machine::push20_opcode, [](machine::machine& m){ return 3; }},
 {machine::push21_opcode, [](machine::machine& m){ return 3; }},
 {machine::push22_opcode, [](machine::machine& m){ return 3; }},
 {machine::push23_opcode, [](machine::machine& m){ return 3; }},
 {machine::push24_opcode, [](machine::machine& m){ return 3; }},
 {machine::push25_opcode, [](machine::machine& m){ return 3; }},
 {machine::push26_opcode, [](machine::machine& m){ return 3; }},
 {machine::push27_opcode, [](machine::machine& m){ return 3; }},
 {machine::push28_opcode, [](machine::machine& m){ return 3; }},
 {machine::push29_opcode, [](machine::machine& m){ return 3; }},
 {machine::push30_opcode, [](machine::machine& m){ return 3; }},
 {machine::push31_opcode, [](machine::machine& m){ return 3; }},
 {machine::push32_opcode, [](machine::machine& m){ return 3; }},
 {machine::dup1_opcode, [](machine::machine& m){ return 3; }},
 {machine::dup2_opcode, [](machine::machine& m){ return 3; }},
 {machine::dup3_opcode, [](machine::machine& m){ return 3; }},
 {machine::dup4_opcode, [](machine::machine& m){ return 3; }},
 {machine::dup5_opcode, [](machine::machine& m){ return 3; }},
 {machine::dup6_opcode, [](machine::machine& m){ return 3; }},
 {machine::dup7_opcode, [](machine::machine& m){ return 3; }},
 {machine::dup8_opcode, [](machine::machine& m){ return 3; }},
 {machine::dup9_opcode, [](machine::machine& m){ return 3; }},
 {machine::dup10_opcode, [](machine::machine& m){ return 3; }},
 {machine::dup11_opcode, [](machine::machine& m){ return 3; }},
 {machine::dup12_opcode, [](machine::machine& m){ return 3; }},
 {machine::dup13_opcode, [](machine::machine& m){ return 3; }},
 {machine::dup14_opcode, [](machine::machine& m){ return 3; }},
 {machine::dup15_opcode, [](machine::machine& m){ return 3; }},
 {machine::dup16_opcode, [](machine::machine& m){ return 3; }},
 {machine::swap1_opcode, [](machine::machine& m){ return 3; }},
 {machine::swap2_opcode, [](machine::machine& m){ return 3; }},
 {machine::swap3_opcode, [](machine::machine& m){ return 3; }},
 {machine::swap4_opcode, [](machine::machine& m){ return 3; }},
 {machine::swap5_opcode, [](machine::machine& m){ return 3; }},
 {machine::swap6_opcode, [](machine::machine& m){ return 3; }},
 {machine::swap7_opcode, [](machine::machine& m){ return 3; }},
 {machine::swap8_opcode, [](machine::machine& m){ return 3; }},
 {machine::swap9_opcode, [](machine::machine& m){ return 3; }},
 {machine::swap10_opcode, [](machine::machine& m){ return 3; }},
 {machine::swap11_opcode, [](machine::machine& m){ return 3; }},
 {machine::swap12_opcode, [](machine::machine& m){ return 3; }},
 {machine::swap13_opcode, [](machine::machine& m){ return 3; }},
 {machine::swap14_opcode, [](machine::machine& m){ return 3; }},
 {machine::swap15_opcode, [](machine::machine& m){ return 3; }},
 {machine::swap16_opcode, [](machine::machine& m){ return 3; }},
 {machine::log0_opcode, [](machine::machine& m){ return 0; }}, // TODO -- log0 -- variable
 {machine::log1_opcode, [](machine::machine& m){ return 0; }}, // TODO -- log1 -- variable
 {machine::log2_opcode, [](machine::machine& m){ return 0; }}, // TODO -- log2 -- variable
 {machine::log3_opcode, [](machine::machine& m){ return 0; }}, // TODO -- log3 -- variable
 {machine::log4_opcode, [](machine::machine& m){ return 0; }}, // TODO -- log4 -- variable
 {machine::create_opcode, [](machine::machine& m){ return 0; }}, // TODO -- create -- variable
 {machine::call_opcode, [](machine::machine& m){ return 0; }}, // TODO -- call -- variable
 {machine::callcode_opcode, [](machine::machine& m){ return 0; }}, // TODO -- callcode -- variable
 {machine::return_opcode, [](machine::machine& m){ return 0; }},
 {machine::delegatecall_opcode, [](machine::machine& m){ return 0; }}, // TODO -- delegatecall -- variable
 {machine::create2_opcode, [](machine::machine& m){ return 0; }}, // TODO -- create2 -- variable
 {machine::staticcall_opcode, [](machine::machine& m){ return 0; }}, // TODO -- staticcall -- variable
 {machine::revert_opcode, [](machine::machine& m){ return 0; }},
 {machine::invalid_opcode, [](machine::machine& m){ return 0; }},
 {machine::selfdestruct_opcode, [](machine::machine& m){ return 0; }} // TODO -- selfdestruct -- variable
};

// TODO: Revisit this
fc::ripemd160 generate_random_ripemd160()
{
   const size_t buflen = 128;
   char buf[buflen];
   fc::rand_bytes(buf, buflen);
   return fc::ripemd160::hash(buf, buflen);
}

uint256_t ripemd160_to_uint256_t(fc::ripemd160 h)
{
  uint256_t n(0);
  uint256_t m;
  for (int i = 0; i < 5; i++)
  {
    m = h._hash[i];
    m = m << (8 * i);
    n |= m;
  }
  return n;
}

en_address_type uint256_t_to_ripemd160(uint256_t address) {
  std::stringstream ss;
  ss << std::hex << address;
  return ss.str();
}

int char2int(char input)
{
   if(input >= '0' && input <= '9')
      return input - '0';
   if(input >= 'A' && input <= 'F')
      return input - 'A' + 10;
   if(input >= 'a' && input <= 'f')
      return input - 'a' + 10;
   throw std::invalid_argument("Invalid input string");
}

// This function assumes src to be a zero terminated sanitized string with
// an even number of [0-9a-f] characters, and target to be sufficiently large
void hex2bin(const char* src, char* target)
{
   while(*src && src[1])
   {
      *(target++) = char2int(*src)*16 + char2int(src[1]);
      src += 2;
   }
}

template <typename Hash>
inline std::string to_hex(const Hash& h)
{
    static const auto hex_chars = "0123456789abcdef";
    std::string str;
    str.reserve(sizeof(h) * 2);
    for (auto b : h.bytes)
    {
        str.push_back(hex_chars[uint8_t(b) >> 4]);
        str.push_back(hex_chars[uint8_t(b) & 0xf]);
    }
    return str;
}

void hex_to_uint8(std::string hex, uint8_t bytes[]) {
   if ( hex.size() >= 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X') )
      hex.erase(0, 2);
   if (hex.size() % 2 != 0)
      hex = '0' + hex;
   for (size_t i = 0; i < hex.size(); i += 2) {
      if (hex.substr(i, 2) == "0x")
         continue;
      uint8_t code = std::stoi(hex.substr(i, 2), 0, 16);
      bytes[i / 2] = code;
   }
   return;
}

std::string generate_create2_address(std::string sender, std::string salt, std::string init_code, std::string target_address = "TODO MIGRATE TO TESTS") {
   std::stringstream ss;

   // Recipe: keccak256( 0xff ++ address ++ salt ++ keccak256(init_code))[12:]

   std::cout << "INIT CODE CREATE2: " << init_code << std::endl;
   size_t init_code_size = init_code.size() / 2;
   uint8_t init_code_bytes[init_code_size];
   hex_to_uint8(init_code, init_code_bytes);
   auto init_code_output = ethash_keccak256( init_code_bytes, init_code_size );
   std::string init_code_hash = to_hex(init_code_output);

   std::cout << "INIT CODE HASH CREATE2: " << init_code_hash << std::endl;

   ss << "ff" << sender << salt << init_code_hash;
   std::string keccak_input_str = ss.str();

   std::cout << "KECCAK INPUT CREATE2: " << keccak_input_str << std::endl;

   size_t size = keccak_input_str.size() / 2;
   uint8_t bytes[size];
   hex_to_uint8(keccak_input_str, bytes);

   auto keccak_output = ethash_keccak256( bytes, size );
   std::string untrimmed_address = to_hex(keccak_output);
   std::string generated_create2_address = untrimmed_address.substr(24, 40);

   std::cout << "Target create2 address: " << target_address << std::endl;
   std::cout << "Actual create2 address: " << generated_create2_address << std::endl;

   return generated_create2_address;
}

// TODO sender is en_address
const std::string generate_en_address(std::string sender, uint32_t nonce, std::string salt = "", std::vector<machine::word> init_code = {}) {
   if (salt == "") {
      std::string rlp_input;
      rlp_input += sender += nonce;

      RLPValue v = RLPValue(rlp_input);

      // Generate output rlp-encoded hex
      std::string genOutput = v.write();
      std::string genHexStr = HexStr( genOutput.begin(), genOutput.end() );

      size_t size = genHexStr.size() / 2;
      uint8_t bytes[size];
      hex_to_uint8(genHexStr, bytes);

      auto keccak_output = ethash_keccak256( bytes, size );
      std::string untrimmed_address = to_hex(keccak_output);
      std::string generated_rlp_address = untrimmed_address.substr(untrimmed_address.size() - 40);

      return generated_rlp_address;
   } else {
      std::stringstream ss;

      // TODO migrate to tests
      // generate_create2_address("0000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000", "00", "4d1a2e2bb4f88f0250f26ffff098b0b30b26bf38");
      // generate_create2_address("deadbeef00000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000", "00", "b928f69bb1d91cd65274e3c79d8986362984fda3");
      // generate_create2_address("deadbeef00000000000000000000000000000000", "000000000000000000000000feed000000000000000000000000000000000000", "00", "d04116cdd17bebe565eb2422f2497e06cc1c9833");
      // generate_create2_address("0000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000", "deadbeef", "70f2b2914a2a4b783faefb75f459a580616fcb5e");
      // generate_create2_address("00000000000000000000000000000000deadbeef", "00000000000000000000000000000000000000000000000000000000cafebabe", "deadbeef", "60f3f640a8508fc6a86d45Df051962668e1e8ac7");
      // generate_create2_address("00000000000000000000000000000000deadbeef", "00000000000000000000000000000000000000000000000000000000cafebabe", "deadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeef", "1d8bfdc5d46dc4f61d6b6115972536ebe6a8854c");
      // generate_create2_address("0000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000", "", "e33c0c7f7df4809055c3eba6c09cfe4baf1bd9e0");

      for (size_t i = 0; i < init_code.size(); i++) {
         ss << std::hex << (machine::opcode)init_code[i];
      }

      std::string init_code_str = ss.str();
      std::string trimmed_wallet_name = generate_create2_address(sender, salt, init_code_str);

      return trimmed_wallet_name;
   }
};

const wallet_object& wallet_create(chain::database& _db, uint32_t nonce, std::string predetermined_en_wallet_name = "")
{
   fc::ecc::private_key recovery_privkey = generate_random_private_key();
   fc::ecc::public_key recovery_pubkey = recovery_privkey.get_public_key();
   fc::ecc::private_key money_privkey = generate_random_private_key();
   fc::ecc::public_key money_pubkey = money_privkey.get_public_key();
   fc::ecc::private_key social_privkey = generate_random_private_key();
   fc::ecc::public_key social_pubkey = social_privkey.get_public_key();
   fc::ecc::private_key memo_privkey = generate_random_private_key();
   fc::ecc::public_key memo_pubkey = memo_privkey.get_public_key();
   std::vector<public_key_type> recovery_pubkeys = {recovery_pubkey};
   const auto& props = _db.get_dynamic_global_properties();

   std::string wallet_name = wallet_create_operation::get_wallet_name(recovery_pubkeys);

   // TODO Add en name to wallet create operation
   const wallet_object& wallet = _db.create< wallet_object >( [&]( wallet_object& w )
   {
      initialize_wallet_object( w, wallet_name, memo_pubkey, props, false /*mined*/, XGT_INIT_MINER_NAME, _db.get_hardfork(), nonce, predetermined_en_wallet_name );
   });

   _db.create< account_authority_object >( [&]( account_authority_object& auth )
   {
      auth.account = wallet_name;
      auth.recovery.weight_threshold = 1;
      auth.recovery.add_authority((public_key_type)recovery_pubkey, 1);
      auth.money.weight_threshold = 1;
      auth.money.add_authority((public_key_type)money_pubkey, 1);
      auth.social.weight_threshold = 1;
      auth.social.add_authority((public_key_type)social_pubkey, 1);
      auth.last_recovery_update = fc::time_point_sec::min();
   });

   return wallet;
}

std::vector<machine::word> contract_invoke(chain::database& _db, wallet_name_type o, wallet_name_type caller, contract_hash_type contract_hash, uint64_t value, uint64_t energy, std::vector<machine::word> args, map< fc::sha256, fc::sha256 >& storage)
{
   wlog("contract_invoke another contract ${w}", ("w",contract_hash));

   std::stringstream ss;
   const auto& c = _db.get_contract(contract_hash);
   const wallet_object& destination_wallet = _db.get_account(c.wallet);

   ss << std::hex << std::string(destination_wallet.en_address);
   uint256_t destination_en_address;
   ss >> destination_en_address;
   ss.str("");

   const wallet_object& wallet = _db.get_account(caller);

   ss << std::hex << std::string(wallet.en_address);
   uint256_t caller_en_address;
   ss >> caller_en_address;
   ss.str("");

   machine::message msg = {
      0,
      0,
      int64_t(energy),
      caller_en_address,
      destination_en_address,
      value,
      // TODO args size is inaccurate
      args.size() * 2,
      args,
      0
   };

   const bool is_debug = true;
   const uint64_t block_timestamp = static_cast<uint64_t>( _db.head_block_time().sec_since_epoch() );
   const uint64_t block_number = _db.head_block_num();
   const uint64_t block_difficulty = static_cast<uint64_t>( _db.get_pow_summary_target() );
   const uint64_t block_energylimit = 0;
   const uint64_t tx_energyprice = 0;
   std::string tx_origin = caller; // TODO update this
   std::string block_coinbase = caller; // TODO update this to this block's miner

   machine::context ctx = {
      is_debug,
      block_timestamp,
      block_number,
      block_difficulty,
      block_energylimit,
      tx_energyprice,
      tx_origin,
      block_coinbase
   };

   int64_t energy_cost_incurred = 0;

   std::vector<machine::word> code(c.code.begin(), c.code.end());
   machine::chain_adapter adapter = make_chain_adapter(_db, o, caller, c.wallet, contract_hash, storage);
   machine::machine m(ctx, code, msg, adapter);

   std::string line;
   while (m.is_running())
   {
      std::cerr << "step\n" << std::endl;
      std::cerr << m.to_json() << std::endl;
      m.step();
      auto energy_callback = energy_cost[m.get_current_opcode()];
      // Calculate energy cost and add to total
      energy_cost_incurred += energy_callback(m);
      // Print out any logging that was generated
      while ( std::getline(m.get_logger(), line) )
         std::cerr << "\e[36m" << "LOG: " << line << "\e[0m" << std::endl;
   }
   while ( std::getline(m.get_logger(), line) )
      std::cerr << "\e[36m" << "LOG: " << line << "\e[0m" << std::endl;
   std::cout << m.to_json() << std::endl;

   uint32_t energy_regen_period = XGT_ENERGY_REGENERATION_SECONDS; // TODO: Hardcode this for now (should be a constant I think)
   _db.modify(wallet, [&](wallet_object& w)
   {
      util::update_energybar( _db.get_dynamic_global_properties(), w, energy_regen_period, true );
      w.energybar.use_energy( energy_cost_incurred );
   });

   // Transfer value (C_xfer) if callvalue != 0
   // TODO XXX Value is dramatically adjusted for test purposes...
   // Ethereum wei values are significantly smaller proportionally than XGT g values
   // 1 Ether = 1,000,000,000,000,000,000 wei
   // 1 XGT   =               100,000,000 g
   if (value != 0) {
      auto callvalue = asset( value / 1000000000, XGT_SYMBOL );

      const auto& contract_wallet = _db.get_account(c.wallet);

      _db.adjust_balance( contract_wallet, callvalue );

      const auto& caller_wallet = _db.get_account(caller);

      _db.adjust_balance( caller_wallet, -callvalue );
   }

   wlog("contract_invoke return value ${w}", ("w",m.get_return_value()));
   return m.get_return_value();
}

void contract_create_evaluator::do_apply( const contract_create_operation& op )
{
   wlog("!!!!!! contract_create owner ${w} code size ${y}", ("w",op.owner)("y",op.code.size()));

   const auto& contract_wallet = wallet_create(_db, 1);

   auto base_contract = _db.create< contract_object >( [&](contract_object& c)
   {
      c.contract_hash = make_contract_hash(op.owner, op.code);
      wlog("!!!!!! contract_create contract_hash ${c}", ("c",c.contract_hash));
      c.wallet = contract_wallet.name;
      c.owner = op.owner;
      c.code = op.code;
   });

   auto storage = map< fc::sha256, fc::sha256 >();
   _db.create< contract_storage_object >([&](contract_storage_object& cs) {
      cs.contract = base_contract.contract_hash;
      cs.caller = op.owner;
      cs.data = storage;
   });

   auto machine_return = contract_invoke(_db, op.owner, op.owner, base_contract.contract_hash, 0, 0, {}, storage);
   std::vector<char> result(machine_return.begin(), machine_return.end());

   const contract_storage_object* cs = _db.find_contract_storage(base_contract.contract_hash, op.owner);
   _db.modify< contract_storage_object >(*cs, [&](contract_storage_object& cs) {
      cs.data = storage;
   });

   const auto& contract = _db.get_contract(base_contract.contract_hash);
   _db.modify< contract_object >(contract, [&](contract_object& c) {
     c.code = result;
   });
}

std::string inspect(std::vector<machine::word> words)
{
   std::stringstream ss;
   ss << "[";
   for (size_t i = 0; i < words.size(); i++)
   {
      ss << std::to_string(words.at(i));
      if (i != words.size() -1)
         ss << ", ";
   }
   ss << "]";
   return ss.str();
}

machine::chain_adapter make_chain_adapter(chain::database& _db, wallet_name_type owner, wallet_name_type caller, wallet_name_type contract_wallet, contract_hash_type contract_hash, map<fc::sha256, fc::sha256>& storage)
{

  std::function< machine::big_word(std::vector<machine::word>) > sha3 = [&](std::vector<machine::word> memory) -> machine::big_word
  {
    std::stringstream ss;
    if (memory.size() > 32) {
       for (size_t i = 0; i < memory.size(); i++) {
          ss << "0000000000000000000000000000000";
          ss << std::hex << (machine::opcode)memory[i];
       }
    } else {
       for (size_t i = 0; i < memory.size(); i++) {
          if ( (machine::opcode)memory[i] < 0x10 )
             ss << "0";
          ss << std::hex << (machine::opcode)memory[i];
       }
    }

    std::string memory_str = ss.str();
    std::cout << "KECCAK INPUT SHA3: " << memory_str << std::endl;

    size_t memory_size = memory_str.size() / 2;
    uint8_t memory_bytes[memory_size];
    hex_to_uint8(memory_str, memory_bytes);
    auto memory_output = ethash_keccak256( memory_bytes, memory_size );
    std::string memory_hash = to_hex(memory_output);

    ss.str("");
    ss << std::hex << memory_hash;
    uint256_t sha3_return;
    ss >> sha3_return;
    std::cout << "INIT CODE HASH SHA3: " << memory_hash << std::endl;

    return sha3_return;
  };

  std::function< machine::big_word(machine::big_word) > get_balance = [&](machine::big_word en_address) -> machine::big_word
  {
    std::stringstream ss;
    ss << std::hex << en_address;
    const en_address_type r160 = ss.str();
    auto& wallet = _db.get_account_by_en_address(r160);
    return static_cast<machine::big_word>(wallet.balance.amount.value);
  };

  std::function< std::string(std::string) > get_code_hash = [&](std::string address) -> std::string
  {
    fc::ripemd160 en_address(address);
    const auto& contract = _db.get_contract(en_address);
    std::string message(contract.code.begin(), contract.code.end());
    unsigned char output[32];
    SHA3_CTX ctx;
    keccak_init(&ctx);
    keccak_update(&ctx, (unsigned char*)message.c_str(), message.size());
    keccak_final(&ctx, output);
    std::string fin((char*)output, 32);
    return fin;
  };

  // TODO: Revisit this, we may want to use the original sha256 hash
  std::function< machine::big_word(uint64_t) > get_block_hash = [&](uint64_t block_num) -> machine::big_word
  {
    if ( block_num > _db.head_block_num() ) {
      return 0;
    }
    fc::optional<fc::sha256> block_hash = _db.get_block_hash_from_block_num(block_num);
    auto ripemd160_hash = fc::ripemd160::hash( block_hash->data() );
    machine::big_word item = ripemd160_to_uint256_t(ripemd160_hash);
    return item;
  };

  std::function< std::vector<machine::word>(machine::big_word) > get_code_at_addr = [&](machine::big_word address) -> std::vector<machine::word>
  {
    std::stringstream ss;
    ss << std::hex << address;
    const en_address_type r160 = ss.str();
    const auto& contract_wallet_account = _db.get_account_by_en_address(r160);
    const contract_object& contract = _db.get_contract_by_wallet(contract_wallet_account.name);

    return std::vector<unsigned char>(contract.code.begin(), contract.code.end());
  };

  std::function< machine::big_word(std::vector<machine::word>, machine::big_word) > contract_create = [&, owner, caller, contract_hash](std::vector<machine::word> memory, machine::big_word value) -> machine::big_word
  {
    std::stringstream ss;

    const wallet_name_type sender_address = _db.get_contract(contract_hash).wallet;
    const auto& caller_contract_wallet = _db.get_account(sender_address);

    if (caller_contract_wallet.balance.amount.value >= value) {
      fc::ripemd160 new_contract_hash = generate_random_ripemd160();

      std::string new_wallet_name = generate_en_address(std::string(sender_address), caller_contract_wallet.nonce);

      wallet_create(_db, 1, new_wallet_name);
      chain::contract_object base_contract = _db.create< contract_object >( [&](contract_object& c) {
          c.contract_hash = new_contract_hash;
          c.owner = sender_address;
          c.code = reinterpret_cast< std::vector<char>& >(memory);
          c.wallet = new_wallet_name;
      });

      if (value != 0) {
        // TODO XXX value is set to be compatible with eth value, needs to be adjusted
        auto callvalue = asset( value, XGT_SYMBOL );
        _db.adjust_balance( caller_contract_wallet, -callvalue );
        _db.adjust_balance( base_contract.wallet, callvalue );
      }

      const contract_storage_object* cs = _db.find_contract_storage(new_contract_hash, sender_address);

      map< fc::sha256, fc::sha256 > storage;
      if (cs != nullptr) {
         storage = cs->data;
      }
      else {
         storage = map< fc::sha256, fc::sha256 >();
      }

      _db.create< contract_storage_object >([&](contract_storage_object& cs) {
         cs.contract = new_contract_hash;
         cs.caller = sender_address;
         cs.data = storage;
      });

      // TODO Exceptions should occupy return_value in the case of contract invoke failure (result == 0)
      std::vector<machine::word> base_contract_return = contract_invoke(_db, owner, sender_address, new_contract_hash, 0, 0, {}, storage);
      std::vector<char> result(base_contract_return.begin(), base_contract_return.end());

      cs = _db.find_contract_storage(new_contract_hash, sender_address);
      _db.modify< contract_storage_object >(*cs, [&](contract_storage_object& cs) {
         cs.data = storage;
      });

      const auto& deployed_contract = _db.get_contract(new_contract_hash);
      _db.modify< contract_object >(deployed_contract, [&](contract_object& co) {
         co.code = result;
      });

      const wallet_object& deployed_contract_wallet = _db.get_account(deployed_contract.wallet);
      ss << std::hex << std::string(deployed_contract_wallet.en_address);
      uint256_t new_contract_wallet_address;
      ss >> new_contract_wallet_address;

      _db.modify( caller_contract_wallet, [&]( wallet_object& acc )
      {
         if (acc.nonce)
            acc.nonce += 1;
         else
            acc.nonce = 1;
      });

      return new_contract_wallet_address;
    }
    else {
      return 0;
    }
  };

  std::function< std::pair< machine::word, std::vector<machine::word> >(machine::big_word, uint64_t, machine::big_word, std::vector<machine::word>) > contract_call = [&, caller, contract_hash](machine::big_word address, uint64_t energy, machine::big_word value, std::vector<machine::word> args) -> std::pair< machine::word, std::vector<machine::word> >
  {
     // TODO Implement call depth limit
     std::stringstream ss;

     const en_address_type r160 = uint256_t_to_ripemd160(address);
     const wallet_object* ext_wallet = _db.find_account_by_en_address(r160);
     wallet_name_type ext_wallet_name;

     if (ext_wallet != nullptr) {
        ext_wallet_name = ext_wallet->name;
     } else {
        // TODO Check energy before creating contract
        ss << std::hex << contract_create(args, value);
        const en_address_type new_r160 = ss.str();
        const wallet_object* new_wallet = _db.find_account_by_en_address(new_r160);
        ext_wallet_name = new_wallet->name;
     }

     const wallet_name_type sender_address = _db.get_contract(contract_hash).wallet;
     const auto& caller_contract_wallet = _db.get_account(sender_address);

     if (value != 0) {
        if (caller_contract_wallet.balance.amount.value >= value) {
           // TODO XXX value is set to be compatible with eth value, needs to be adjusted
           auto callvalue = asset( value, XGT_SYMBOL );
           _db.adjust_balance( caller_contract_wallet, -callvalue );
           _db.adjust_balance( ext_wallet_name, callvalue );
        } else {
           return std::make_pair(0, std::vector<machine::word>());
        }
     }
     const contract_object* ext_contract = _db.find_contract_by_wallet(ext_wallet_name);

     std::vector<machine::word> contract_return = {};

     if (args.size() > 0) {
        if (!ext_contract) {
           const en_address_type new_contract_r160 = uint256_t_to_ripemd160( contract_create(args, 0) );
           const wallet_object* new_contract_wallet = _db.find_account_by_en_address(new_contract_r160);
           ext_wallet_name = new_contract_wallet->name;
           ext_contract = _db.find_contract_by_wallet(ext_wallet_name);
        }
        const contract_storage_object* cs = _db.find_contract_storage(ext_contract->contract_hash, owner);
        map< fc::sha256, fc::sha256 > storage;
        if (cs)
           storage = cs->data;
        else
           storage = map< fc::sha256, fc::sha256 >();

        contract_return = contract_invoke(_db, ext_contract->owner, sender_address, ext_contract->contract_hash, (uint64_t)value, energy, args, storage);

        if (cs != nullptr) {
           _db.modify< contract_storage_object >(*cs, [&](contract_storage_object& cs) {
              cs.contract = ext_contract->contract_hash;
              cs.caller = caller;
              cs.data = storage;
           });
        } else {
           _db.create< contract_storage_object >([&](contract_storage_object& cs) {
              cs.contract = ext_contract->contract_hash;
              cs.caller = caller;
              cs.data = storage;
           });
        }
     }
     return std::make_pair(machine::word(1), contract_return);
  };

  std::function< std::pair< machine::word, std::vector<machine::word> >(machine::big_word, uint64_t, machine::big_word, std::vector<machine::word>) > contract_callcode = [&, caller, contract_hash](machine::big_word address, uint64_t energy, machine::big_word value, std::vector<machine::word> args) -> std::pair< machine::word, std::vector<machine::word> >
  {
     // TODO Implement call depth limit
     std::stringstream ss;

     const en_address_type r160 = uint256_t_to_ripemd160(address);
     const wallet_object* ext_wallet = _db.find_account_by_en_address(r160);
     wallet_name_type ext_wallet_name;

     if (ext_wallet != nullptr) {
        ext_wallet_name = ext_wallet->name;
     } else {
        // TODO Check energy before creating contract
        ss << std::hex << contract_create(args, value);
        const en_address_type new_r160 = ss.str();
        const wallet_object* new_wallet = _db.find_account_by_en_address(new_r160);
        ext_wallet_name = new_wallet->name;
     }

     const auto& caller_wallet = _db.get_account(caller);

     if (value != 0) {
        if (caller_wallet.balance.amount.value >= value) {
           // TODO XXX value is set to be compatible with eth value, needs to be adjusted
           auto callvalue = asset( value, XGT_SYMBOL );
           _db.adjust_balance( caller, -callvalue );
           _db.adjust_balance( caller, callvalue );
        } else {
           return std::make_pair(0, std::vector<machine::word>());
        }
     }
     const contract_object* ext_contract = _db.find_contract_by_wallet(ext_wallet_name);

     std::vector<machine::word> contract_return = {};

     if (args.size() > 0) {
        if (!ext_contract) {
           const en_address_type new_contract_r160 = uint256_t_to_ripemd160( contract_create(args, 0) );
           const wallet_object* new_contract_wallet = _db.find_account_by_en_address(new_contract_r160);
           ext_wallet_name = new_contract_wallet->name;
           ext_contract = _db.find_contract_by_wallet(ext_wallet_name);
        }
        const contract_storage_object* cs = _db.find_contract_storage(contract_hash, caller);
        map< fc::sha256, fc::sha256 > storage;
        if (cs)
           storage = cs->data;
        else
           storage = map< fc::sha256, fc::sha256 >();

        contract_return = contract_invoke(_db, ext_contract->owner, caller, ext_contract->contract_hash, (uint64_t)value, energy, args, storage);

        if (cs != nullptr) {
           _db.modify< contract_storage_object >(*cs, [&](contract_storage_object& cs) {
              cs.contract = contract_hash;
              cs.caller = caller;
              cs.data = storage;
           });
        } else {
           _db.create< contract_storage_object >([&](contract_storage_object& cs) {
              cs.contract = contract_hash;
              cs.caller = caller;
              cs.data = storage;
           });
        }
     }
     return std::make_pair(machine::word(1), contract_return);
  };

  std::function< std::pair< machine::word, std::vector<machine::word> >(machine::big_word, uint64_t, std::vector<machine::word>) > contract_delegatecall = [&, caller, owner, contract_hash](machine::big_word address, uint64_t energy, std::vector<machine::word> args) -> std::pair< machine::word, std::vector<machine::word> >
  {
     // TODO Implement call depth limit
     std::stringstream ss;

     const en_address_type r160 = uint256_t_to_ripemd160(address);
     const wallet_object* ext_wallet = _db.find_account_by_en_address(r160);
     wallet_name_type ext_wallet_name;

     if (ext_wallet != nullptr) {
        ext_wallet_name = ext_wallet->name;
     } else {
        // TODO Check energy before creating contract
        ss << std::hex << contract_create(args, 0);
        const en_address_type new_r160 = ss.str();
        const wallet_object* new_wallet = _db.find_account_by_en_address(new_r160);
        ext_wallet_name = new_wallet->name;
     }

     const contract_object* ext_contract = _db.find_contract_by_wallet(ext_wallet_name);

     std::vector<machine::word> contract_return = {};

     if (args.size() > 0) {
        if (!ext_contract) {
           const en_address_type new_contract_r160 = uint256_t_to_ripemd160( contract_create(args, 0) );
           const wallet_object* new_contract_wallet = _db.find_account_by_en_address(new_contract_r160);
           ext_wallet_name = new_contract_wallet->name;
           ext_contract = _db.find_contract_by_wallet(ext_wallet_name);
        }
        const contract_storage_object* cs = _db.find_contract_storage(contract_hash, caller);
        map< fc::sha256, fc::sha256 > storage;
        if (cs)
           storage = cs->data;
        else
           storage = map< fc::sha256, fc::sha256 >();

        contract_return = contract_invoke(_db, ext_contract->owner, caller, ext_contract->contract_hash, 0, energy, args, storage);

        if (cs != nullptr) {
           _db.modify< contract_storage_object >(*cs, [&](contract_storage_object& cs) {
              cs.contract = contract_hash;
              cs.caller = caller;
              cs.data = storage;
           });
        } else {
           _db.create< contract_storage_object >([&](contract_storage_object& cs) {
              cs.contract = contract_hash;
              cs.caller = caller;
              cs.data = storage;
           });
        }
     }
     return std::make_pair(machine::word(1), contract_return);
  };

  std::function< std::pair< machine::word, std::vector<machine::word> >(machine::big_word, uint64_t, std::vector<machine::word>) > contract_staticcall = [&, owner, caller, contract_hash](machine::big_word address, uint64_t energy, std::vector<machine::word> args) -> std::pair< machine::word, std::vector<machine::word> >
  {
     std::stringstream ss;

     const en_address_type r160 = uint256_t_to_ripemd160(address);
     const wallet_object* ext_wallet = _db.find_account_by_en_address(r160);
     wallet_name_type ext_wallet_name;

     if (ext_wallet != nullptr) {
        ext_wallet_name = ext_wallet->name;
     } else {
        ss << std::hex << contract_create(args, 0);
        const en_address_type new_r160 = ss.str();
        const wallet_object* new_wallet = _db.find_account_by_en_address(new_r160);
        ext_wallet_name = new_wallet->name;
     }

     const wallet_name_type sender_address = _db.get_contract(contract_hash).wallet;
     const contract_object* ext_contract = _db.find_contract_by_wallet(ext_wallet_name);

     std::vector<machine::word> contract_return = {};

     if (args.size() > 0) {
        if (!ext_contract) {
           const en_address_type new_contract_r160 = uint256_t_to_ripemd160( contract_create(args, 0) );
           const wallet_object* new_contract_wallet = _db.find_account_by_en_address(new_contract_r160);
           ext_wallet_name = new_contract_wallet->name;
           ext_contract = _db.find_contract_by_wallet(ext_wallet_name);
        }
        const contract_storage_object* cs = _db.find_contract_storage(ext_contract->contract_hash, owner);
        map< fc::sha256, fc::sha256 > storage;
        if (cs)
           storage = cs->data;
        else
           storage = map< fc::sha256, fc::sha256 >();

        contract_return = contract_invoke(_db, ext_contract->owner, sender_address, ext_contract->contract_hash, 0, energy, args, storage);

        if (cs != nullptr) {
           _db.modify< contract_storage_object >(*cs, [&](contract_storage_object& cs) {
              cs.contract = ext_contract->contract_hash;
              cs.caller = caller;
              cs.data = storage;
           });
        } else {
           _db.create< contract_storage_object >([&](contract_storage_object& cs) {
              cs.contract = ext_contract->contract_hash;
              cs.caller = caller;
              cs.data = storage;
           });
        }
     }
     return std::make_pair(machine::word(1), contract_return);
  };

  std::function< machine::big_word(std::vector<machine::word>, machine::big_word, machine::big_word) > contract_create2 = [&_db, owner, caller, contract_hash](std::vector<machine::word> memory, machine::big_word value, machine::big_word salt) -> machine::big_word
  {
    // TODO: https://github.com/ethereum/EIPs/blob/master/EIPS/eip-1014.md
    const wallet_name_type sender_address = _db.get_contract(contract_hash).wallet;
    const auto& caller_contract_wallet = _db.get_account(sender_address);

    // TODO XXX Create wallet for contract

    if (caller_contract_wallet.balance.amount.value >= value) {
      fc::ripemd160 new_contract_hash = generate_random_ripemd160();

      std::stringstream ss;
      ss << salt;
      std::string salt_str = ss.str();
      ss.str("");

      std::string caller_en_address = caller_contract_wallet.en_address;
      std::cout << "CALLER EN ADDRESS: " << caller_en_address << std::endl;
      std::string new_wallet_en_address = generate_en_address(caller_en_address, caller_contract_wallet.nonce, salt_str, memory);
      std::cout << "NEW EN WALLET NAME: " << new_wallet_en_address << std::endl;

      wallet_create(_db, caller_contract_wallet.nonce, new_wallet_en_address);

      const auto& new_wallet_name = _db.get_account_by_en_address(new_wallet_en_address).name;

      chain::contract_object base_contract = _db.create< contract_object >( [&](contract_object& c) {
          c.contract_hash = new_contract_hash;
          c.owner = sender_address;
          c.code = reinterpret_cast< std::vector<char>& >(memory);
          c.wallet = new_wallet_name;
      });

      if (value != 0) {
        // TODO XXX value is set to be compatible with eth value, needs to be adjusted
        auto callvalue = asset( value, XGT_SYMBOL );
        // TODO this should be the calling contract's wallet
        _db.adjust_balance( caller_contract_wallet, -callvalue );
        _db.adjust_balance( base_contract.wallet, callvalue );
      }

      const contract_storage_object* cs = _db.find_contract_storage(new_contract_hash, sender_address);

      map< fc::sha256, fc::sha256 > storage;
      if (cs != nullptr) {
         storage = cs->data;
      }
      else {
         storage = map< fc::sha256, fc::sha256 >();
      }

      _db.create< contract_storage_object >([&](contract_storage_object& cs) {
         cs.contract = new_contract_hash;
         cs.caller = sender_address;
         cs.data = storage;
      });

      // TODO Exceptions should occupy return_value in the case of contract invoke failure (result == 0)
      std::vector<machine::word> base_contract_return = contract_invoke(_db, owner, sender_address, new_contract_hash, 0, 0, {}, storage);
      std::vector<char> result(base_contract_return.begin(), base_contract_return.end());

      cs = _db.find_contract_storage(new_contract_hash, sender_address);
      _db.modify< contract_storage_object >(*cs, [&](contract_storage_object& cs) {
         cs.data = storage;
      });

      const auto& deployed_contract = _db.get_contract(new_contract_hash);
      _db.modify< contract_object >(deployed_contract, [&](contract_object& co) {
         co.code = result;
      });

      const std::string& address_ref = std::string(new_wallet_en_address);

      ss.str("");
      ss << std::hex << address_ref;
      uint256_t new_contract_wallet_address;
      ss >> new_contract_wallet_address;

      _db.modify( caller_contract_wallet, [&]( wallet_object& acc )
      {
         if (acc.nonce)
            acc.nonce = acc.nonce + 1;
         else
            acc.nonce = 1;
      });

      std::cout << "NEW CONTRACT WALLET ADDRESS: " << new_contract_wallet_address << std::endl;

      return new_contract_wallet_address;
    }
    else {
      return 0;
    }
  };

  std::function< bool(std::vector<machine::word>) > revert = [&](std::vector<machine::word> memory) -> bool
  {
    // TODO from eth yellowpaper: Halt execution reverting state changes but returning data and remaining gas.
    return true;
  };

  std::function< machine::big_word(machine::big_word) > get_storage = [&](uint256_t key) -> machine::big_word
  {
    return _db.hash_to_bigint( storage[ _db.bigint_to_hash(key) ] );
  };

  std::function< bool(machine::big_word, machine::big_word) > set_storage = [&](machine::big_word key, machine::big_word value) -> bool
  {
    storage[ _db.bigint_to_hash(key) ] = _db.bigint_to_hash(value);
    return true;
  };

  std::function< std::vector<machine::word>(std::vector<machine::word>) > contract_return = [&](std::vector<machine::word> memory) -> std::vector<machine::word>
  {
    return memory;
  };

  std::function< bool(std::string) > self_destruct = [&](std::string address) -> bool
  {
    // TODO send all funds from contract to address
    std::stringstream ss;
    ss << std::hex << address;
    const en_address_type r160 = ss.str();
    const auto& destination_wallet = _db.get_account_by_en_address(r160);

    const auto& caller_contract_wallet = _db.get_account(owner);

    const auto value = caller_contract_wallet.balance.amount.value;

    if (value != 0) {
       // TODO XXX value is set to be compatible with eth value, needs to be adjusted
       auto callvalue = asset( value, XGT_SYMBOL );
       // TODO this should be the calling contract's wallet
       _db.adjust_balance( caller_contract_wallet, -callvalue );
       _db.adjust_balance( destination_wallet, callvalue );
    }

    contract_object contract = _db.get_contract_by_wallet(owner);
    _db.remove(contract);

    return true;
  };

  std::function< std::vector<machine::word>(std::string) > get_input_data = [&](std::string input) -> std::vector<machine::word>
  {
    // TODO
    return std::vector<machine::word>();
  };

  std::function< void(const machine::log_object&) > emit_log = [&](const machine::log_object& log)
  {
    _db.create< contract_log_object >( [&](contract_log_object& cl)
    {
      cl.contract_hash = contract_hash;
      cl.owner = owner;
      cl.topics.reserve(log.topics.size());
      for (auto topic : log.topics)
         cl.topics.push_back(_db.bigint_to_hash(topic));
      cl.data = log.data;
    });

    return contract_hash.str();
  };

  machine::chain_adapter adapter = {
    sha3,
    get_balance,
    get_code_hash,
    get_block_hash,
    get_code_at_addr,
    contract_create,
    contract_call,
    contract_callcode,
    contract_delegatecall,
    contract_staticcall,
    contract_create2,
    revert,
    get_storage,
    set_storage,
    contract_return,
    self_destruct,
    get_input_data,
    emit_log
  };

  return adapter;
}

void contract_invoke_evaluator::do_apply( const contract_invoke_operation& op )
{
   const contract_hash_type contract_hash = op.contract_hash;
   const auto& c = _db.get_contract(contract_hash);
   uint64_t energy = 0; // TODO

   const contract_storage_object* cs = _db.find_contract_storage(contract_hash, c.owner);

   map< fc::sha256, fc::sha256 > storage;
   if (cs != nullptr) {
      storage = cs->data;
   }
   else {
      storage = map< fc::sha256, fc::sha256 >();
   }

   std::vector<unsigned char> unsigned_args;
   std::copy(op.args.begin(), op.args.end(), std::back_inserter(unsigned_args));
   auto result = contract_invoke(_db, c.owner, op.caller, contract_hash, op.value, energy, unsigned_args, storage);

   if (cs != nullptr) {
      _db.modify< contract_storage_object >(*cs, [&](contract_storage_object& cs) {
         cs.contract = contract_hash;
         cs.caller = c.owner;
         cs.data = storage;
      });
   }
   else {
      _db.create< contract_storage_object >([&](contract_storage_object& cs) {
         cs.contract = contract_hash;
         cs.caller = c.owner;
         cs.data = storage;
      });
   }
}

} } // xgt::chain
