#pragma once

#include <xgt/protocol/types.hpp>
#include <xgt/protocol/authority.hpp>
#include <xgt/protocol/version.hpp>

#include <fc/time.hpp>

namespace xgt { namespace protocol {

   struct base_operation
   {
      void get_required_authorities( vector<authority>& )const {}
      void get_required_money_authorities( flat_set<wallet_name_type>& )const {}
      void get_required_social_authorities( flat_set<wallet_name_type>& )const {}
      void get_required_recovery_authorities( flat_set<wallet_name_type>& )const {}

      bool is_virtual()const { return false; }
      bool is_wallet_create()const { return false; }
      bool is_wallet_update()const { return false; }
      bool is_contract_create()const { return false; }
      bool is_contract_invoke()const { return false; }
      bool is_contract_deploy()const { return false; }
      bool is_contract_call()const { return false; }
      bool is_pow()const { return false; }
      void validate()const {}
      uint64_t energy_cost()const { return 0; }
   };

   struct virtual_operation : public base_operation
   {
      bool is_virtual()const { return true; }
      void validate()const { FC_ASSERT( false, "This is a virtual operation" ); }
   };

   typedef static_variant<
      void_t
      >                                future_extensions;

   typedef flat_set<future_extensions> extensions_type;


} } // xgt::protocol

FC_REFLECT_TYPENAME( xgt::protocol::future_extensions )
