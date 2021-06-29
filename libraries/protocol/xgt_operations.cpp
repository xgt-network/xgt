#include <xgt/protocol/xgt_operations.hpp>

#include <fc/macros.hpp>
#include <fc/io/json.hpp>
#include <fc/macros.hpp>

#include <locale>

namespace xgt { namespace protocol {

   void validate_auth_size( const authority& a )
   {
      size_t size = a.wallet_auths.size() + a.key_auths.size();
      FC_ASSERT( size <= XGT_MAX_AUTHORITY_MEMBERSHIP,
         "Authority membership exceeded. Max: ${max} Current: ${n}", ("max", XGT_MAX_AUTHORITY_MEMBERSHIP)("n", size) );
   }

   void wallet_create_operation::validate() const
   {
      wlog("!!!!!! wallet_create_operation");
      // TODO
      // validate_wallet_name( new_wallet_name );
      FC_ASSERT( is_asset_type( fee, XGT_SYMBOL ), "Account creation fee must be XGT" );
      recovery.validate();
      money.validate();

      if ( json_metadata.size() > 0 )
      {
         FC_ASSERT( fc::is_utf8(json_metadata), "JSON Metadata not formatted in UTF8" );
         FC_ASSERT( fc::json::is_valid(json_metadata), "JSON Metadata not valid JSON" );
      }
      FC_ASSERT( fee >= asset( 0, XGT_SYMBOL ), "Account creation fee cannot be negative" );
   }

   void wallet_update_operation::validate() const
   {
      validate_wallet_name( wallet );

      if ( json_metadata.size() > 0 )
      {
         FC_ASSERT( fc::is_utf8(json_metadata), "JSON Metadata not formatted in UTF8" );
         FC_ASSERT( fc::json::is_valid(json_metadata), "JSON Metadata not valid JSON" );
      }

      if ( social_json_metadata.size() > 0 )
      {
         FC_ASSERT( fc::is_utf8(social_json_metadata), "JSON Metadata not formatted in UTF8" );
         FC_ASSERT( fc::json::is_valid(social_json_metadata), "JSON Metadata not valid JSON" );
      }
   }

   void comment_operation::validate() const
   {
      FC_ASSERT( title.size() < XGT_COMMENT_TITLE_LIMIT,
         "Title size limit exceeded. Max: ${max} Current: ${n}", ("max", XGT_COMMENT_TITLE_LIMIT - 1)("n", title.size()) );
      FC_ASSERT( fc::is_utf8( title ), "Title not formatted in UTF8" );
      FC_ASSERT( body.size() > 0, "Body is empty" );
      FC_ASSERT( fc::is_utf8( body ), "Body not formatted in UTF8" );


      if( parent_author.size() )
         validate_wallet_name( parent_author );
      validate_wallet_name( author );
      validate_permlink( parent_permlink );
      validate_permlink( permlink );

      if( json_metadata.size() > 0 )
      {
         FC_ASSERT( fc::json::is_valid(json_metadata), "JSON Metadata not valid JSON" );
      }
   }

   void comment_options_operation::validate()const
   {
      validate_wallet_name( author );
      validate_permlink( permlink );
   }

   void delete_comment_operation::validate()const
   {
      validate_permlink( permlink );
      validate_wallet_name( author );
   }

   void vote_operation::validate() const
   {
      validate_wallet_name( voter );
      validate_wallet_name( author );
      validate_permlink( permlink );
   }

   void transfer_operation::validate() const
   { try {
      validate_wallet_name( from );
      // TODO: Add this back in
      //validate_wallet_name( to );
      FC_ASSERT( amount.amount > 0, "Cannot transfer a negative amount (aka: stealing)" );
      FC_ASSERT( memo.size() < XGT_MAX_MEMO_SIZE, "Memo is too large" );
      FC_ASSERT( fc::is_utf8( memo ), "Memo is not UTF8" );
   } FC_CAPTURE_AND_RETHROW( (*this) ) }

   void witness_update_operation::validate() const
   {
      validate_wallet_name( owner );

      FC_ASSERT( url.size() <= XGT_MAX_WITNESS_URL_LENGTH, "URL is too long" );

      FC_ASSERT( url.size() > 0, "URL size must be greater than 0" );
      FC_ASSERT( fc::is_utf8( url ), "URL is not valid UTF8" );
      FC_ASSERT( fee >= asset( 0, XGT_SYMBOL ), "Fee cannot be negative" );
      props.validate< false >();
   }

   void custom_operation::validate() const {
      /// required auth accounts are the ones whose bandwidth is consumed
      FC_ASSERT( required_auths.size() > 0, "at least one account must be specified" );
   }
   void custom_json_operation::validate() const {
      /// required auth accounts are the ones whose bandwidth is consumed
      FC_ASSERT( (required_auths.size() + required_social_auths.size()) > 0, "at least one account must be specified" );
      FC_ASSERT( id.size() <= XGT_CUSTOM_OP_ID_MAX_LENGTH,
         "Operation ID length exceeded. Max: ${max} Current: ${n}", ("max", XGT_CUSTOM_OP_ID_MAX_LENGTH)("n", id.size()) );
      FC_ASSERT( fc::is_utf8(json), "JSON Metadata not formatted in UTF8" );
      FC_ASSERT( fc::json::is_valid(json), "JSON Metadata not valid JSON" );
   }


   struct pow_operation_validate_visitor
   {
      typedef void result_type;

      template< typename PowType >
      void operator()( const PowType& pow )const
      {
         pow.validate();
      }
   };

   void pow_operation::validate()const
   {
      props.validate< true >();
      work.visit( pow_operation_validate_visitor() );
   }

   struct pow_operation_get_required_money_visitor
   {
      typedef void result_type;

      pow_operation_get_required_money_visitor( flat_set< wallet_name_type >& required_money )
         : _required_money( required_money ) {}

      template< typename PowType >
      void operator()( const PowType& work )const
      {
         _required_money.insert( work.input.worker_account );
      }

      flat_set<wallet_name_type>& _required_money;
   };

   void pow_operation::get_required_money_authorities( flat_set<wallet_name_type>& a )const
   {
      if( !new_recovery_key )
      {
         pow_operation_get_required_money_visitor vtor( a );
         work.visit( vtor );
      }
   }

   void pow::create( const block_id_type& prev, const wallet_name_type& wallet_name, uint64_t n )
   {
      input.worker_account = wallet_name;
      input.prev_block     = prev;
      input.nonce          = n;

      auto prv_key = fc::sha256::hash( input );
      auto input = fc::sha256::hash( prv_key );
      auto signature = fc::ecc::private_key::regenerate( prv_key ).sign_compact( input, fc::ecc::fc_canonical );

      auto sig_hash            = fc::sha256::hash( signature );
      public_key_type recover  = fc::ecc::public_key( signature, sig_hash );

      fc::sha256 work = fc::sha256::hash(std::make_pair(input,recover));
      pow_summary = work.approx_log_32();
   }

   void pow::validate()const
   {
      validate_wallet_name( input.worker_account );
      pow tmp; tmp.create( input.prev_block, input.worker_account, input.nonce );
      FC_ASSERT( pow_summary == tmp.pow_summary, "reported work does not match calculated work" );
   }

   void sha2_pow::create( const block_id_type& recent_block, const wallet_name_type& wallet_name, uint64_t nonce )
   {
      input.worker_account = wallet_name;
      input.prev_block = recent_block;
      input.nonce = nonce;

      proof = fc::sha256::hash( fc::sha256::hash( input ) );
      pow_summary = proof.approx_log_32();
   }

   void sha2_pow::init( const block_id_type& recent_block, const wallet_name_type& wallet_name )
   {
      input.worker_account = wallet_name;
      input.prev_block = recent_block;
   }

   void sha2_pow::update(uint64_t nonce) {
      input.nonce = nonce;

      proof = fc::sha256::hash( fc::sha256::hash( input ) );
      pow_summary = proof.approx_log_32();
   }

   void sha2_pow::validate() const
   {
      validate_wallet_name( input.worker_account );
      FC_ASSERT(is_valid(), "proof of work seed does not match expected seed");
   }

   bool sha2_pow::is_valid() const
   {
      // TODO: Double-hash
      auto p = fc::sha256::hash( fc::sha256::hash( input ) );
      if (proof != p)
         return false;
      if ( pow_summary != proof.approx_log_32() )
         return false;
      return true;
   }

   void report_over_production_operation::validate()const
   {
      validate_wallet_name( reporter );
      validate_wallet_name( first_block.witness );
      FC_ASSERT( first_block.witness   == second_block.witness );
      FC_ASSERT( first_block.timestamp == second_block.timestamp );
      FC_ASSERT( first_block.signee()  == second_block.signee() );
      FC_ASSERT( first_block.id() != second_block.id() );
   }

   void escrow_transfer_operation::validate()const
   {
      validate_wallet_name( from );
      validate_wallet_name( to );
      validate_wallet_name( agent );
      FC_ASSERT( fee.amount >= 0, "fee cannot be negative" );
      FC_ASSERT( xgt_amount.amount >= 0, "xgt amount cannot be negative" );
      FC_ASSERT( xgt_amount.amount > 0, "escrow must transfer a non-zero amount" );
      FC_ASSERT( from != agent && to != agent, "agent must be a third party" );
      FC_ASSERT( (fee.symbol == XGT_SYMBOL), "fee must be XGT" );
      FC_ASSERT( xgt_amount.symbol == XGT_SYMBOL, "xgt amount must contain XGT" );
      FC_ASSERT( ratification_deadline < escrow_expiration, "ratification deadline must be before escrow expiration" );
      if ( json_meta.size() > 0 )
      {
         FC_ASSERT( fc::is_utf8(json_meta), "JSON Metadata not formatted in UTF8" );
         FC_ASSERT( fc::json::is_valid(json_meta), "JSON Metadata not valid JSON" );
      }
   }

   void escrow_approve_operation::validate()const
   {
      validate_wallet_name( from );
      validate_wallet_name( to );
      validate_wallet_name( agent );
      validate_wallet_name( who );
      FC_ASSERT( who == to || who == agent, "to or agent must approve escrow" );
   }

   void escrow_dispute_operation::validate()const
   {
      validate_wallet_name( from );
      validate_wallet_name( to );
      validate_wallet_name( agent );
      validate_wallet_name( who );
      FC_ASSERT( who == from || who == to, "who must be from or to" );
   }

   void escrow_release_operation::validate()const
   {
      validate_wallet_name( from );
      validate_wallet_name( to );
      validate_wallet_name( agent );
      validate_wallet_name( who );
      validate_wallet_name( receiver );
      FC_ASSERT( who == from || who == to || who == agent, "who must be from or to or agent" );
      FC_ASSERT( receiver == from || receiver == to, "receiver must be from or to" );
      FC_ASSERT( xgt_amount.amount >= 0, "xgt amount cannot be negative" );
      FC_ASSERT( xgt_amount.amount > 0, "escrow must release a non-zero amount" );
      FC_ASSERT( xgt_amount.symbol == XGT_SYMBOL, "xgt amount must contain XGT" );
   }

   void request_wallet_recovery_operation::validate()const
   {
      validate_wallet_name( recovery_account );
      validate_wallet_name( account_to_recover );
      new_recovery_authority.validate();
   }

   void recover_wallet_operation::validate()const
   {
      validate_wallet_name( account_to_recover );
      FC_ASSERT( !( new_recovery_authority == recent_recovery_authority ), "Cannot set new recovery authority to the recent recovery authority" );
      FC_ASSERT( !new_recovery_authority.is_impossible(), "new recovery authority cannot be impossible" );
      FC_ASSERT( !recent_recovery_authority.is_impossible(), "recent recovery authority cannot be impossible" );
      FC_ASSERT( new_recovery_authority.weight_threshold, "new recovery authority cannot be trivial" );
      new_recovery_authority.validate();
      recent_recovery_authority.validate();
   }

   void change_recovery_wallet_operation::validate()const
   {
      validate_wallet_name( account_to_recover );
      validate_wallet_name( new_recovery_account );
   }

   void contract_create_operation::validate()const
   { try {
      validate_wallet_name( owner );
   } FC_CAPTURE_AND_RETHROW( (*this) ) }

   void contract_invoke_operation::validate()const
   {
   }

} } // xgt::protocol
