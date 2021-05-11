#include <xgt/protocol/authority.hpp>

#include <xgt/chain/util/impacted.hpp>

#include <fc/utility.hpp>

namespace xgt { namespace app {

using namespace fc;
using namespace xgt::protocol;

// TODO:  Review all of these, especially no-ops
struct get_impacted_account_visitor
{
   flat_set<wallet_name_type>& _impacted;
   get_impacted_account_visitor( flat_set<wallet_name_type>& impact ):_impacted( impact ) {}
   typedef void result_type;

   template<typename T>
   void operator()( const T& op )
   {
      op.get_required_social_authorities( _impacted );
      op.get_required_money_authorities( _impacted );
      op.get_required_recovery_authorities( _impacted );
   }

   // ops
   void operator()( const wallet_create_operation& op )
   {
      _impacted.insert( op.get_wallet_name() );
      _impacted.insert( op.creator );
   }

   void operator()( const comment_operation& op )
   {
      _impacted.insert( op.author );
      if( op.parent_author.size() )
         _impacted.insert( op.parent_author );
   }

   void operator()( const transfer_operation& op )
   {
      _impacted.insert( op.from );
      _impacted.insert( op.to );
   }

   void operator()( const escrow_transfer_operation& op )
   {
      _impacted.insert( op.from );
      _impacted.insert( op.to );
      _impacted.insert( op.agent );
   }

   void operator()( const escrow_approve_operation& op )
   {
      _impacted.insert( op.from );
      _impacted.insert( op.to );
      _impacted.insert( op.agent );
   }

   void operator()( const escrow_dispute_operation& op )
   {
      _impacted.insert( op.from );
      _impacted.insert( op.to );
      _impacted.insert( op.agent );
   }

   void operator()( const escrow_release_operation& op )
   {
      _impacted.insert( op.from );
      _impacted.insert( op.to );
      _impacted.insert( op.agent );
   }

   struct pow_impacted_visitor
   {
      pow_impacted_visitor(){}

      typedef const wallet_name_type& result_type;

      template< typename WorkType >
      result_type operator()( const WorkType& work )const
      {
         return work.input.worker_account;
      }
   };

   void operator()( const pow_operation& op )
   {
      _impacted.insert( op.work.visit( pow_impacted_visitor() ) );
   }

   void operator()( const request_wallet_recovery_operation& op )
   {
      _impacted.insert( op.account_to_recover );
      _impacted.insert( op.recovery_account );
   }

   void operator()( const recover_wallet_operation& op )
   {
      _impacted.insert( op.account_to_recover );
   }

   void operator()( const change_recovery_wallet_operation& op )
   {
      _impacted.insert( op.account_to_recover );
   }


   // vops

   void operator()( const shutdown_witness_operation& op )
   {
      _impacted.insert( op.recovery );
   }

   void operator()( const hardfork_operation& op )
   {
      _impacted.insert( XGT_INIT_MINER_NAME );
   }

   //void operator()( const operation& op ){}
};

void operation_get_impacted_accounts( const operation& op, flat_set<wallet_name_type>& result )
{
   get_impacted_account_visitor vtor = get_impacted_account_visitor( result );
   op.visit( vtor );
}

void transaction_get_impacted_accounts( const transaction& tx, flat_set<wallet_name_type>& result )
{
   for( const auto& op : tx.operations )
      operation_get_impacted_accounts( op, result );
}

} }
