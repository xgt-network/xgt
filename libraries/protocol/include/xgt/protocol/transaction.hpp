#pragma once
#include <xgt/protocol/operations.hpp>
#include <xgt/protocol/sign_state.hpp>
#include <xgt/protocol/types.hpp>
#include <xgt/protocol/transaction.hpp>

#include <numeric>

namespace xgt { namespace protocol {

using fc::ecc::canonical_signature_type;

   struct transaction
   {
      uint16_t           ref_block_num    = 0;
      uint32_t           ref_block_prefix = 0;

      fc::time_point_sec expiration;

      vector<operation>  operations;
      extensions_type    extensions;

      digest_type         digest()const;
      transaction_id_type id()const;
      void                validate() const;
      digest_type         sig_digest( const chain_id_type& chain_id )const;

      void set_expiration( fc::time_point_sec expiration_time );
      void set_reference_block( const block_id_type& reference_block );

      template<typename Visitor>
      vector<typename Visitor::result_type> visit( Visitor&& visitor )
      {
         vector<typename Visitor::result_type> results;
         for( auto& op : operations )
            results.push_back(op.visit( std::forward<Visitor>( visitor ) ));
         return results;
      }
      template<typename Visitor>
      vector<typename Visitor::result_type> visit( Visitor&& visitor )const
      {
         vector<typename Visitor::result_type> results;
         for( auto& op : operations )
            results.push_back(op.visit( std::forward<Visitor>( visitor ) ));
         return results;
      }

      void get_required_authorities( flat_set< wallet_name_type >& money,
                                     flat_set< wallet_name_type >& recovery,
                                     flat_set< wallet_name_type >& social,
                                     vector< authority >& other )const;

      bool has_pow_op()const;
   };

   struct signed_transaction : public transaction
   {
      signed_transaction( const transaction& trx = transaction() )
         : transaction(trx){}

      const signature_type& sign( const private_key_type& key, const chain_id_type& chain_id, canonical_signature_type canon_type/* = fc::ecc::fc_canonical*/ );

      signature_type sign( const private_key_type& key, const chain_id_type& chain_id, canonical_signature_type canon_type/* = fc::ecc::fc_canonical*/ )const;

      set<public_key_type> get_required_signatures(
         const chain_id_type& chain_id,
         const flat_set<public_key_type>& available_keys,
         const authority_getter& get_money,
         const authority_getter& get_recovery,
         const authority_getter& get_social,
         uint32_t max_recursion = XGT_MAX_SIG_CHECK_DEPTH,
         uint32_t max_membership = XGT_MAX_AUTHORITY_MEMBERSHIP,
         uint32_t max_wallet_auths = XGT_MAX_SIG_CHECK_WALLETS,
         canonical_signature_type canon_type = fc::ecc::fc_canonical
         )const;

      void verify_authority(
         const chain_id_type& chain_id,
         const authority_getter& get_money,
         const authority_getter& get_recovery,
         const authority_getter& get_social,
         uint32_t max_recursion/* = XGT_MAX_SIG_CHECK_DEPTH*/,
         uint32_t max_membership = XGT_MAX_AUTHORITY_MEMBERSHIP,
         uint32_t max_wallet_auths = XGT_MAX_SIG_CHECK_WALLETS,
         canonical_signature_type canon_type = fc::ecc::fc_canonical
         )const;

      set<public_key_type> minimize_required_signatures(
         const chain_id_type& chain_id,
         const flat_set<public_key_type>& available_keys,
         const authority_getter& get_money,
         const authority_getter& get_recovery,
         const authority_getter& get_social,
         uint32_t max_recursion = XGT_MAX_SIG_CHECK_DEPTH,
         uint32_t max_membership = XGT_MAX_AUTHORITY_MEMBERSHIP,
         uint32_t max_wallet_auths = XGT_MAX_SIG_CHECK_WALLETS,
         canonical_signature_type canon_type = fc::ecc::fc_canonical
         ) const;

      flat_set<public_key_type> get_signature_keys( const chain_id_type& chain_id, canonical_signature_type/* = fc::ecc::fc_canonical*/ )const;

      vector<signature_type> signatures;

      digest_type merkle_digest()const;

      void clear() { operations.clear(); signatures.clear(); }
   };

   struct annotated_signed_transaction : public signed_transaction {
      annotated_signed_transaction(){}
      annotated_signed_transaction( const signed_transaction& trx )
      :signed_transaction(trx),transaction_id(trx.id()){}

      transaction_id_type transaction_id;
      uint32_t            block_num = 0;
      uint32_t            transaction_num = 0;
   };


   /// @} transactions group

} } // xgt::protocol

FC_REFLECT( xgt::protocol::transaction, (ref_block_num)(ref_block_prefix)(expiration)(operations)(extensions) )
FC_REFLECT_DERIVED( xgt::protocol::signed_transaction, (xgt::protocol::transaction), (signatures) )
FC_REFLECT_DERIVED( xgt::protocol::annotated_signed_transaction, (xgt::protocol::signed_transaction), (transaction_id)(block_num)(transaction_num) );
