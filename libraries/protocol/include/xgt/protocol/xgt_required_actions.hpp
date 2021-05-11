#pragma once

#include <xgt/protocol/types.hpp>
#include <xgt/protocol/base.hpp>
#include <xgt/protocol/asset.hpp>
#include <xgt/protocol/misc_utilities.hpp>

namespace xgt { namespace protocol {

   struct example_required_action : public base_operation
   {
      wallet_name_type account;

      void validate()const;
      void get_required_money_authorities( flat_set<wallet_name_type>& a )const{ a.insert(account); }

      friend bool operator==( const example_required_action& lhs, const example_required_action& rhs );
   };

   struct xtt_refund_action : public base_operation
   {
      wallet_name_type contributor;
      asset_symbol_type symbol;
      uint32_t contribution_id;
      asset refund;

      void validate()const;
      void get_required_money_authorities( flat_set<wallet_name_type>& a )const { a.insert( contributor ); }

      friend bool operator==( const xtt_refund_action& lhs, const xtt_refund_action& rhs );
   };

   struct xtt_ico_launch_action : public base_operation
   {
      wallet_name_type control_account;
      asset_symbol_type symbol;

      void validate()const;
      void get_required_money_authorities( flat_set<wallet_name_type>& a )const { a.insert( control_account ); }

      friend bool operator==( const xtt_ico_launch_action& lhs, const xtt_ico_launch_action& rhs );
   };

   struct xtt_ico_evaluation_action : public base_operation
   {
      wallet_name_type control_account;
      asset_symbol_type symbol;

      void validate()const;
      void get_required_money_authorities( flat_set<wallet_name_type>& a )const { a.insert( control_account ); }

      friend bool operator==( const xtt_ico_evaluation_action& lhs, const xtt_ico_evaluation_action& rhs );
   };

   struct xtt_token_launch_action : public base_operation
   {
      wallet_name_type control_account;
      asset_symbol_type symbol;

      void validate()const;
      void get_required_money_authorities( flat_set<wallet_name_type>& a )const { a.insert( control_account ); }

      friend bool operator==( const xtt_token_launch_action& lhs, const xtt_token_launch_action& rhs );
   };
} } // xgt::protocol

FC_REFLECT( xgt::protocol::example_required_action, (account) )

FC_REFLECT( xgt::protocol::xtt_refund_action, (contributor)(symbol)(contribution_id)(refund) )
FC_REFLECT( xgt::protocol::xtt_ico_launch_action, (control_account)(symbol) )
FC_REFLECT( xgt::protocol::xtt_ico_evaluation_action, (control_account)(symbol) )
FC_REFLECT( xgt::protocol::xtt_token_launch_action, (control_account)(symbol) )
