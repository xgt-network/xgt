#pragma once

#include <xgt/chain/xgt_object_types.hpp>
#include <xgt/chain/util/energybar.hpp>

#include <xgt/protocol/xtt_operations.hpp>

namespace xgt { namespace chain {

/**
 * Class responsible for holding regular (i.e. non-reward) balance of XTT for given account.
 * It has not been unified with reward balance object counterpart, due to different number
 * of fields needed to hold balances (2 for regular, 3 for reward).
 */
class account_regular_balance_object : public object< account_regular_balance_object_type, account_regular_balance_object >
{
   XGT_STD_ALLOCATOR_CONSTRUCTOR( account_regular_balance_object );

public:
   template <typename Constructor, typename Allocator>
   account_regular_balance_object(Constructor&& c, allocator< Allocator > a)
   {
      c( *this );
   }

   id_type             id;
   wallet_name_type    name;
   asset               liquid;

   share_type          withdrawn               = 0;
   share_type          to_withdraw             = 0;

   util::energybar     energybar;

   fc::time_point_sec  last_vote_time;

   asset_symbol_type get_liquid_symbol() const
   {
      return liquid.symbol;
   }

   asset_symbol_type get_stripped_symbol() const
   {
      return asset_symbol_type::from_asset_num( liquid.symbol.get_stripped_precision_xtt_num() );
   }

   void initialize_assets( asset_symbol_type liquid_symbol )
   {
      liquid                   = asset( 0, liquid_symbol );
   }

   bool validate() const
   {
      return true;
   }
};


struct by_name_liquid_symbol;

typedef multi_index_container <
   account_regular_balance_object,
   indexed_by <
      ordered_unique< tag< by_id >,
         member< account_regular_balance_object, account_regular_balance_id_type, &account_regular_balance_object::id >
      >,
      ordered_unique< tag< by_name_liquid_symbol >,
         composite_key< account_regular_balance_object,
            member< account_regular_balance_object, wallet_name_type, &account_regular_balance_object::name >,
            const_mem_fun< account_regular_balance_object, asset_symbol_type, &account_regular_balance_object::get_liquid_symbol >
         >
      >
   >,
   allocator< account_regular_balance_object >
> account_regular_balance_index;

} } // namespace xgt::chain

FC_REFLECT( xgt::chain::account_regular_balance_object,
   (id)
   (name)
   (liquid)
   (withdrawn)
   (to_withdraw)
   (energybar)
   (last_vote_time)
)

CHAINBASE_SET_INDEX_TYPE( xgt::chain::account_regular_balance_object, xgt::chain::account_regular_balance_index )
