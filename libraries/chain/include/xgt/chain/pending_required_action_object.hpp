#pragma once
#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/protocol/required_automated_actions.hpp>

#include <xgt/chain/xgt_object_types.hpp>

namespace xgt { namespace chain {

using xgt::protocol::required_automated_action;

class pending_required_action_object : public object< pending_required_action_object_type, pending_required_action_object >
{
   XGT_STD_ALLOCATOR_CONSTRUCTOR( pending_required_action_object )

   public:
      id_type                    id;

      time_point_sec             execution_time;
      required_automated_action  action;
};

struct by_execution;

typedef multi_index_container<
   pending_required_action_object,
   indexed_by<
      ordered_unique< tag< by_id >, member< pending_required_action_object, pending_required_action_id_type, &pending_required_action_object::id > >,
      ordered_unique< tag< by_execution >,
         composite_key< pending_required_action_object,
            member< pending_required_action_object, time_point_sec, &pending_required_action_object::execution_time >,
            member< pending_required_action_object, pending_required_action_id_type, &pending_required_action_object::id >
         >
      >
   >
> pending_required_action_index;

} } //xgt::chain

FC_REFLECT( xgt::chain::pending_required_action_object,
            (id)(execution_time)(action) )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::pending_required_action_object, xgt::chain::pending_required_action_index )
