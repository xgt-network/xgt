#pragma once
#include <xgt/protocol/types.hpp>

namespace xgt { namespace protocol {

   struct authority
   {
      authority(){}

      enum classification
      {
         recovery = 0,
         money    = 1,
         key      = 2,
         social   = 3
      };

      template< class ...Args >
      authority( uint32_t threshold, Args... auths )
         : weight_threshold( threshold )
      {
         add_authorities( auths... );
      }

      void add_authority( const public_key_type& k, weight_type w );
      void add_authority( const wallet_name_type& k, weight_type w );

      template< typename AuthType >
      void add_authorities( AuthType k, weight_type w )
      {
         add_authority( k, w );
      }

      template< typename AuthType, class ...Args >
      void add_authorities( AuthType k, weight_type w, Args... auths )
      {
         add_authority( k, w );
         add_authorities( auths... );
      }

      vector< public_key_type > get_keys()const;

      bool     is_impossible()const;
      uint32_t num_auths()const;
      void     clear();
      void     validate()const;

      typedef flat_map< wallet_name_type, weight_type >              wallet_authority_map;
      typedef flat_map< public_key_type, weight_type >               key_authority_map;

      uint32_t                                                       weight_threshold = 0;
      wallet_authority_map                                           wallet_auths;
      key_authority_map                                              key_auths;
   };

template< typename AuthorityType >
void add_authority_wallets(
   flat_set<wallet_name_type>& result,
   const AuthorityType& a
   )
{
   for( auto& item : a.wallet_auths )
      result.insert( item.first );
}

/**
 * Names must comply with the following grammar (RFC 1035):
 * <domain> ::= <subdomain> | " "
 * <subdomain> ::= <label> | <subdomain> "." <label>
 * <label> ::= <letter> [ [ <ldh-str> ] <let-dig> ]
 * <ldh-str> ::= <let-dig-hyp> | <let-dig-hyp> <ldh-str>
 * <let-dig-hyp> ::= <let-dig> | "-"
 * <let-dig> ::= <letter> | <digit>
 *
 * Which is equivalent to the following:
 *
 * <domain> ::= <subdomain> | " "
 * <subdomain> ::= <label> ("." <label>)*
 * <label> ::= <letter> [ [ <let-dig-hyp>+ ] <let-dig> ]
 * <let-dig-hyp> ::= <let-dig> | "-"
 * <let-dig> ::= <letter> | <digit>
 *
 * I.e. a valid name consists of a dot-separated sequence
 * of one or more labels consisting of the following rules:
 *
 * - Each label is three characters or more
 * - Each label begins with a letter
 * - Each label ends with a letter or digit
 * - Each label contains only letters, digits or hyphens
 *
 * In addition we require the following:
 *
 * - All letters are lowercase
 * - Length is between (inclusive) XGT_MIN_WALLET_NAME_LENGTH and XGT_MAX_WALLET_NAME_LENGTH
 */
bool is_valid_wallet_name( const string& name );

bool operator == ( const authority& a, const authority& b );

} } // namespace xgt::protocol


FC_REFLECT_TYPENAME( xgt::protocol::authority::wallet_authority_map)
FC_REFLECT_TYPENAME( xgt::protocol::authority::key_authority_map)
FC_REFLECT( xgt::protocol::authority, (weight_threshold)(wallet_auths)(key_auths) )
FC_REFLECT_ENUM( xgt::protocol::authority::classification, (recovery)(money)(key)(social) )
