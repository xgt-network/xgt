#include <xgt/protocol/operations.hpp>

#include <xgt/protocol/operation_util_impl.hpp>

namespace xgt { namespace protocol {

struct is_vop_visitor
{
   typedef bool result_type;

   template< typename T >
   bool operator()( const T& v )const { return v.is_virtual(); }
};

bool is_virtual_operation( const operation& op )
{
   return op.visit( is_vop_visitor() );
}

struct is_wallet_create_visitor
{
   typedef bool result_type;

   template< typename T >
   bool operator()( const T& v )const { return v.is_wallet_create(); }
};

struct is_wallet_update_visitor
{
   typedef bool result_type;

   template< typename T >
   bool operator()( const T& v )const { return v.is_wallet_update(); }
};

struct is_pow_visitor
{
   typedef bool result_type;

   template< typename T >
   bool operator()( const T& v )const { return v.is_pow(); }
};

bool is_wallet_create_operation( const operation& op )
{
   return op.visit( is_wallet_create_visitor() );
}

bool is_wallet_update_operation( const operation& op )
{
   return op.visit( is_wallet_update_visitor() );
}

bool is_pow_operation( const operation& op )
{
   return op.visit( is_pow_visitor() );
}

} } // xgt::protocol

XGT_DEFINE_OPERATION_TYPE( xgt::protocol::operation )
