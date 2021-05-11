#pragma once

#include <cstdint>

#include <fc/saturation.hpp>
#include <fc/uint128.hpp>
#include <fc/time.hpp>
#include <fc/exception/exception.hpp>
#include <fc/reflect/reflect.hpp>

namespace xgt { namespace chain { namespace util {

struct energybar_params
{
   int64_t    max_energy       = 0;
   uint32_t   regen_time       = 0;

   energybar_params() {}
   energybar_params( int64_t mm, uint32_t rt )
      : max_energy(mm), regen_time(rt) {}

   void validate()const
   { try{
      FC_ASSERT( max_energy >= 0 );
   } FC_CAPTURE_AND_RETHROW( (max_energy) ) }
};

struct energybar
{
   int64_t    current_energy   = 0;
   uint32_t   last_update_time = 0;

   energybar() {}
   energybar( int64_t m, uint32_t t )
      : current_energy(m), last_update_time(t) {}

   template< bool skip_cap_regen = false >
   void regenerate_energy( const energybar_params& params, uint32_t now )
   {
      params.validate();

      FC_ASSERT( now >= last_update_time );
      uint32_t dt = now - last_update_time;
      if( current_energy >= params.max_energy )
      {
         current_energy = params.max_energy;
         last_update_time = now;
         return;
      }

      if( !skip_cap_regen )
         dt = (dt > params.regen_time) ? params.regen_time : dt;

      fc::uint128_t max_energy_dt = uint64_t( params.max_energy >= 0 ? params.max_energy : 0 );
      ilog("??? max_energy_dt ${i}", ("i",max_energy_dt));
      max_energy_dt *= dt;
      ilog("??? max_energy_dt ${i}", ("i",max_energy_dt));
      uint64_t u_regen = (max_energy_dt / params.regen_time).to_uint64();
      ilog("??? u_regen ${i}", ("i",u_regen));
      FC_ASSERT( u_regen <= std::numeric_limits< int64_t >::max() );
      int64_t new_current_energy = fc::signed_sat_add( current_energy, int64_t( u_regen ) );
      ilog("??? new_current_energy ${i}", ("i",new_current_energy));
      current_energy = (new_current_energy > params.max_energy) ? params.max_energy : new_current_energy;
      ilog("??? current_energy ${i}", ("i",current_energy));

      last_update_time = now;
   }

   template< bool skip_cap_regen = false >
   void regenerate_energy( const energybar_params& params, fc::time_point_sec now )
   {
      regenerate_energy< skip_cap_regen >( params, now.sec_since_epoch() );
   }

   bool has_energy( int64_t energy_needed )const
   {
      return (energy_needed <= 0) || (current_energy >= energy_needed);
   }

   bool has_energy( uint64_t energy_needed )const
   {
      FC_ASSERT( energy_needed <= std::numeric_limits< int64_t >::max() );
      return has_energy( (int64_t) energy_needed );
   }

   void use_energy( int64_t energy_used, int64_t min_energy = std::numeric_limits< uint64_t >::min() )
   {
      current_energy = fc::signed_sat_sub( current_energy, energy_used );

      if( current_energy < min_energy )
      {
         current_energy = min_energy;
      }
   }

   void use_energy( uint64_t energy_used, int64_t min_energy = std::numeric_limits< uint64_t >::min() )
   {
      FC_ASSERT( energy_used <= std::numeric_limits< int64_t >::max() );
      use_energy( (int64_t) energy_used, min_energy );
   }
};

template< typename PropType, typename AccountType >
void update_energybar( const PropType& gpo, AccountType& account, int32_t energy_regen_seconds, bool downvote_energy = false, int64_t new_energy = 0 )
{
}

} } } // xgt::chain::util

FC_REFLECT( xgt::chain::util::energybar,
   (current_energy)
   (last_update_time)
   )
