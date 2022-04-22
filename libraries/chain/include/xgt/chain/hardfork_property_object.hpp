#pragma once

#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/protocol/version.hpp>

#include <xgt/chain/xgt_object_types.hpp>

namespace xgt { namespace chain {

   class hardfork_property_object : public object< hardfork_property_object_type, hardfork_property_object >
   {
      XGT_STD_ALLOCATOR_CONSTRUCTOR( hardfork_property_object )

      public:
         id_type                                                              id;

         using t_processed_hardforks = boost::container::vector< fc::time_point_sec >;

         t_processed_hardforks                                                processed_hardforks;
         uint32_t                                                             last_hardfork = 0;
         protocol::hardfork_version                                           current_hardfork_version;
         protocol::hardfork_version                                           next_hardfork;
         fc::time_point_sec                                                   next_hardfork_time;
   };

   typedef multi_index_container<
      hardfork_property_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< hardfork_property_object, hardfork_property_object::id_type, &hardfork_property_object::id > >
      >
   > hardfork_property_index;

} } // xgt::chain

FC_REFLECT( xgt::chain::hardfork_property_object,
   (id)(processed_hardforks)(last_hardfork)(current_hardfork_version)
   (next_hardfork)(next_hardfork_time) )
CHAINBASE_SET_INDEX_TYPE( xgt::chain::hardfork_property_object, xgt::chain::hardfork_property_index )
