#pragma once

#include <xgt/protocol/base.hpp>
#include <xgt/protocol/asset.hpp>
#include <xgt/protocol/misc_utilities.hpp>

#define XTT_MAX_UNIT_ROUTES            10
#define XTT_MAX_UNIT_COUNT             20
#define XTT_MAX_DECIMAL_PLACES         8

namespace xgt { namespace protocol {

/**
 * This operation introduces new XTT into blockchain as identified by
 * Numerical Asset Identifier (NAI). Also the XTT precision (decimal points)
 * is explicitly provided.
 */
struct xtt_create_operation : public base_operation
{
   wallet_name_type control_account;
   asset_symbol_type symbol;

   xtt_ticker_type   desired_ticker;

   /// The amount to be transfered from @account to null account as elevation fee.
   asset             xtt_creation_fee;
   /// Separately provided precision for clarity and redundancy.
   uint8_t           precision;

   extensions_type   extensions;

   void validate()const;

   void get_required_money_authorities( flat_set<wallet_name_type>& a )const
   { a.insert( control_account ); }
};

struct xtt_generation_unit
{
   flat_map< unit_target_type, uint16_t > xgt_unit;
   flat_map< unit_target_type, uint16_t > token_unit;

   uint32_t xgt_unit_sum()const;
   uint32_t token_unit_sum()const;

   void validate()const;
};

struct xtt_capped_generation_policy
{
   xtt_generation_unit generation_unit;

   extensions_type     extensions;

   void validate()const;
};

typedef static_variant<
   xtt_capped_generation_policy
   > xtt_generation_policy;

struct xtt_setup_operation : public base_operation
{
   wallet_name_type control_account;
   asset_symbol_type symbol;

   int64_t                 max_supply = XGT_MAX_SHARE_SUPPLY;

   time_point_sec          contribution_begin_time;
   time_point_sec          contribution_end_time;
   time_point_sec          launch_time;

   share_type              xgt_units_min;

   uint32_t                min_unit_ratio = 0;
   uint32_t                max_unit_ratio = 0;

   extensions_type         extensions;

   void validate()const;

   void get_required_money_authorities( flat_set<wallet_name_type>& a )const
   { a.insert( control_account ); }
};

struct xtt_setup_ico_tier_operation : public base_operation
{
   wallet_name_type      control_account;
   asset_symbol_type     symbol;

   share_type            xgt_units_cap;
   xtt_generation_policy generation_policy;
   bool                  remove = false;

   extensions_type       extensions;

   void validate()const;

   void get_required_active_authorities( flat_set<wallet_name_type>& a )const
   { a.insert( control_account ); }
};

struct xtt_param_allow_voting
{
   bool value = true;
};

typedef static_variant<
   xtt_param_allow_voting
   > xtt_setup_parameter;

struct xtt_param_windows_v1
{
   uint32_t reverse_auction_window_seconds = 0;        // XGT_REVERSE_AUCTION_WINDOW_SECONDS
};

struct xtt_param_vote_regeneration_period_seconds_v1
{
   uint32_t vote_regeneration_period_seconds = 0;      // XGT_VOTING_ENERGY_REGENERATION_SECONDS
   uint32_t votes_per_regeneration_period = 0;
};

struct xtt_param_allow_downvotes
{
   bool value = true;
};

typedef static_variant<
   xtt_param_windows_v1,
   xtt_param_vote_regeneration_period_seconds_v1,
   xtt_param_allow_downvotes
   > xtt_runtime_parameter;

struct xtt_set_setup_parameters_operation : public base_operation
{
   wallet_name_type                 control_account;
   asset_symbol_type                symbol;
   flat_set< xtt_setup_parameter >  setup_parameters;
   extensions_type                  extensions;

   void validate()const;

   void get_required_money_authorities( flat_set<wallet_name_type>& a )const
   { a.insert( control_account ); }
};

struct xtt_set_runtime_parameters_operation : public base_operation
{
   wallet_name_type                   control_account;
   asset_symbol_type                   symbol;
   flat_set< xtt_runtime_parameter >   runtime_parameters;
   extensions_type                     extensions;

   void validate()const;

   void get_required_money_authorities( flat_set<wallet_name_type>& a )const
   { a.insert( control_account ); }
};

struct xtt_contribute_operation : public base_operation
{
   wallet_name_type   contributor;
   asset_symbol_type  symbol;
   uint32_t           contribution_id;
   asset              contribution;
   extensions_type    extensions;

   void validate() const;
   void get_required_money_authorities( flat_set<wallet_name_type>& a )const
   { a.insert( contributor ); }
};

} }

FC_REFLECT(
   xgt::protocol::xtt_create_operation,
   (control_account)
   (symbol)
   (desired_ticker)
   (xtt_creation_fee)
   (precision)
   (extensions)
)

FC_REFLECT(
   xgt::protocol::xtt_setup_operation,
   (control_account)
   (symbol)
   (max_supply)
   (contribution_begin_time)
   (contribution_end_time)
   (launch_time)
   (xgt_units_min)
   (min_unit_ratio)
   (max_unit_ratio)
   (extensions)
   )

FC_REFLECT(
   xgt::protocol::xtt_generation_unit,
   (xgt_unit)
   (token_unit)
   )


FC_REFLECT(
   xgt::protocol::xtt_capped_generation_policy,
   (generation_unit)
   (extensions)
   )

FC_REFLECT(
   xgt::protocol::xtt_setup_ico_tier_operation,
   (control_account)
   (symbol)
   (xgt_units_cap)
   (generation_policy)
   (remove)
   (extensions)
   )

FC_REFLECT(
   xgt::protocol::xtt_param_allow_voting,
   (value)
   )

FC_REFLECT_TYPENAME( xgt::protocol::xtt_setup_parameter )

FC_REFLECT(
   xgt::protocol::xtt_param_windows_v1,
   (reverse_auction_window_seconds)
   )

FC_REFLECT(
   xgt::protocol::xtt_param_vote_regeneration_period_seconds_v1,
   (vote_regeneration_period_seconds)
   (votes_per_regeneration_period)
   )

FC_REFLECT(
   xgt::protocol::xtt_param_allow_downvotes,
   (value)
)

FC_REFLECT_TYPENAME(
   xgt::protocol::xtt_runtime_parameter
   )

FC_REFLECT(
   xgt::protocol::xtt_set_setup_parameters_operation,
   (control_account)
   (symbol)
   (setup_parameters)
   (extensions)
   )

FC_REFLECT(
   xgt::protocol::xtt_set_runtime_parameters_operation,
   (control_account)
   (symbol)
   (runtime_parameters)
   (extensions)
   )

FC_REFLECT(
   xgt::protocol::xtt_contribute_operation,
   (contributor)
   (symbol)
   (contribution_id)
   (contribution)
   (extensions)
   )
