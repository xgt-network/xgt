#pragma once

#include <xgt/plugins/database_api/database_api_objects.hpp>

#include <xgt/protocol/types.hpp>
#include <xgt/protocol/transaction.hpp>
#include <xgt/protocol/block_header.hpp>

#include <xgt/plugins/json_rpc/utility.hpp>

namespace xgt { namespace plugins { namespace database_api {

using protocol::wallet_name_type;
using protocol::signed_transaction;
using protocol::transaction_id_type;
using protocol::public_key_type;
using plugins::json_rpc::void_type;

enum sort_order_type
{
   by_name,
   by_account,
   by_expiration,
   by_effective_date,
   by_vote_name,
   by_schedule_time,
   by_account_witness,
   by_witness_account,
   by_from_id,
   by_ratification_deadline,
   by_destination,
   by_complete_from_id,
   by_to_complete,
   by_account_expiration,
   by_conversion_date,
   by_permlink,
   by_root,
   by_parent,
   by_last_update,
   by_author_last_update,
   by_comment_voter,
   by_voter_comment,
   by_price,
   by_symbol_contributor,
   by_symbol,
   by_control_account,
   by_symbol_time,
   by_creator,
   by_start_date,
   by_end_date,
   by_total_votes,
   by_contributor,
   by_symbol_id,
   by_comment_voter_symbol,
   by_voter_comment_symbol,
   by_comment_symbol_voter,
   by_voter_symbol_comment,
   by_account_symbol
};

enum order_direction_type
{
   ascending, ///< sort with ascending order
   descending ///< sort with descending order
};

struct list_object_args_type
{
   fc::variant       start;
   uint32_t          limit;
   sort_order_type   order;
};

struct round_type
{
   int round_num;
};

/* get_config */

typedef void_type          get_config_args;
typedef fc::variant_object get_config_return;

/* get_version */
typedef void_type          get_version_args;
struct get_version_return
{
   get_version_return() {}
   get_version_return( fc::string xgt_v, fc::string bc_v, fc::string s_v, chain_id_type c_id )
      :xgt_version(xgt_v), blockchain_version( bc_v ), xgt_revision( s_v ), chain_id( c_id ) {}

   fc::string     xgt_version;
   fc::string     blockchain_version;
   fc::string     xgt_revision;
   chain_id_type  chain_id;
};


/* Singletons */

/* get_dynamic_global_properties */

typedef void_type                            get_dynamic_global_properties_args;
typedef api_dynamic_global_property_object   get_dynamic_global_properties_return;


/* get_hardfork_properties */

typedef void_type                      get_hardfork_properties_args;
typedef api_hardfork_property_object   get_hardfork_properties_return;


/* Witnesses */

typedef list_object_args_type list_witnesses_args;

struct list_witnesses_return
{
   vector< api_witness_object > witnesses;
};


struct find_witnesses_args
{
   vector< wallet_name_type > miners;
};

struct find_witnesses_return
{
   vector< api_witness_object > miners;
};


/* Account */

typedef list_object_args_type list_wallets_args;

struct list_wallets_return
{
   vector< api_wallet_object > wallets;
};


struct find_wallets_args
{
   vector< wallet_name_type > wallets;
};

typedef list_wallets_return find_wallets_return;


/* Escrow */

typedef list_object_args_type list_escrows_args;

struct list_escrows_return
{
   vector< api_escrow_object > escrows;
};


struct find_escrows_args
{
   wallet_name_type from;
};

typedef list_escrows_return find_escrows_return;


/* Comments */

typedef list_object_args_type list_comments_args;

struct list_comments_return
{
   vector< api_comment_object > comments;
};


struct find_comments_args
{
   vector< std::pair< wallet_name_type, string > > comments;
};

typedef list_comments_return find_comments_return;


/* Votes */

typedef list_object_args_type list_votes_args;

struct list_votes_return
{
   vector< api_comment_vote_object > votes;
};


struct find_votes_args
{
   wallet_name_type author;
   string            permlink;
   asset_symbol_type symbol = XGT_SYMBOL;
};

typedef list_votes_return find_votes_return;


struct get_transaction_hex_args
{
   signed_transaction trx;
};

struct get_transaction_hex_return
{
   std::string hex;
};


struct get_required_signatures_args
{
   signed_transaction          trx;
   flat_set< public_key_type > available_keys;
};

struct get_required_signatures_return
{
   set< public_key_type > keys;
};


struct get_potential_signatures_args
{
   signed_transaction trx;
};

typedef get_required_signatures_return get_potential_signatures_return;


struct verify_authority_args
{
   signed_transaction         trx;
   fc::optional< authority >  auth;
};

struct verify_authority_return
{
   bool valid;
};

struct verify_account_authority_args
{
   wallet_name_type            account;
   flat_set< public_key_type > signers;
};

typedef verify_authority_return verify_account_authority_return;

struct verify_signatures_args
{
   fc::sha256                   hash;
   vector< signature_type >     signatures;
   vector< wallet_name_type >   required_recovery;
   vector< wallet_name_type >   required_money;
   vector< wallet_name_type >   required_social;
   vector< authority >          required_other;

   void get_required_recovery_authorities( flat_set< wallet_name_type >& a )const
   {
      a.insert( required_recovery.begin(), required_recovery.end() );
   }

   void get_required_money_authorities( flat_set< wallet_name_type >& a )const
   {
      a.insert( required_money.begin(), required_money.end() );
   }

   void get_required_social_authorities( flat_set< wallet_name_type >& a )const
   {
      a.insert( required_social.begin(), required_social.end() );
   }

   void get_required_authorities( vector< authority >& a )const
   {
      a.insert( a.end(), required_other.begin(), required_other.end() );
   }
};

struct verify_signatures_return
{
   bool valid;
};

typedef void_type get_nai_pool_args;

struct get_nai_pool_return
{
   vector< asset_symbol_type > nai_pool;
};

typedef vector< variant > get_xtt_balances_args;

struct get_xtt_balances_return
{
   vector< api_xtt_account_balance_object > balances;
};

typedef list_object_args_type list_xtt_contributions_args;

struct list_xtt_contributions_return
{
   vector< xtt_contribution_object > contributions;
};

struct find_xtt_contributions_args
{
   vector< std::pair< asset_symbol_type, wallet_name_type > > symbol_contributors;
};

typedef list_xtt_contributions_return find_xtt_contributions_return;

typedef list_object_args_type list_xtt_tokens_args;

struct list_xtt_tokens_return
{
   vector< api_xtt_token_object > tokens;
};

struct find_xtt_tokens_args
{
   vector< asset_symbol_type > symbols;
};

typedef list_xtt_tokens_return find_xtt_tokens_return;


struct find_xtt_token_balances_args
{
   vector< std::pair< wallet_name_type, asset_symbol_type > > account_symbols;
};

struct find_xtt_token_balances_return
{
   vector< api_xtt_account_balance_object > balances;
};

typedef list_object_args_type list_xtt_token_balances_args;

typedef find_xtt_token_balances_return list_xtt_token_balances_return;


} } } // xgt::database_api

FC_REFLECT( xgt::plugins::database_api::get_version_return,
            (xgt_version)(blockchain_version)(xgt_revision)(chain_id) )

FC_REFLECT_ENUM( xgt::plugins::database_api::sort_order_type,
   (by_name)
   (by_account)
   (by_expiration)
   (by_effective_date)
   (by_vote_name)
   (by_schedule_time)
   (by_account_witness)
   (by_witness_account)
   (by_from_id)
   (by_ratification_deadline)
   (by_destination)
   (by_complete_from_id)
   (by_to_complete)
   (by_account_expiration)
   (by_conversion_date)
   (by_permlink)
   (by_root)
   (by_parent)
   (by_last_update)
   (by_author_last_update)
   (by_comment_voter)
   (by_voter_comment)
   (by_price)
   (by_symbol_contributor)
   (by_symbol)
   (by_control_account)
   (by_symbol_time)
   (by_creator)
   (by_start_date)
   (by_end_date)
   (by_total_votes)
   (by_contributor)
   (by_symbol_id)
   (by_comment_voter_symbol)
   (by_voter_comment_symbol)
   (by_comment_symbol_voter)
   (by_voter_symbol_comment)
   (by_account_symbol) )

FC_REFLECT_ENUM( xgt::plugins::database_api::order_direction_type,
  (ascending)
  (descending) )

FC_REFLECT( xgt::plugins::database_api::list_object_args_type,
   (start)(limit)(order) )

FC_REFLECT( xgt::plugins::database_api::round_type,
   (round_num) )

FC_REFLECT( xgt::plugins::database_api::list_witnesses_return,
   (witnesses) )

FC_REFLECT( xgt::plugins::database_api::find_witnesses_args,
   (miners) )

FC_REFLECT( xgt::plugins::database_api::find_witnesses_return,
   (miners) )

FC_REFLECT( xgt::plugins::database_api::list_wallets_return,
   (wallets) )

FC_REFLECT( xgt::plugins::database_api::find_wallets_args,
   (wallets) )

FC_REFLECT( xgt::plugins::database_api::list_escrows_return,
   (escrows) )

FC_REFLECT( xgt::plugins::database_api::find_escrows_args,
   (from) )

FC_REFLECT( xgt::plugins::database_api::list_comments_return,
   (comments) )

FC_REFLECT( xgt::plugins::database_api::find_comments_args,
   (comments) )

FC_REFLECT( xgt::plugins::database_api::list_votes_return,
   (votes) )

FC_REFLECT( xgt::plugins::database_api::find_votes_args,
   (author)(permlink)(symbol) )

FC_REFLECT( xgt::plugins::database_api::get_transaction_hex_args,
   (trx) )

FC_REFLECT( xgt::plugins::database_api::get_transaction_hex_return,
   (hex) )

FC_REFLECT( xgt::plugins::database_api::get_required_signatures_args,
   (trx)
   (available_keys) )

FC_REFLECT( xgt::plugins::database_api::get_required_signatures_return,
   (keys) )

FC_REFLECT( xgt::plugins::database_api::get_potential_signatures_args,
   (trx) )

FC_REFLECT( xgt::plugins::database_api::verify_authority_args,
   (trx)
   (auth) )

FC_REFLECT( xgt::plugins::database_api::verify_authority_return,
   (valid) )

FC_REFLECT( xgt::plugins::database_api::verify_account_authority_args,
   (account)
   (signers) )

FC_REFLECT( xgt::plugins::database_api::verify_signatures_args,
   (hash)
   (signatures)
   (required_recovery)
   (required_money)
   (required_social)
   (required_other) )

FC_REFLECT( xgt::plugins::database_api::verify_signatures_return,
   (valid) )

FC_REFLECT( xgt::plugins::database_api::get_nai_pool_return,
   (nai_pool) )

FC_REFLECT( xgt::plugins::database_api::get_xtt_balances_return,
   (balances) )

FC_REFLECT( xgt::plugins::database_api::list_xtt_contributions_return,
   (contributions) )

FC_REFLECT( xgt::plugins::database_api::find_xtt_contributions_args,
   (symbol_contributors) )

FC_REFLECT( xgt::plugins::database_api::list_xtt_tokens_return,
   (tokens) )

FC_REFLECT( xgt::plugins::database_api::find_xtt_tokens_args,
   (symbols) )

FC_REFLECT( xgt::plugins::database_api::find_xtt_token_balances_args,
   (account_symbols) )

FC_REFLECT( xgt::plugins::database_api::find_xtt_token_balances_return,
   (balances) )

