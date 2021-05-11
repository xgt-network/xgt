#include <xgt/chain/xgt_fwd.hpp>
#include <xgt/chain/util/xtt_token.hpp>
#include <xgt/chain/xgt_object_types.hpp>
#include <xgt/protocol/xtt_util.hpp>

namespace xgt { namespace chain { namespace util { namespace xtt {

const xtt_token_object* find_token( const database& db, uint32_t nai )
{
   const auto& idx = db.get_index< xtt_token_index >().indices().get< by_symbol >();

   auto itr = idx.lower_bound( asset_symbol_type::from_nai( nai, 0 ) );
   for (; itr != idx.end(); ++itr )
   {
      if (itr->liquid_symbol.to_nai() != nai )
         break;
      return &*itr;
   }

   return nullptr;
}

const xtt_token_object* find_token( const database& db, asset_symbol_type symbol, bool precision_agnostic )
{
   if ( precision_agnostic )
      return find_token( db, symbol.to_nai() );
   else
      return db.find< xtt_token_object, by_symbol >( symbol );
}

namespace ico {

bool schedule_next_refund( database& db, const asset_symbol_type& a )
{
   bool action_scheduled = false;
   auto& idx = db.get_index< xtt_contribution_index, by_symbol_id >();
   auto itr = idx.lower_bound( a );

   if ( itr != idx.end() && itr->symbol == a )
   {
      xtt_refund_action refund_action;
      refund_action.symbol = itr->symbol;
      refund_action.contributor = itr->contributor;
      refund_action.contribution_id = itr->contribution_id;
      refund_action.refund = itr->contribution;

      db.push_required_action( refund_action );
      action_scheduled = true;
   }

   return action_scheduled;
}

fc::optional< share_type > xgt_units_hard_cap( database& db, const asset_symbol_type& a )
{
   const auto& idx = db.get_index< xtt_ico_tier_index, by_symbol_xgt_units_cap >();

   const auto range = idx.equal_range( a );

   auto itr = range.second;
   while ( itr != range.first )
   {
      --itr;
      return itr->xgt_units_cap;
   }

   return {};
}

void remove_ico_objects( database& db, const asset_symbol_type& symbol )
{
   db.remove( db.get< xtt_ico_object, by_symbol >( symbol ) );

   const auto& ico_tier_idx = db.get_index< xtt_ico_tier_index, by_symbol_xgt_units_cap >();
   auto itr = ico_tier_idx.lower_bound( symbol );
   while( itr != ico_tier_idx.end() && itr->symbol == symbol )
   {
      const auto& current = *itr;
      ++itr;
      db.remove( current );
   }
}

// If we increase the value of XTT_MAX_ICO_TIERS, we should track the size in the ICO object
std::size_t ico_tier_size( database& db, const asset_symbol_type& symbol )
{
   std::size_t num_ico_tiers = 0;

   const auto& ico_tier_idx = db.get_index< xtt_ico_tier_index, by_symbol_xgt_units_cap >();
   auto ico_tier_itr = ico_tier_idx.lower_bound( symbol );
   while ( ico_tier_itr != ico_tier_idx.end() && ico_tier_itr->symbol == symbol )
   {
      num_ico_tiers++;
      ++ico_tier_itr;
   }

   return num_ico_tiers;
}

} // xgt::chain::util::xtt::ico

} } } } // xgt::chain::util::xtt

