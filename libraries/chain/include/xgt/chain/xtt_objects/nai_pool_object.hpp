#pragma once

#include <xgt/chain/xgt_fwd.hpp>
#include <xgt/chain/xgt_object_types.hpp>
#include <xgt/protocol/asset_symbol.hpp>

namespace xgt { namespace chain {

   class nai_pool_object : public object< nai_pool_object_type, nai_pool_object >
   {
      public:
      nai_pool_object() = default;;

   public:
      using pool_type = fc::array< asset_symbol_type, XTT_MAX_NAI_POOL_COUNT >;

      id_type id;

      uint8_t num_available_nais    = 0;
      uint32_t attempts_per_block   = 0;
      uint32_t collisions_per_block = 0;
      block_id_type last_block_id   = block_id_type();

      pool_type nais;

      std::vector< asset_symbol_type > pool() const
      {
         return std::vector< asset_symbol_type >{ nais.begin(), nais.end() - (XTT_MAX_NAI_POOL_COUNT - num_available_nais) };
      }

      bool contains( const asset_symbol_type& a ) const
      {
         const auto end = nais.end() - (XTT_MAX_NAI_POOL_COUNT - num_available_nais);
         return std::find( nais.begin(), end, asset_symbol_type::from_asset_num( a.get_stripped_precision_xtt_num() ) ) != end;
      }
   };

   typedef multi_index_container <
      nai_pool_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< nai_pool_object, nai_pool_id_type, &nai_pool_object::id > >
      >
   > nai_pool_index;

} } // namespace xgt::chain

FC_REFLECT( xgt::chain::nai_pool_object, (id)(num_available_nais)(attempts_per_block)(collisions_per_block)(last_block_id)(nais) )

CHAINBASE_SET_INDEX_TYPE( xgt::chain::nai_pool_object, xgt::chain::nai_pool_index )
