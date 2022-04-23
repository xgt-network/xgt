#pragma once

#include <xgt/chain/xgt_object_types.hpp>

#include <xgt/protocol/authority.hpp>

#include <chainbase/chainbase.hpp>

namespace xgt { namespace chain {
   using xgt::protocol::authority;
   using xgt::protocol::public_key_type;
   using xgt::protocol::wallet_name_type;
   using xgt::protocol::weight_type;

   /**
    *  The purpose of this class is to represent an authority object.
    */
   struct shared_authority
   {
      public:
      shared_authority() = default;

      public:

      shared_authority( const authority& a ) :
         wallet_auths(),
         key_auths()
      {
         wallet_auths.reserve( a.wallet_auths.size() );
         key_auths.reserve( a.key_auths.size() );
         for( const auto& item : a.wallet_auths )
            wallet_auths.insert( item );
         for( const auto& item : a.key_auths )
            key_auths.insert( item );
         weight_threshold = a.weight_threshold;
      }

      shared_authority( const shared_authority& cpy ) :
         weight_threshold( cpy.weight_threshold ),
         wallet_auths( cpy.wallet_auths ), key_auths( cpy.key_auths ) {}

      template< class ...Args >
      shared_authority( uint32_t weight_threshold, Args... auths ) :
         weight_threshold( weight_threshold ),
         wallet_auths( ),
         key_auths(  )
      {
         add_authorities( auths... );
      }

      operator authority()const;

      shared_authority& operator=( const authority& a );

      void add_authority( const public_key_type& k, weight_type w );
      void add_authority( const wallet_name_type& k, weight_type w );

      template<typename AuthType>
      void add_authorities(AuthType k, weight_type w)
      {
         add_authority(k, w);
      }

      template<typename AuthType, class ...Args>
      void add_authorities(AuthType k, weight_type w, Args... auths)
      {
         add_authority(k, w);
         add_authorities(auths...);
      }

      vector<public_key_type> get_keys()const;

      bool     is_impossible()const;
      uint32_t num_auths()const;
      void     clear();
      void     validate()const;

      typedef boost::container::flat_map< wallet_name_type, weight_type> account_authority_map;
      typedef boost::container::flat_map< public_key_type, weight_type> key_authority_map;

      uint32_t                                                                weight_threshold = 0;
      account_authority_map                                                   wallet_auths;
      key_authority_map                                                       key_auths;
   };

   bool operator == ( const shared_authority& a, const shared_authority& b );
   bool operator == ( const authority& a, const shared_authority& b );
   bool operator == ( const shared_authority& a, const authority& b );

} } //xgt::chain

//FC_REFLECT_TYPENAME( xgt::chain::shared_authority::account_authority_map)
FC_REFLECT( xgt::chain::shared_authority, (weight_threshold)(wallet_auths)(key_auths) )
