#pragma once
#include <xgt/protocol/required_automated_actions.hpp>
#include <xgt/protocol/optional_automated_actions.hpp>
#include <xgt/protocol/base.hpp>

namespace xgt { namespace protocol {

   typedef vector< required_automated_action > required_automated_actions;
   typedef vector< optional_automated_action > optional_automated_actions;

   typedef static_variant<
      void_t,
      version,                // Normal witness version reporting, for diagnostics and voting
      hardfork_version_vote,   // Voting for the next hardfork to trigger
      required_automated_actions,
      optional_automated_actions
      >                                block_header_extensions;

   typedef flat_set<block_header_extensions > block_header_extensions_type;

   struct block_header
   {
      digest_type                   digest()const;
      block_id_type                 previous;
      uint32_t                      block_num()const { return num_from_id(previous) + 1; }
      fc::time_point_sec            timestamp;
      string                        witness;
      checksum_type                 transaction_merkle_root;
      block_header_extensions_type  extensions;

      uint32_t pow_summary = 0;

      static uint32_t num_from_id(const block_id_type& id);
   };

   struct signed_block_header : public block_header
   {
      block_id_type              id()const;
      fc::ecc::public_key        signee( fc::ecc::canonical_signature_type canon_type = fc::ecc::bip_0062 )const;
      void                       sign( const fc::ecc::private_key& signer, fc::ecc::canonical_signature_type canon_type = fc::ecc::bip_0062 );
      bool                       validate_signee( const fc::ecc::public_key& expected_signee, fc::ecc::canonical_signature_type canon_type = fc::ecc::bip_0062 )const;

      signature_type             witness_signature;
   };


} } // xgt::protocol

FC_REFLECT_TYPENAME( xgt::protocol::block_header_extensions )

FC_REFLECT( xgt::protocol::block_header, (previous)(timestamp)(witness)(transaction_merkle_root)(extensions) )
FC_REFLECT_DERIVED( xgt::protocol::signed_block_header, (xgt::protocol::block_header), (witness_signature) )
