
#include <xgt/protocol/transaction.hpp>
#include <xgt/protocol/transaction_util.hpp>

#include <fc/io/raw.hpp>
#include <fc/bitutil.hpp>
#include <fc/smart_ref_impl.hpp>

#include <algorithm>

namespace xgt { namespace protocol {

digest_type signed_transaction::merkle_digest()const
{
   digest_type::encoder enc;
   fc::raw::pack( enc, *this );
   return enc.result();
}

digest_type transaction::digest()const
{
   digest_type::encoder enc;
   fc::raw::pack( enc, *this );
   return enc.result();
}

digest_type transaction::sig_digest( const chain_id_type& chain_id )const
{
   digest_type::encoder enc;
   fc::raw::pack( enc, chain_id );
   fc::raw::pack( enc, *this );
   return enc.result();
}

void transaction::validate() const
{
   FC_ASSERT( operations.size() > 0, "A transaction must have at least one operation", ("trx",*this) );
   for( const auto& op : operations )
      operation_validate(op);
}

xgt::protocol::transaction_id_type xgt::protocol::transaction::id() const
{
   auto h = digest();
   transaction_id_type result;
   memcpy(result._hash, h._hash, std::min(sizeof(result), sizeof(h)));
   return result;
}

const signature_type& xgt::protocol::signed_transaction::sign( const private_key_type& key, const chain_id_type& chain_id, canonical_signature_type canon_type )
{
   digest_type h = sig_digest( chain_id );
   signatures.push_back( key.sign_compact( h, canon_type ) );
   return signatures.back();
}

signature_type xgt::protocol::signed_transaction::sign( const private_key_type& key, const chain_id_type& chain_id, canonical_signature_type canon_type )const
{
   digest_type::encoder enc;
   fc::raw::pack( enc, chain_id );
   fc::raw::pack( enc, *this );
   return key.sign_compact( enc.result(), canon_type );
}

void transaction::set_expiration( fc::time_point_sec expiration_time )
{
    expiration = expiration_time;
}

void transaction::set_reference_block( const block_id_type& reference_block )
{
   ref_block_num = fc::endian_reverse_u32(reference_block._hash[0]);
   ref_block_prefix = reference_block._hash[1];
}

void transaction::get_required_authorities( flat_set< wallet_name_type >& money,
                                            flat_set< wallet_name_type >& recovery,
                                            flat_set< wallet_name_type >& social,
                                            vector< authority >& other )const
{
   for( const auto& op : operations )
      operation_get_required_authorities( op, money, recovery, social, other );
}

bool transaction::has_pow_op()const
{
   for (auto& op : operations) {
      if (is_pow_operation(op)) {
         return true;
      }
   }

   return false;
}

flat_set<public_key_type> signed_transaction::get_signature_keys( const chain_id_type& chain_id, canonical_signature_type canon_type )const
{ try {
   auto d = sig_digest( chain_id );
   flat_set<public_key_type> result;
   for( const auto&  sig : signatures )
   {
      XGT_ASSERT(
         result.insert( fc::ecc::public_key( sig, d, canon_type ) ).second,
         tx_duplicate_sig,
         "Duplicate Signature detected" );
   }
   return result;
} FC_CAPTURE_AND_RETHROW() }



set<public_key_type> signed_transaction::get_required_signatures(
   const chain_id_type& chain_id,
   const flat_set<public_key_type>& available_keys,
   const authority_getter& get_money,
   const authority_getter& get_recovery,
   const authority_getter& get_social,
   uint32_t max_recursion_depth,
   uint32_t max_membership,
   uint32_t max_wallet_auths,
   canonical_signature_type canon_type )const
{
   flat_set< wallet_name_type > required_money;
   flat_set< wallet_name_type > required_recovery;
   flat_set< wallet_name_type > required_social;
   vector< authority > other;
   get_required_authorities( required_money, required_recovery, required_social, other );

   /** social authority cannot be mixed with money authority in same transaction */
   if( required_social.size() ) {
      sign_state s( get_signature_keys( chain_id, canon_type ), get_social,available_keys );
      s.max_recursion = max_recursion_depth;
      s.max_membership = max_membership;
      s.max_wallet_auths = max_wallet_auths;

      FC_ASSERT( !required_recovery.size() );
      FC_ASSERT( !required_money.size() );
      for( auto& social : required_social )
         s.check_authority( social  );

      s.remove_unused_signatures();

      set<public_key_type> result;

      for( auto& provided_sig : s.provided_signatures )
         if( available_keys.find( provided_sig.first ) != available_keys.end() )
            result.insert( provided_sig.first );

      return result;
   }


   sign_state s( get_signature_keys( chain_id, canon_type ), get_money, available_keys );
   s.max_recursion = max_recursion_depth;
   s.max_membership = max_membership;
   s.max_wallet_auths = max_wallet_auths;

   for( const auto& auth : other )
      s.check_authority( auth );
   for( auto& recovery : required_recovery )
      s.check_authority( get_recovery( recovery ) );
   for( auto& money : required_money )
      s.check_authority( money  );

   s.remove_unused_signatures();

   set<public_key_type> result;

   for( auto& provided_sig : s.provided_signatures )
      if( available_keys.find( provided_sig.first ) != available_keys.end() )
         result.insert( provided_sig.first );

   return result;
}

set<public_key_type> signed_transaction::minimize_required_signatures(
   const chain_id_type& chain_id,
   const flat_set< public_key_type >& available_keys,
   const authority_getter& get_money,
   const authority_getter& get_recovery,
   const authority_getter& get_social,
   uint32_t max_recursion,
   uint32_t max_membership,
   uint32_t max_wallet_auths,
   canonical_signature_type canon_type
   ) const
{
   set< public_key_type > s = get_required_signatures( chain_id, available_keys, get_money, get_recovery, get_social, max_recursion, max_membership, max_wallet_auths, canon_type );
   flat_set< public_key_type > result( s.begin(), s.end() );

   for( const public_key_type& k : s )
   {
      result.erase( k );
      try
      {
         xgt::protocol::verify_authority(
            operations,
            result,
            get_money,
            get_recovery,
            get_social,
            max_recursion,
            max_membership,
            max_wallet_auths,
            false,
            flat_set< wallet_name_type >(),
            flat_set< wallet_name_type >(),
            flat_set< wallet_name_type >() );
         continue;  // element stays erased if verify_authority is ok
      }
      catch( const tx_missing_recovery_auth& e ) {}
      catch( const tx_missing_money_auth& e ) {}
      catch( const tx_missing_social_auth& e ) {}
      catch( const tx_missing_other_auth& e ) {}
      result.insert( k );
   }
   return set<public_key_type>( result.begin(), result.end() );
}

void signed_transaction::verify_authority(
   const chain_id_type& chain_id,
   const authority_getter& get_money,
   const authority_getter& get_recovery,
   const authority_getter& get_social,
   uint32_t max_recursion,
   uint32_t max_membership,
   uint32_t max_wallet_auths,
   canonical_signature_type canon_type )const
{ try {
   xgt::protocol::verify_authority(
      operations,
      get_signature_keys( chain_id, canon_type ),
      get_money,
      get_recovery,
      get_social,
      max_recursion,
      max_membership,
      max_wallet_auths,
      false,
      flat_set< wallet_name_type >(),
      flat_set< wallet_name_type >(),
      flat_set< wallet_name_type >() );
} FC_CAPTURE_AND_RETHROW( (*this) ) }

} } // xgt::protocol
