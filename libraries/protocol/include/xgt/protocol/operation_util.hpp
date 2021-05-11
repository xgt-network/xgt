
#pragma once

#include <xgt/protocol/authority.hpp>

#include <fc/variant.hpp>

#include <boost/container/flat_set.hpp>

#include <string>
#include <vector>

namespace xgt { namespace protocol {

struct get_required_auth_visitor
{
   typedef void result_type;

   flat_set< wallet_name_type >& money;
   flat_set< wallet_name_type >& recovery;
   flat_set< wallet_name_type >& social;
   std::vector< authority >&  other;

   get_required_auth_visitor(
         flat_set< wallet_name_type >& a,
         flat_set< wallet_name_type >& own,
         flat_set< wallet_name_type >& post,
         std::vector< authority >& oth )
      : money( a ), recovery( own ), social( post ), other( oth ) {}

   template< typename ...Ts >
   void operator()( const fc::static_variant< Ts... >& v )
   {
      v.visit( *this );
   }

   template< typename T >
   void operator()( const T& v )const
   {
      v.get_required_money_authorities( money );
      v.get_required_recovery_authorities( recovery );
      v.get_required_social_authorities( social );
      v.get_required_authorities( other );
   }
};

} } // xgt::protocol

//
// Place XGT_DECLARE_OPERATION_TYPE in a .hpp file to declare
// functions related to your operation type
//
#define XGT_DECLARE_OPERATION_TYPE( OperationType )                               \
                                                                                  \
namespace xgt { namespace protocol {                                            \
                                                                                  \
void operation_validate( const OperationType& o );                                \
void operation_get_required_authorities( const OperationType& op,                 \
                                         flat_set< wallet_name_type >& money,    \
                                         flat_set< wallet_name_type >& recovery, \
                                         flat_set< wallet_name_type >& social,   \
                                         vector< authority >& other );            \
                                                                                  \
} } /* xgt::protocol */
