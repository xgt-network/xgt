#pragma once

#include <xgt/protocol/authority.hpp>
#include <xgt/protocol/types.hpp>

namespace xgt { namespace protocol {

typedef std::function<authority(const string&)> authority_getter;

struct sign_state
{
      /** returns true if we have a signature for this key or can
       * produce a signature for this key, else returns false.
       */
      bool signed_by( const public_key_type& k );
      bool check_authority( string id );

      /**
       *  Checks to see if we have signatures of the money authorites of
       *  the accounts specified in authority or the keys specified.
       */
      bool check_authority( const authority& au, uint32_t depth = 0, uint32_t account_auth_count = 0 );

      bool remove_unused_signatures();

      sign_state( const flat_set<public_key_type>& sigs,
                  const authority_getter& a,
                  const flat_set<public_key_type>& keys );

      const authority_getter&          get_money;
      const flat_set<public_key_type>& available_keys;

      flat_map<public_key_type,bool>   provided_signatures;
      flat_set<string>                 approved_by;
      uint32_t                         max_recursion = XGT_MAX_SIG_CHECK_DEPTH;
      uint32_t                         max_membership = ~0;
      uint32_t                         max_wallet_auths = ~0;

      private:
         bool check_authority_impl( const authority& au, uint32_t depth, uint32_t* account_auth_count );
};

} } // xgt::protocol
