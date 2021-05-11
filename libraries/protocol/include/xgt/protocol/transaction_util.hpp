#pragma once
#include <xgt/protocol/sign_state.hpp>
#include <xgt/protocol/exceptions.hpp>

namespace xgt { namespace protocol {

template< typename AuthContainerType >
void verify_authority( const vector<AuthContainerType>& auth_containers, const flat_set<public_key_type>& sigs,
                       const authority_getter& get_money,
                       const authority_getter& get_recovery,
                       const authority_getter& get_social,
                       uint32_t max_recursion_depth = XGT_MAX_SIG_CHECK_DEPTH,
                       uint32_t max_membership = XGT_MAX_AUTHORITY_MEMBERSHIP,
                       uint32_t max_wallet_auths = XGT_MAX_SIG_CHECK_WALLETS,
                       bool allow_committe = false,
                       const flat_set< wallet_name_type >& money_approvals = flat_set< wallet_name_type >(),
                       const flat_set< wallet_name_type >& recovery_approvals = flat_set< wallet_name_type >(),
                       const flat_set< wallet_name_type >& social_approvals = flat_set< wallet_name_type >()
                       )
{ try {
   flat_set< wallet_name_type > required_money;
   flat_set< wallet_name_type > required_recovery;
   flat_set< wallet_name_type > required_social;
   vector< authority > other;

   get_required_auth_visitor auth_visitor( required_money, required_recovery, required_social, other );

   for( const auto& a : auth_containers )
      auth_visitor( a );

   /**
    *  Transactions with operations required social authority cannot be combined
    *  with transactions requiring money or recovery authority. This is for ease of
    *  implementation. Future versions of authority verification may be able to
    *  check for the merged authority of money and social.
    */
   if( required_social.size() ) {
      FC_ASSERT( required_money.size() == 0 );
      FC_ASSERT( required_recovery.size() == 0 );
      FC_ASSERT( other.size() == 0 );

      flat_set< public_key_type > avail;
      sign_state s(sigs,get_social,avail);
      s.max_recursion = max_recursion_depth;
      s.max_membership = max_membership;
      s.max_wallet_auths = max_wallet_auths;
      for( auto& id : social_approvals )
         s.approved_by.insert( id );
      for( const auto& id : required_social )
      {
         XGT_ASSERT( s.check_authority(id) ||
               s.check_authority(get_money(id)) ||
               s.check_authority(get_recovery(id)),
               tx_missing_social_auth, "Missing Social Authority ${id}",
               ("id",id)
               ("social",get_social(id))
               ("money",get_money(id))
               ("recovery",get_recovery(id)) );
      }
      /*
      XGT_ASSERT(
         !s.remove_unused_signatures(),
         tx_irrelevant_sig,
         "Unnecessary signature(s) detected"
         );
      */
      return;
   }

   flat_set< public_key_type > avail;
   sign_state s(sigs,get_money,avail);
   s.max_recursion = max_recursion_depth;
   s.max_membership = max_membership;
   s.max_wallet_auths = max_wallet_auths;
   for( auto& id : money_approvals )
      s.approved_by.insert( id );
   for( auto& id : recovery_approvals )
      s.approved_by.insert( id );

   for( const auto& auth : other )
   {
      XGT_ASSERT( s.check_authority(auth), tx_missing_other_auth, "Missing Authority", ("auth",auth)("sigs",sigs) );
   }

   // fetch all of the top level authorities
   for( const auto& id : required_money )
   {
      XGT_ASSERT( s.check_authority(id) ||
            s.check_authority(get_recovery(id)),
            tx_missing_money_auth, "Missing Money Authority ${id}", ("id",id)("auth",get_money(id))("recovery",get_recovery(id)) );
   }

   for( const auto& id : required_recovery )
   {
      XGT_ASSERT( recovery_approvals.find(id) != recovery_approvals.end() ||
            s.check_authority(get_recovery(id)),
            tx_missing_recovery_auth, "Missing Recovery Authority ${id}", ("id",id)("auth",get_recovery(id)) );
   }

   /*
   XGT_ASSERT(
      !s.remove_unused_signatures(),
      tx_irrelevant_sig,
      "Unnecessary signature(s) detected"
      );
   */
} FC_CAPTURE_AND_RETHROW( (auth_containers)(sigs) ) }

} } // xgt::protocol
