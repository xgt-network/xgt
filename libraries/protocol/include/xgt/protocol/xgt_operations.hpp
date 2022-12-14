#pragma once
#include <sstream>
#include <fc/crypto/base58.hpp>
#include <xgt/protocol/base.hpp>
#include <xgt/protocol/block_header.hpp>
#include <xgt/protocol/asset.hpp>
#include <xgt/protocol/validation.hpp>
#include <xgt/protocol/legacy_asset.hpp>

namespace xgt { namespace protocol {

   void validate_auth_size( const authority& a );

   struct wallet_create_operation : public base_operation
   {
      asset             fee;
      wallet_name_type  creator;
      wallet_name_type  new_wallet_name;
      string            en_address;
      authority         recovery;
      authority         money;
      authority         social;
      public_key_type   memo_key;
      string            json_metadata;

      bool is_wallet_create()const { return true; }
      void validate()const;
      uint64_t energy_cost()const { return 0; }
      void get_required_money_authorities( flat_set<wallet_name_type>& a )const{ a.insert(creator); }

      static const string get_wallet_name(const vector<public_key_type> &keys) {
         std::ostringstream hasher;
         vector<public_key_type>::const_iterator it;
         for (it = keys.begin(); it != keys.end(); *it++)
         {
            string key = static_cast<string>(*it);
            hasher << key;
         }

         fc::string hashed = fc::sha256::hash(hasher.str()).str();
         const std::size_t length = 32;
         char data[32];
         fc::from_hex(hashed.c_str(), (char*) data, length);
         fc::string hashed_base58 = fc::to_base58(data, length);

         std::ostringstream os;
         os << XGT_ADDRESS_PREFIX << hashed_base58.substr(0, XGT_WALLET_NAME_LENGTH);
         return os.str();
      }

      string get_wallet_name()const
      {
         const vector<public_key_type> keys = recovery.get_keys();
         return get_wallet_name(keys);
      }
   };


   struct wallet_update_operation : public base_operation
   {
      wallet_name_type              wallet;
      optional< authority >         recovery;
      optional< authority >         money;
      optional< authority >         social;
      optional< public_key_type >   memo_key;
      string                        json_metadata;
      string                        social_json_metadata;

      extensions_type               extensions;

      void validate()const;
      uint64_t energy_cost()const { return 0; }
      bool is_wallet_update()const { return true; }
   };

   struct comment_operation : public base_operation
   {
      wallet_name_type parent_author;
      string           parent_permlink;

      wallet_name_type author;
      string           permlink;

      string           title;
      string           body;
      string           json_metadata;

      void validate()const;
      void get_required_social_authorities( flat_set<wallet_name_type>& a )const{ a.insert(author); }
   };

   /**
    *  Authors of posts may not want all of the benefits that come from creating a post. This
    *  operation allows authors to update properties associated with their post.
    *
    */
   struct comment_options_operation : public base_operation
   {
      wallet_name_type author;
      string           permlink;

      bool             allow_votes = true; /// allows a post to receive votes;

      void validate()const;
      uint64_t energy_cost()const { return 0; }
      void get_required_social_authorities( flat_set<wallet_name_type>& a )const{ a.insert(author); }
   };


   struct delete_comment_operation : public base_operation
   {
      wallet_name_type author;
      string           permlink;

      void validate()const;
      uint64_t energy_cost()const { return 0; }
      void get_required_social_authorities( flat_set<wallet_name_type>& a )const{ a.insert(author); }
   };


   struct vote_operation : public base_operation
   {
      wallet_name_type    voter;
      wallet_name_type    author;
      string              permlink;

      extensions_type     extensions;

      void validate()const;
      uint64_t energy_cost()const { return 0; }
      void get_required_social_authorities( flat_set< wallet_name_type >& a )const { a.insert( voter ); }
   };

   /**
    * @ingroup operations
    *
    * @brief Transfers XGT from one account to another.
    */
   struct transfer_operation : public base_operation
   {
      wallet_name_type from;
      /// Account to transfer asset to
      wallet_name_type to;
      /// The amount of asset to transfer from @ref from to @ref to
      asset            amount;

      /// The memo is plain-text, any encryption on the memo is up to
      /// a higher level protocol.
      string            memo;

      void              validate()const;
      uint64_t energy_cost()const { return 0; }
      void get_required_money_authorities( flat_set<wallet_name_type>& a )const{ a.insert(from); }
   };


   /**
    *  The purpose of this operation is to enable someone to send money contingently to
    *  another individual. The funds leave the *from* account and go into a temporary balance
    *  where they are held until *from* releases it to *to* or *to* refunds it to *from*.
    *
    *  In the event of a dispute the *agent* can divide the funds between the to/from account.
    *  Disputes can be raised any time before or on the dispute deadline time, after the escrow
    *  has been approved by all parties.
    *
    *  This operation only creates a proposed escrow transfer. Both the *agent* and *to* must
    *  agree to the terms of the arrangement by approving the escrow.
    *
    *  The escrow agent is paid the fee on approval of all parties. It is up to the escrow agent
    *  to determine the fee.
    *
    *  Escrow transactions are uniquely identified by 'from' and 'escrow_id', the 'escrow_id' is defined
    *  by the sender.
    */
   struct escrow_transfer_operation : public base_operation
   {
      wallet_name_type from;
      wallet_name_type to;
      wallet_name_type agent;
      uint32_t         escrow_id = 30;

      asset            xgt_amount = asset( 0, XGT_SYMBOL );
      asset            fee;

      time_point_sec   ratification_deadline;
      time_point_sec   escrow_expiration;

      string           json_meta;

      void validate()const;
      uint64_t energy_cost()const { return 0; }
      void get_required_money_authorities( flat_set<wallet_name_type>& a )const{ a.insert(from); }
   };


   /**
    *  The agent and to accounts must approve an escrow transaction for it to be valid on
    *  the blockchain. Once a part approves the escrow, the cannot revoke their approval.
    *  Subsequent escrow approve operations, regardless of the approval, will be rejected.
    */
   struct escrow_approve_operation : public base_operation
   {
      wallet_name_type from;
      wallet_name_type to;
      wallet_name_type agent;
      wallet_name_type who; // Either to or agent

      uint32_t          escrow_id = 30;
      bool              approve = true;

      void validate()const;
      uint64_t energy_cost()const { return 0; }
      void get_required_money_authorities( flat_set<wallet_name_type>& a )const{ a.insert(who); }
   };


   /**
    *  If either the sender or receiver of an escrow payment has an issue, they can
    *  raise it for dispute. Once a payment is in dispute, the agent has authority over
    *  who gets what.
    */
   struct escrow_dispute_operation : public base_operation
   {
      wallet_name_type from;
      wallet_name_type to;
      wallet_name_type agent;
      wallet_name_type who;

      uint32_t         escrow_id = 30;

      void validate()const;
      uint64_t energy_cost()const { return 0; }
      void get_required_money_authorities( flat_set<wallet_name_type>& a )const{ a.insert(who); }
   };


   /**
    *  This operation can be used by anyone associated with the escrow transfer to
    *  release funds if they have permission.
    *
    *  The permission scheme is as follows:
    *  If there is no dispute and escrow has not expired, either party can release funds to the other.
    *  If escrow expires and there is no dispute, either party can release funds to either party.
    *  If there is a dispute regardless of expiration, the agent can release funds to either party
    *     following whichever agreement was in place between the parties.
    */
   struct escrow_release_operation : public base_operation
   {
      wallet_name_type from;
      wallet_name_type to; ///< the original 'to'
      wallet_name_type agent;
      wallet_name_type who; ///< the account that is attempting to release the funds, determines valid 'receiver'
      wallet_name_type receiver; ///< the account that should receive funds (might be from, might be to)

      uint32_t         escrow_id = 30;
      asset            xgt_amount = asset( 0, XGT_SYMBOL ); ///< the amount of xgt to release

      void validate()const;
      uint64_t energy_cost()const { return 0; }
      void get_required_money_authorities( flat_set<wallet_name_type>& a )const{ a.insert(who); }
   };


   /**
    * Witnesses must vote on how to set certain chain properties to ensure a smooth
    * and well functioning network.  Any time @recovery is in the active set of witnesses these
    * properties will be used to control the blockchain configuration.
    */
   struct legacy_chain_properties
   {
      /**
       *  This fee, paid in XGT, is converted into VESTING SHARES for the new account. Accounts
       *  without vesting shares cannot earn usage rations and therefore are powerless. This minimum
       *  fee requires all accounts to have some kind of commitment to the network that includes the
       *  ability to vote and make transactions.
       */
      legacy_xgt_asset account_creation_fee = legacy_xgt_asset::from_amount( XGT_MIN_WALLET_CREATION_FEE );

      /**
       *  This witnesses vote for the maximum_block_size which is used by the network
       *  to tune rate limiting and capacity
       */
      uint32_t          maximum_block_size = XGT_MIN_BLOCK_SIZE_LIMIT * 2;

      template< bool force_canon >
      void validate()const
      {
         if( force_canon )
         {
            FC_ASSERT( account_creation_fee.symbol.is_canon() );
         }
         FC_ASSERT( account_creation_fee.amount >= XGT_MIN_WALLET_CREATION_FEE);
         FC_ASSERT( maximum_block_size >= XGT_MIN_BLOCK_SIZE_LIMIT);
      }
      uint64_t energy_cost()const { return 0; }
   };


   /**
    *  Users who wish to become a witness must pay a fee acceptable to
    *  the current witnesses to apply for the position and allow voting
    *  to begin.
    *
    *  If the recovery isn't a witness they will become a witness.  Witnesses
    *  are charged a fee equal to 1 weeks worth of witness pay which in
    *  turn is derived from the current share supply.  The fee is
    *  only applied if the recovery is not already a witness.
    *
    *  If the block_signing_key is null then the witness is removed from
    *  contention.  The network will pick the top 21 witnesses for
    *  producing blocks.
    */
   struct witness_update_operation : public base_operation
   {
      wallet_name_type owner;
      string                  url;
      public_key_type         block_signing_key;
      legacy_chain_properties props;
      asset                   fee; ///< the fee paid to register a new witness, should be 10x current block production pay

      void validate()const;
      uint64_t energy_cost()const { return 0; }
      void get_required_money_authorities( flat_set<wallet_name_type>& a )const{ a.insert(owner); }
   };


   /**
    * @brief provides a generic way to add higher level protocols on top of witness consensus
    * @ingroup operations
    *
    * There is no validation for this operation other than that required auths are valid
    */
   struct custom_operation : public base_operation
   {
      flat_set< wallet_name_type > required_auths;
      uint16_t                     id = 0;
      vector< char >               data;

      void validate()const;
      uint64_t energy_cost()const { return 0; }
      void get_required_money_authorities( flat_set<wallet_name_type>& a )const{ for( const auto& i : required_auths ) a.insert(i); }
   };


   /** serves the same purpose as custom_operation but also supports required social authorities. Unlike custom_operation,
    * this operation is designed to be human readable/developer friendly.
    **/
   struct custom_json_operation : public base_operation
   {
      flat_set< wallet_name_type > required_auths;
      flat_set< wallet_name_type > required_social_auths;
      custom_id_type               id; ///< must be less than 32 characters long
      string                       json; ///< must be proper utf8 / JSON string.

      void validate()const;
      uint64_t energy_cost()const { return 0; }
      void get_required_money_authorities( flat_set<wallet_name_type>& a )const{ for( const auto& i : required_auths ) a.insert(i); }
      void get_required_social_authorities( flat_set<wallet_name_type>& a )const{ for( const auto& i : required_social_auths ) a.insert(i); }
   };


   struct pow_input
   {
      wallet_name_type worker_account;
      block_id_type    prev_block;
      uint64_t         nonce = 0;
   };


   struct pow
   {
      pow_input        input;
      uint32_t          pow_summary = 0;

      void create( const block_id_type& prev_block, const wallet_name_type& wallet_name, uint64_t nonce );
      void validate()const;
      uint64_t energy_cost()const { return 0; }
   };

   struct sha2_pow
   {
      pow_input           input;
      fc::sha256          proof;
      block_id_type       prev_block;
      uint32_t            pow_summary = 0;

      void create( const block_id_type& recent_block, const wallet_name_type& wallet_name, uint64_t nonce );
      void init( const block_id_type& recent_block, const wallet_name_type& wallet_name );
      void update( uint64_t nonce );
      void validate() const;
      uint64_t energy_cost()const { return 0; }
      bool is_valid() const;
   };

   typedef fc::static_variant< pow, sha2_pow > pow_work;

   struct get_worker_name_visitor
   {
      typedef wallet_name_type result_type;

      template< typename WorkType >
      wallet_name_type operator()( const WorkType& work )
      {   return work.input.worker_account;    }
   };

   struct pow_operation : public base_operation
   {
      pow_work                     work;
      optional< public_key_type >  new_recovery_key;
      legacy_chain_properties      props;

      void validate()const;
      uint64_t energy_cost()const { return 0; }

      void get_required_money_authorities( flat_set<wallet_name_type>& a )const;

      void get_required_authorities( vector< authority >& a )const
      {
         if( new_recovery_key )
         {
            a.push_back( authority( 1, *new_recovery_key, 1 ) );
         }
      }

      wallet_name_type get_worker_name()const
      {
         get_worker_name_visitor vtor;
         return work.visit( vtor );
      }

      bool is_pow()const { return true; }
   };


   /**
    * This operation is used to report a miner who signs two blocks
    * at the same time. To be valid, the violation must be reported within
    * XGT_MAX_WITNESSES blocks of the head block (1 round) and the
    * producer must be in the ACTIVE witness set.
    *
    * Users not in the ACTIVE witness set should not have to worry about their
    * key getting compromised and being used to produced multiple blocks so
    * the attacker can report it and steel their vesting xgt.
    *
    * The result of the operation is to transfer the full VESTING XGT balance
    * of the block producer to the reporter.
    */
   struct report_over_production_operation : public base_operation
   {
      wallet_name_type    reporter;
      signed_block_header first_block;
      signed_block_header second_block;

      void validate()const;
      uint64_t energy_cost()const { return 0; }
   };


   /**
    * All account recovery requests come from a listed recovery account. This
    * is secure based on the assumption that only a trusted account should be
    * a recovery account. It is the responsibility of the recovery account to
    * verify the identity of the account holder of the account to recover by
    * whichever means they have agreed upon. The blockchain assumes identity
    * has been verified when this operation is broadcast.
    *
    * This operation creates an account recovery request which the account to
    * recover has 24 hours to respond to before the request expires and is
    * invalidated.
    *
    * There can only be one active recovery request per account at any one time.
    * Pushing this operation for an account to recover when it already has
    * an active request will either update the request to a new new recovery authority
    * and extend the request expiration to 24 hours from the current head block
    * time or it will delete the request. To cancel a request, simply set the
    * weight threshold of the new recovery authority to 0, making it an open authority.
    *
    * Additionally, the new recovery authority must be satisfiable. In other words,
    * the sum of the key weights must be greater than or equal to the weight
    * threshold.
    *
    * This operation only needs to be signed by the the recovery account.
    * The account to recover confirms its identity to the blockchain in
    * the recover account operation.
    */
   struct request_wallet_recovery_operation : public base_operation
   {
      wallet_name_type recovery_account;        ///< The recovery account is listed as the recovery account on the account to recover.

      wallet_name_type account_to_recover;      ///< The account to recover. This is likely due to a compromised recovery authority.

      authority         new_recovery_authority; ///< The new recovery authority the account to recover wishes to have. This is secret
                                                ///< known by the account to recover and will be confirmed in a recover_wallet_operation

      extensions_type   extensions;             ///< Extensions. Not currently used.

      void get_required_money_authorities( flat_set<wallet_name_type>& a )const{ a.insert( recovery_account ); }

      void validate() const;
      uint64_t energy_cost()const { return 0; }
   };


   /**
    * Recover an account to a new authority using a previous authority and verification
    * of the recovery account as proof of identity. This operation can only succeed
    * if there was a recovery request sent by the account's recover account.
    *
    * In order to recover the account, the account holder must provide proof
    * of past recovery and proof of identity to the recovery account. Being able
    * to satisfy an recovery authority that was used in the past 30 days is sufficient
    * to prove past recovery. The get_recovery_history function in the database API
    * returns past recovery authorities that are valid for account recovery.
    *
    * Proving identity is an off chain contract between the account holder and
    * the recovery account. The recovery request contains a new authority which
    * must be satisfied by the account holder to regain control. The actual process
    * of verifying authority may become complicated, but that is an application
    * level concern, not a blockchain concern.
    *
    * This operation requires both the past and future recovery authorities in the
    * operation because neither of them can be derived from the current chain state.
    * The operation must be signed by keys that satisfy both the new recovery authority
    * and the recent recovery authority. Failing either fails the operation entirely.
    *
    * If a recovery request was made inadvertantly, the account holder should
    * contact the recovery account to have the request deleted.
    *
    * The two setp combination of the account recovery request and recover is
    * safe because the recovery account never has access to secrets of the account
    * to recover. They simply act as an on chain endorsement of off chain identity.
    * In other systems, a fork would be required to enforce such off chain state.
    * Additionally, an account cannot be permanently recovered to the wrong account.
    * While any recovery authority from the past 30 days can be used, including a compromised
    * authority, the account can be continually recovered until the recovery account
    * is confident a combination of uncompromised authorities were used to
    * recover the account. The actual process of verifying authority may become
    * complicated, but that is an application level concern, not the blockchain's
    * concern.
    */
   struct recover_wallet_operation : public base_operation
   {
      wallet_name_type account_to_recover;        ///< The account to be recovered

      authority        new_recovery_authority;    ///< The new recovery authority as specified in the request account recovery operation.

      authority        recent_recovery_authority; ///< A previous recovery authority that the account holder will use to prove past recovery of the account to be recovered.

      extensions_type  extensions;                ///< Extensions. Not currently used.

      void get_required_authorities( vector< authority >& a )const
      {
         a.push_back( new_recovery_authority );
         a.push_back( recent_recovery_authority );
      }

      void validate() const;
      uint64_t energy_cost()const { return 0; }
   };


   /**
    * Each account lists another account as their recovery account.
    * The recovery account has the ability to create account_recovery_requests
    * for the account to recover. An account can change their recovery account
    * at any time with a 30 day delay. This delay is to prevent
    * an attacker from changing the recovery account to a malicious account
    * during an attack. These 30 days match the 30 days that an
    * recovery authority is valid for recovery purposes.
    *
    * On account creation the recovery account is set either to the creator of
    * the account (The account that pays the creation fee and is a signer on the transaction)
    * or to the empty string if the account was mined. An account with no recovery
    * has the top voted witness as a recovery account, at the time the recover
    * request is created. Note: This does mean the effective recovery account
    * of an account with no listed recovery account can change at any time as
    * witness vote weights. The top voted witness is explicitly the most trusted
    * witness according to stake.
    */
   struct change_recovery_wallet_operation : public base_operation
   {
      wallet_name_type account_to_recover;     ///< The account that would be recovered in case of compromise
      wallet_name_type new_recovery_account;   ///< The account that creates the recover request
      extensions_type  extensions;             ///< Extensions. Not currently used.

      void get_required_recovery_authorities( flat_set<wallet_name_type>& a )const{ a.insert( account_to_recover ); }
      void validate() const;
      uint64_t energy_cost()const { return 0; }
   };

   // TODO add contract_deploy_operation and contract_call_operation to replace legacy contract create and invoke
   // make sure endowments to contracts are supported
   // requires json dictionary or similar
   // requires asset indicator (nai)

   struct contract_deploy_operation : public base_operation
   {
      wallet_name_type owner;
      vector<char> code;

      void validate()const;
      uint64_t energy_cost()const { return 0; }
      bool is_contract_deploy()const { return true; }
      void get_required_recovery_authorities( flat_set<wallet_name_type>& a )const{ a.insert( owner ); }
      void get_required_money_authorities( flat_set<wallet_name_type>& a )const{ a.insert( owner ); }
   };

   struct contract_call_operation : public base_operation
   {
      wallet_name_type caller;
      contract_hash_type contract_hash;
      vector< vector<char> > args;

      void validate()const;
      uint64_t energy_cost()const { return 0; }
      bool is_contract_call()const { return true; }
   };

   struct contract_create_operation : public base_operation
   {
      wallet_name_type owner;
      vector<char> code;

      void validate()const;
      uint64_t energy_cost()const { return 0; }
      bool is_contract_create()const { return true; }
      void get_required_recovery_authorities( flat_set<wallet_name_type>& a )const{ a.insert( owner ); }
      void get_required_money_authorities( flat_set<wallet_name_type>& a )const{ a.insert( owner ); }
   };


   struct contract_invoke_operation : public base_operation
   {
      wallet_name_type caller;
      contract_hash_type contract_hash;
      vector< vector<char> > args;

      void validate()const;
      uint64_t energy_cost()const { return 0; }
      bool is_contract_invoke()const { return true; }
   };
} } // xgt::protocol


FC_REFLECT( xgt::protocol::report_over_production_operation, (reporter)(first_block)(second_block) )
FC_REFLECT( xgt::protocol::pow, (input)(pow_summary) )
FC_REFLECT( xgt::protocol::pow_input, (worker_account)(prev_block)(nonce) )
FC_REFLECT( xgt::protocol::sha2_pow, (input)(proof)(prev_block)(pow_summary) )
FC_REFLECT( xgt::protocol::legacy_chain_properties,
            (account_creation_fee)
            (maximum_block_size)
          )

FC_REFLECT_TYPENAME( xgt::protocol::pow_work )
FC_REFLECT( xgt::protocol::pow_operation, (work)(new_recovery_key)(props) )

FC_REFLECT( xgt::protocol::wallet_create_operation,
            (fee)
            (creator)
            (new_wallet_name)
            (recovery)
            (money)
            (social)
            (memo_key)
            (json_metadata) )

FC_REFLECT( xgt::protocol::wallet_update_operation,
            (wallet)
            (recovery)
            (money)
            (social)
            (memo_key)
            (json_metadata)
            (social_json_metadata)
            (extensions) )

FC_REFLECT( xgt::protocol::transfer_operation, (from)(to)(amount)(memo) )
FC_REFLECT( xgt::protocol::witness_update_operation, (owner)(url)(block_signing_key)(props)(fee) )
FC_REFLECT( xgt::protocol::comment_operation, (parent_author)(parent_permlink)(author)(permlink)(title)(body)(json_metadata) )
FC_REFLECT( xgt::protocol::vote_operation, (voter)(author)(permlink)(extensions) )
FC_REFLECT( xgt::protocol::custom_operation, (required_auths)(id)(data) )
FC_REFLECT( xgt::protocol::custom_json_operation, (required_auths)(required_social_auths)(id)(json) )
FC_REFLECT( xgt::protocol::delete_comment_operation, (author)(permlink) );
FC_REFLECT( xgt::protocol::comment_options_operation, (author)(permlink)(allow_votes) )
FC_REFLECT( xgt::protocol::escrow_transfer_operation, (from)(to)(xgt_amount)(escrow_id)(agent)(fee)(json_meta)(ratification_deadline)(escrow_expiration) );
FC_REFLECT( xgt::protocol::escrow_approve_operation, (from)(to)(agent)(who)(escrow_id)(approve) );
FC_REFLECT( xgt::protocol::escrow_dispute_operation, (from)(to)(agent)(who)(escrow_id) );
FC_REFLECT( xgt::protocol::escrow_release_operation, (from)(to)(agent)(who)(receiver)(escrow_id)(xgt_amount) );
FC_REFLECT( xgt::protocol::request_wallet_recovery_operation, (recovery_account)(account_to_recover)(new_recovery_authority)(extensions) );
FC_REFLECT( xgt::protocol::recover_wallet_operation, (account_to_recover)(new_recovery_authority)(recent_recovery_authority)(extensions) );
FC_REFLECT( xgt::protocol::change_recovery_wallet_operation, (account_to_recover)(new_recovery_account)(extensions) );

FC_REFLECT( xgt::protocol::contract_create_operation, (owner)(code) );
FC_REFLECT( xgt::protocol::contract_invoke_operation, (caller)(contract_hash)(args) );
FC_REFLECT( xgt::protocol::contract_deploy_operation, (owner)(code) );
FC_REFLECT( xgt::protocol::contract_call_operation, (caller)(contract_hash)(args) );
