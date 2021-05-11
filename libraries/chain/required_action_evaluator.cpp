#include <xgt/chain/xgt_fwd.hpp>
#include <xgt/chain/required_action_evaluator.hpp>
#include <xgt/chain/database.hpp>
#include <xgt/chain/xtt_objects.hpp>
#include <xgt/chain/util/xtt_token.hpp>
#include <cmath>

namespace xgt { namespace chain {

void example_required_evaluator::do_apply( const example_required_action& a ) {}

void xtt_ico_launch_evaluator::do_apply( const xtt_ico_launch_action& a )
{
   const xtt_token_object& token = _db.get< xtt_token_object, by_symbol >( a.symbol );
   const xtt_ico_object& ico = _db.get< xtt_ico_object, by_symbol >( token.liquid_symbol );

   _db.modify( token, []( xtt_token_object& o )
   {
      o.phase = xtt_phase::ico;
   } );

   xtt_ico_evaluation_action eval_action;
   eval_action.control_account = token.control_account;
   eval_action.symbol = token.liquid_symbol;
   _db.push_required_action( eval_action, ico.contribution_end_time );
}

void xtt_ico_evaluation_evaluator::do_apply( const xtt_ico_evaluation_action& a )
{
   const xtt_token_object& token = _db.get< xtt_token_object, by_symbol >( a.symbol );
   const xtt_ico_object& ico = _db.get< xtt_ico_object, by_symbol >( token.liquid_symbol );

   if ( ico.contributed.amount >= ico.xgt_units_min )
   {
      _db.modify( token, []( xtt_token_object& o )
      {
         o.phase = xtt_phase::ico_completed;
      } );

      xtt_token_launch_action launch_action;
      launch_action.control_account = token.control_account;
      launch_action.symbol = token.liquid_symbol;
      _db.push_required_action( launch_action, ico.launch_time );
   }
   else
   {
      _db.modify( token, []( xtt_token_object& o )
      {
         o.phase = xtt_phase::launch_failed;
      } );

      //if ( !util::xtt::ico::schedule_next_refund( _db, token.liquid_symbol ) )
      //   _db.remove( ico );
   }
}

void xtt_token_launch_evaluator::do_apply( const xtt_token_launch_action& a )
{
   const xtt_token_object& token = _db.get< xtt_token_object, by_symbol >( a.symbol );

   _db.modify( token, []( xtt_token_object& o )
   {
      o.phase = xtt_phase::launch_success;
   } );

  // if ( !util::xtt::ico::schedule_next_contributor_payout( _db, token.liquid_symbol ) )
  //    util::xtt::ico::remove_ico_objects( _db, token.liquid_symbol );
}

void xtt_refund_evaluator::do_apply( const xtt_refund_action& a )
{
}

} } //xgt::chain
