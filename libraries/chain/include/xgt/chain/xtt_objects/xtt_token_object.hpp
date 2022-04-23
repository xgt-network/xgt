#pragma once

#include <xgt/chain/xgt_fwd.hpp>
#include <xgt/chain/xgt_object_types.hpp>
#include <xgt/protocol/xtt_operations.hpp>

namespace xgt { namespace chain {

using protocol::curve_id;
using protocol::xtt_ticker_type;

enum class xtt_phase : uint8_t
{
   setup,
   setup_completed,
   ico,
   ico_completed,
   launch_failed,
   launch_success
};

/**Note that the object represents both liquid and vesting variant of XTT.
 * The same object is returned by indices when searched by liquid/vesting symbol/nai.
 */
class xtt_token_object : public object< xtt_token_object_type, xtt_token_object >
{
   public:
      xtt_token_object() = default;;

public:
   asset_symbol_type get_stripped_symbol() const
   {
      return asset_symbol_type::from_asset_num( liquid_symbol.get_stripped_precision_xtt_num() );
   }

   // id_type is actually oid<xtt_token_object>
   id_type              id;

   asset_symbol_type    liquid_symbol;
   wallet_name_type     control_account;
   xtt_ticker_type      desired_ticker;
   xtt_phase            phase                           = xtt_phase::setup;
   share_type           current_supply                  = 0;

   uint128_t            recent_claims;

   /// set_setup_parameters
   bool                 allow_voting = true;

   /// set_runtime_parameters
   uint32_t             reverse_auction_window_seconds = XGT_REVERSE_AUCTION_WINDOW_SECONDS;

   uint32_t             vote_regeneration_period_seconds = XGT_VOTING_ENERGY_REGENERATION_SECONDS;
   uint32_t             votes_per_regeneration_period = XTT_DEFAULT_VOTES_PER_REGEN_PERIOD;

   uint128_t            content_constant = XGT_CONTENT_CONSTANT_HF0;

   bool                 allow_downvotes = true;

   ///parameters for 'xtt_setup_operation'
   int64_t                       max_supply = 0;
};

class xtt_ico_object : public object< xtt_ico_object_type, xtt_ico_object >
{
   public:
      xtt_ico_object() = default;;

public:
   id_type id;
   asset_symbol_type             symbol;
   time_point_sec                contribution_begin_time;
   time_point_sec                contribution_end_time;
   time_point_sec                launch_time;
   share_type                    xgt_units_min         = -1;
   uint32_t                      min_unit_ratio          = 0;
   uint32_t                      max_unit_ratio          = 0;
   asset                         contributed             = asset( 0, XGT_SYMBOL );
   share_type                    processed_contributions = 0;
};

struct shared_xtt_generation_unit
{
   public:
      shared_xtt_generation_unit() = default;;
   public:

   typedef boost::container::flat_map< xgt::protocol::unit_target_type, uint16_t > unit_map_type;

   unit_map_type xgt_unit;
   unit_map_type token_unit;

   uint32_t xgt_unit_sum()const
   {
      uint32_t result = 0;
      for( const auto& e : xgt_unit )
      {
         result += e.second;
      }
      return result;
   }

   uint32_t token_unit_sum()const
   {
      uint32_t result = 0;
      for( const auto& e : token_unit )
      {
         result += e.second;
      }
      return result;
   }

   shared_xtt_generation_unit& operator =( const xgt::protocol::xtt_generation_unit& g )
   {
      xgt_unit.insert( g.xgt_unit.begin(), g.xgt_unit.end() );
      token_unit.insert( g.token_unit.begin(), g.token_unit.end() );
      return *this;
   }

   operator xgt::protocol::xtt_generation_unit()const
   {
      xgt::protocol::xtt_generation_unit g;
      g.xgt_unit.insert( xgt_unit.begin(), xgt_unit.end() );
      g.token_unit.insert( token_unit.begin(), token_unit.end() );
      return g;
   }
};

class xtt_ico_tier_object : public object< xtt_ico_tier_object_type, xtt_ico_tier_object >
{
   public:
      xtt_ico_tier_object() = default;;

public:
   id_type                                id;
   asset_symbol_type                      symbol;
   share_type                             xgt_units_cap = -1;
   shared_xtt_generation_unit             generation_unit;
};

class xtt_contribution_object : public object< xtt_contribution_object_type, xtt_contribution_object >
{
   public:
      xtt_contribution_object() = default;;
   public:
   id_type                               id;
   asset_symbol_type                     symbol;
   wallet_name_type                      contributor;
   uint32_t                              contribution_id;
   asset                                 contribution;
};

struct by_symbol_contributor;
struct by_contributor;
struct by_symbol_id;

typedef multi_index_container <
   xtt_contribution_object,
   indexed_by <
      ordered_unique< tag< by_id >,
         member< xtt_contribution_object, xtt_contribution_object_id_type, &xtt_contribution_object::id > >,
      ordered_unique< tag< by_symbol_contributor >,
         composite_key< xtt_contribution_object,
            member< xtt_contribution_object, asset_symbol_type, &xtt_contribution_object::symbol >,
            member< xtt_contribution_object, wallet_name_type, &xtt_contribution_object::contributor >,
            member< xtt_contribution_object, uint32_t, &xtt_contribution_object::contribution_id >
         >
      >,
      ordered_unique< tag< by_symbol_id >,
         composite_key< xtt_contribution_object,
            member< xtt_contribution_object, asset_symbol_type, &xtt_contribution_object::symbol >,
            member< xtt_contribution_object, xtt_contribution_object_id_type, &xtt_contribution_object::id >
         >
      >
#ifndef IS_LOW_MEM
      ,
      ordered_unique< tag< by_contributor >,
         composite_key< xtt_contribution_object,
            member< xtt_contribution_object, wallet_name_type, &xtt_contribution_object::contributor >,
            member< xtt_contribution_object, asset_symbol_type, &xtt_contribution_object::symbol >,
            member< xtt_contribution_object, uint32_t, &xtt_contribution_object::contribution_id >
         >
      >
#endif
   >
> xtt_contribution_index;

struct by_symbol;
struct by_control_account;

typedef multi_index_container <
   xtt_token_object,
   indexed_by <
      ordered_unique< tag< by_id >,
         member< xtt_token_object, xtt_token_id_type, &xtt_token_object::id > >,
      ordered_unique< tag< by_symbol >,
         member< xtt_token_object, asset_symbol_type, &xtt_token_object::liquid_symbol > >,
      ordered_unique< tag< by_control_account >,
         composite_key< xtt_token_object,
            member< xtt_token_object, wallet_name_type, &xtt_token_object::control_account >,
            member< xtt_token_object, asset_symbol_type, &xtt_token_object::liquid_symbol >
         >
      >
   >
> xtt_token_index;

typedef multi_index_container <
   xtt_ico_object,
   indexed_by <
      ordered_unique< tag< by_id >,
         member< xtt_ico_object, xtt_ico_object_id_type, &xtt_ico_object::id > >,
      ordered_unique< tag< by_symbol >,
         member< xtt_ico_object, asset_symbol_type, &xtt_ico_object::symbol > >
   >
> xtt_ico_index;

struct by_symbol_xgt_units_cap;

typedef multi_index_container <
   xtt_ico_tier_object,
   indexed_by <
      ordered_unique< tag< by_id >,
         member< xtt_ico_tier_object, xtt_ico_tier_object_id_type, &xtt_ico_tier_object::id > >,
      ordered_unique< tag< by_symbol_xgt_units_cap >,
         composite_key< xtt_ico_tier_object,
            member< xtt_ico_tier_object, asset_symbol_type, &xtt_ico_tier_object::symbol >,
            member< xtt_ico_tier_object, share_type, &xtt_ico_tier_object::xgt_units_cap >
         >,
         composite_key_compare< std::less< asset_symbol_type >, std::less< share_type > >
      >
   >
> xtt_ico_tier_index;

} } // namespace xgt::chain

FC_REFLECT_ENUM( xgt::chain::xtt_phase,
                  (setup)
                  (setup_completed)
                  (ico)
                  (ico_completed)
                  (launch_failed)
                  (launch_success)
)

FC_REFLECT( xgt::chain::xtt_token_object,
   (id)
   (liquid_symbol)
   (control_account)
   (desired_ticker)
   (phase)
   (current_supply)
   (recent_claims)
   (allow_downvotes)
   (allow_voting)
   (reverse_auction_window_seconds)
   (vote_regeneration_period_seconds)
   (votes_per_regeneration_period)
   (content_constant)
   (max_supply)
)

FC_REFLECT( xgt::chain::xtt_ico_object,
   (id)
   (symbol)
   (contribution_begin_time)
   (contribution_end_time)
   (launch_time)
   (xgt_units_min)
   (min_unit_ratio)
   (max_unit_ratio)
   (contributed)
   (processed_contributions)
)

FC_REFLECT( xgt::chain::shared_xtt_generation_unit,
   (xgt_unit)
   (token_unit) )

FC_REFLECT( xgt::chain::xtt_ico_tier_object,
   (id)
   (symbol)
   (xgt_units_cap)
   (generation_unit)
)

FC_REFLECT( xgt::chain::xtt_contribution_object,
   (id)
   (symbol)
   (contributor)
   (contribution_id)
   (contribution)
)

CHAINBASE_SET_INDEX_TYPE( xgt::chain::xtt_token_object, xgt::chain::xtt_token_index )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::xtt_ico_object, xgt::chain::xtt_ico_index )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::xtt_contribution_object, xgt::chain::xtt_contribution_index )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::xtt_ico_tier_object, xgt::chain::xtt_ico_tier_index )
