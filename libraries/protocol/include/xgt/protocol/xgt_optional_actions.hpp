#pragma once
#include <xgt/protocol/types.hpp>
#include <xgt/protocol/base.hpp>
#include <xgt/protocol/asset.hpp>
#include <xgt/protocol/xtt_operations.hpp>

namespace xgt { namespace protocol {

   struct example_optional_action : public base_operation
   {
      wallet_name_type wallet;

      void validate()const;
      void get_required_money_authorities( flat_set<wallet_name_type>& a )const{ a.insert(wallet); }
   };

   struct xtt_token_emission_action : public base_operation
   {
      wallet_name_type                         control_wallet;
      asset_symbol_type                        symbol;
      time_point_sec                           emission_time;
      flat_map< unit_target_type, share_type > emissions;

      extensions_type                          extensions;

      void validate()const;
      void get_required_money_authorities( flat_set<wallet_name_type>& a )const{ a.insert(control_wallet); }
   };

} } // xgt::protocol

FC_REFLECT( xgt::protocol::example_optional_action, (wallet) )

FC_REFLECT( xgt::protocol::xtt_token_emission_action, (control_wallet)(symbol)(emission_time)(emissions)(extensions) )
