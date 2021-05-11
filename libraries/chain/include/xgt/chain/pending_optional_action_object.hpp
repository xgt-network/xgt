#pragma once

#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/protocol/required_automated_actions.hpp>

#include <xgt/chain/xgt_object_types.hpp>

namespace xgt { namespace chain {

using xgt::protocol::optional_automated_action;

class pending_optional_action_object : public object< pending_optional_action_object_type, pending_optional_action_object >
{
   XGT_STD_ALLOCATOR_CONSTRUCTOR( pending_optional_action_object )

   public:
      template< typename Constructor, typename Allocator >
      pending_optional_action_object( Constructor&& c, allocator< Allocator > a )
      {
         c( *this );
      }

      id_type                    id;

      time_point_sec             execution_time;
      fc::sha256                 action_hash;
      optional_automated_action  action;
};

struct by_hash;

typedef multi_index_container<
   pending_optional_action_object,
   indexed_by<
      ordered_unique< tag< by_id >, member< pending_optional_action_object, pending_optional_action_id_type, &pending_optional_action_object::id > >,
      ordered_unique< tag< by_execution >,
         composite_key< pending_optional_action_object,
            member< pending_optional_action_object, time_point_sec, &pending_optional_action_object::execution_time >,
            member< pending_optional_action_object, pending_optional_action_id_type, &pending_optional_action_object::id >
         >
      >,
      ordered_unique< tag< by_hash >,
         composite_key< pending_optional_action_object,
            member< pending_optional_action_object, fc::sha256, &pending_optional_action_object::action_hash >,
            member< pending_optional_action_object, pending_optional_action_id_type, &pending_optional_action_object::id >
         >
      >
   >,
   allocator< pending_optional_action_object >
> pending_optional_action_index;

} } //xgt::chain

FC_REFLECT( xgt::chain::pending_optional_action_object,
            (id)(execution_time)(action_hash)(action) )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::pending_optional_action_object, xgt::chain::pending_optional_action_index )
