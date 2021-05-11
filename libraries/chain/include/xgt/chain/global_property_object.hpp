#pragma once

#include <xgt/chain/xgt_fwd.hpp>

#include <fc/uint128.hpp>

#include <xgt/chain/xgt_object_types.hpp>

#include <xgt/protocol/asset.hpp>

namespace xgt { namespace chain {

   using xgt::protocol::asset;
   using xgt::protocol::price;

   /**
    * @class dynamic_global_property_object
    * @brief Maintains global state information
    * @ingroup object
    * @ingroup implementation
    *
    * This is an implementation detail. The values here are calculated during normal chain operations and reflect the
    * current values of global blockchain properties.
    */
   class dynamic_global_property_object : public object< dynamic_global_property_object_type, dynamic_global_property_object>
   {
      public:
         template< typename Constructor, typename Allocator >
         dynamic_global_property_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         dynamic_global_property_object(){}

         id_type           id;

         uint32_t          head_block_number = 0;
         block_id_type     head_block_id;
         time_point_sec    time;
         wallet_name_type  current_witness;

         fc::sha256 mining_target;
         time_point_sec last_mining_recalc_time;

         /**
          *  The total POW accumulated, aka the sum of num_pow_witness at the time new POW is added
          */
         uint64_t total_pow = -1;

         /**
          * The current count of how many pending POW witnesses there are, determines the difficulty
          * of doing pow
          */
         uint32_t num_pow_witnesses = 0;

         asset       virtual_supply             = asset( 0, XGT_SYMBOL );
         asset       current_supply             = asset( 0, XGT_SYMBOL );
         asset       confidential_supply        = asset( 0, XGT_SYMBOL ); ///< total asset held in confidential balances

         /**
          *  Maximum block size is decided by the set of active witnesses which change every round.
          *  Each witness posts what they think the maximum size should be as part of their witness
          *  properties, the median size is chosen to be the maximum block size for the round.
          *
          *  @note the minimum value for maximum_block_size is defined by the protocol to prevent the
          *  network from getting stuck by witnesses attempting to set this too low.
          */
         uint32_t     maximum_block_size = 0;

         /**
          * The size of the block that is partitioned for actions.
          * Required actions can only be delayed if they take up more than this amount. More can be
          * included, but are not required. Block generation should only include transactions up
          * to maximum_block_size - required_actions_parition_size to ensure required actions are
          * not delayed when they should not be.
          */
         uint16_t     required_actions_partition_percent = 0;

         /**
          * The current absolute slot number.  Equal to the total
          * number of slots since genesis.  Also equal to the total
          * number of missed slots plus head_block_number.
          */
         uint64_t      current_aslot = 0;

         /**
          * used to compute witness participation.
          */
         fc::uint128_t recent_slots_filled;
         uint8_t       participation_count = 0; ///< Divide by 128 to compute participation percentage

         uint32_t last_irreversible_block_num = 0;

         /**
          * The number of votes regenerated per day.  Any user voting slower than this rate will be
          * "wasting" voting power through spillover; any user voting faster than this rate will have
          * their votes reduced.
          */
         uint32_t target_votes_per_period = XGT_INITIAL_VOTE_POWER_RATE;

         uint32_t delegation_return_period = XGT_VOTING_ENERGY_REGENERATION_SECONDS;

         uint64_t reverse_auction_seconds = 0;

         //settings used to compute payments for every proposal
         time_point_sec next_maintenance_time;
         time_point_sec last_budget_time;

         uint16_t downvote_pool_percent = 0;

         asset xtt_creation_fee = asset( 1000, XGT_SYMBOL );
   };

   typedef multi_index_container<
      dynamic_global_property_object,
      indexed_by<
         ordered_unique< tag< by_id >,
            member< dynamic_global_property_object, dynamic_global_property_object::id_type, &dynamic_global_property_object::id > >
      >,
      allocator< dynamic_global_property_object >
   > dynamic_global_property_index;

} } // xgt::chain

#ifdef ENABLE_MIRA
namespace mira {

template<> struct is_static_length< xgt::chain::dynamic_global_property_object > : public boost::true_type {};

} // mira
#endif

FC_REFLECT( xgt::chain::dynamic_global_property_object,
             (id)
             (head_block_number)
             (head_block_id)
             (time)
             (current_witness)
             (total_pow)
             (num_pow_witnesses)
             (virtual_supply)
             (current_supply)
             (confidential_supply)
             (maximum_block_size)
             (required_actions_partition_percent)
             (current_aslot)
             (recent_slots_filled)
             (participation_count)
             (last_irreversible_block_num)
             (target_votes_per_period)
             (delegation_return_period)
             (reverse_auction_seconds)
             (next_maintenance_time)
             (last_budget_time)
             (downvote_pool_percent)
             (xtt_creation_fee)
          )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::dynamic_global_property_object, xgt::chain::dynamic_global_property_index )
