#pragma once

#include <fc/optional.hpp>
#include <xgt/chain/database.hpp>
#include <xgt/chain/xtt_objects.hpp>
#include <xgt/protocol/asset_symbol.hpp>

namespace xgt { namespace chain { namespace util { namespace xtt {

const xtt_token_object* find_token( const database& db, uint32_t nai );
const xtt_token_object* find_token( const database& db, asset_symbol_type symbol, bool precision_agnostic = false );
const xtt_token_emissions_object* last_emission( const database& db, const asset_symbol_type& symbol );
fc::optional< time_point_sec > last_emission_time( const database& db, const asset_symbol_type& symbol );
fc::optional< time_point_sec > next_emission_time( const database& db, const asset_symbol_type& symbol, time_point_sec time = time_point_sec() );
const xtt_token_emissions_object* get_emission_object( const database& db, const asset_symbol_type& symbol, time_point_sec t );
flat_map< unit_target_type, share_type > generate_emissions( const xtt_token_object& token, const xtt_token_emissions_object& emission, time_point_sec t );

namespace ico {

bool schedule_next_refund( database& db, const asset_symbol_type& a );

fc::optional< share_type > xgt_units_hard_cap( database& db, const asset_symbol_type& a );
std::size_t ico_tier_size( database& db, const asset_symbol_type& symbol );
void remove_ico_objects( database& db, const asset_symbol_type& symbol );

} // xgt::chain::util::xtt::ico

} } } } // xgt::chain::util::xtt
