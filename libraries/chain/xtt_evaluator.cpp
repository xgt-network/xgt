#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/chain/xgt_evaluator.hpp>
#include <xgt/chain/database.hpp>
#include <xgt/chain/xgt_objects.hpp>
#include <xgt/chain/xtt_objects.hpp>
#include <xgt/chain/util/xtt_token.hpp>

#include <xgt/protocol/xtt_operations.hpp>
#include <xgt/protocol/xtt_util.hpp>

namespace xgt { namespace chain {

namespace {

/// Return XTT token object controlled by this wallet identified by its symbol. Throws assert exception when not found!
inline const xtt_token_object& get_controlled_xtt( const database& db, const wallet_name_type& control_account, const asset_symbol_type& xtt_symbol )
{
   const xtt_token_object* xtt = db.find< xtt_token_object, by_symbol >( xtt_symbol );
   // The XTT is supposed to be found.
   FC_ASSERT( xtt != nullptr, "XTT numerical asset identifier ${xtt} not found", ("xtt", xtt_symbol.to_nai()) );
   // Check against unotherized wallet trying to access (and alter) XTT. Unotherized means "other than the one that created the XTT".
   FC_ASSERT( xtt->control_account == control_account, "The wallet ${wallet} does NOT control the XTT ${xtt}",
      ("wallet", control_account)("xtt", xtt_symbol.to_nai()) );

   return *xtt;
}

}

namespace {

class xtt_setup_parameters_visitor : public fc::visitor<bool>
{
public:
   xtt_setup_parameters_visitor( xtt_token_object& xtt_token ) : _xtt_token( xtt_token ) {}

   bool operator () ( const xtt_param_allow_voting& allow_voting )
   {
      _xtt_token.allow_voting = allow_voting.value;
      return true;
   }

private:
   xtt_token_object& _xtt_token;
};

const xtt_token_object& common_pre_setup_evaluation(
   const database& _db, const asset_symbol_type& symbol, const wallet_name_type& control_account )
{
   const xtt_token_object& xtt = get_controlled_xtt( _db, control_account, symbol );

   FC_ASSERT( xtt.phase < xtt_phase::setup_completed, "XTT pre-setup operation no longer allowed after setup phase is over" );

   return xtt;
}

} // namespace

void xtt_create_evaluator::do_apply( const xtt_create_operation& o )
{
   const dynamic_global_property_object& dgpo = _db.get_dynamic_global_properties();

   auto token_ptr = util::xtt::find_token( _db, o.symbol, true );

   if ( o.xtt_creation_fee.amount > 0 ) // Creation case
   {
      FC_ASSERT( token_ptr == nullptr, "XTT ${nai} has already been created.", ("nai", o.symbol.to_nai() ) );
      FC_ASSERT( _db.get< nai_pool_object >().contains( o.symbol ), "Cannot create an XTT that didn't come from the NAI pool." );
      FC_ASSERT( o.xtt_creation_fee == dgpo.xtt_creation_fee,
         "Fee of ${ef} does not match the creation fee of ${sf}", ("ef", o.xtt_creation_fee)("sf", dgpo.xtt_creation_fee) );

      _db.adjust_balance( o.control_account , -o.xtt_creation_fee );
      _db.adjust_balance( XGT_NULL_WALLET,  o.xtt_creation_fee );
   }
   else // Reset case
   {
      FC_ASSERT( token_ptr != nullptr, "Cannot reset a non-existent XTT. Did you forget to specify the creation fee?" );
      FC_ASSERT( token_ptr->control_account == o.control_account, "You do not control this XTT. Control Account: ${a}", ("a", token_ptr->control_account) );
      FC_ASSERT( token_ptr->phase == xtt_phase::setup, "XTT cannot be reset if setup is completed. Phase: ${p}", ("p", token_ptr->phase) );

      _db.remove( *token_ptr );
   }

   _db.create< xtt_token_object >( [&]( xtt_token_object& token )
   {
      token.liquid_symbol = o.symbol;
      token.control_account = o.control_account;
      token.desired_ticker = o.desired_ticker;
   });

   remove_from_nai_pool( _db, o.symbol );

   if ( !_db.processor.is_pending_tx() )
      replenish_nai_pool( _db );
}

static void verify_accounts( database& db, const flat_map< unit_target_type, uint16_t >& units )
{
   for ( auto& unit : units )
   {
      if ( !protocol::xtt::unit_target::is_wallet_name_type( unit.first ) )
         continue;

      auto wallet_name = protocol::xtt::unit_target::get_unit_target_account( unit.first );
      const auto* account = db.find_account( wallet_name );
      FC_ASSERT( account != nullptr, "The provided account unit target ${target} does not exist.", ("target", unit.first) );
   }
}

struct xtt_generation_policy_verifier
{
   database& _db;

   xtt_generation_policy_verifier( database& db ): _db( db ){}

   typedef void result_type;

   void operator()( const xtt_capped_generation_policy& capped_generation_policy ) const
   {
      verify_accounts( _db, capped_generation_policy.generation_unit.xgt_unit );
      verify_accounts( _db, capped_generation_policy.generation_unit.token_unit );
   }
};

template< class T >
struct xtt_generation_policy_visitor
{
   T& _obj;

   xtt_generation_policy_visitor( T& o ): _obj( o ) {}

   typedef void result_type;

   void operator()( const xtt_capped_generation_policy& capped_generation_policy ) const
   {
      _obj = capped_generation_policy;
   }
};

void xtt_setup_evaluator::do_apply( const xtt_setup_operation& o )
{
   const xtt_token_object& _token = common_pre_setup_evaluation( _db, o.symbol, o.control_account );

   if ( o.xgt_units_min > 0 )
   {
      auto possible_hard_cap = util::xtt::ico::xgt_units_hard_cap( _db, o.symbol );

      FC_ASSERT( possible_hard_cap.valid(),
         "An XTT with a Xgt Units Minimum of ${s} cannot succeed without an ICO tier.", ("s", o.xgt_units_min) );

      share_type hard_cap = *possible_hard_cap;

      FC_ASSERT( o.xgt_units_min <= hard_cap,
         "The Xgt Units Minimum must be less than the hard cap. Xgt Units Minimum: ${s}, Hard Cap: ${c}",
         ("s", o.xgt_units_min)("c", hard_cap) );
   }

   _db.modify( _token, [&]( xtt_token_object& token )
   {
      token.phase = xtt_phase::setup_completed;
      token.max_supply = o.max_supply;
   } );

   _db.create< xtt_ico_object >( [&] ( xtt_ico_object& token_ico_obj )
   {
      token_ico_obj.symbol = _token.liquid_symbol;
      token_ico_obj.contribution_begin_time = o.contribution_begin_time;
      token_ico_obj.contribution_end_time = o.contribution_end_time;
      token_ico_obj.launch_time = o.launch_time;
      token_ico_obj.xgt_units_min = o.xgt_units_min;
      token_ico_obj.min_unit_ratio = o.min_unit_ratio;
      token_ico_obj.max_unit_ratio = o.max_unit_ratio;
   } );

   xtt_ico_launch_action ico_launch_action;
   ico_launch_action.control_account = _token.control_account;
   ico_launch_action.symbol = _token.liquid_symbol;
   _db.push_required_action( ico_launch_action, o.contribution_begin_time );
}

void xtt_setup_ico_tier_evaluator::do_apply( const xtt_setup_ico_tier_operation& o )
{
   const xtt_token_object& token = common_pre_setup_evaluation( _db, o.symbol, o.control_account );

   xtt_capped_generation_policy generation_policy;
   xtt_generation_policy_visitor< xtt_capped_generation_policy > visitor( generation_policy );
   o.generation_policy.visit( visitor );

   if ( o.remove )
   {
      auto key = boost::make_tuple( token.liquid_symbol, o.xgt_units_cap );
      const auto* ito = _db.find< xtt_ico_tier_object, by_symbol_xgt_units_cap >( key );

      FC_ASSERT( ito != nullptr,
         "The specified ICO tier does not exist. Symbol: ${s}, Xgt Units Cap: ${c}",
         ("s", token.liquid_symbol)("c", o.xgt_units_cap)
      );

      _db.remove( *ito );
   }
   else
   {
      auto num_ico_tiers = util::xtt::ico::ico_tier_size( _db, o.symbol );
      FC_ASSERT( num_ico_tiers < XTT_MAX_ICO_TIERS,
         "There can be a maximum of ${n} ICO tiers. Current: ${c}", ("n", XTT_MAX_ICO_TIERS)("c", num_ico_tiers) );

      xtt_generation_policy_verifier generation_policy_verifier( _db );
      o.generation_policy.visit( generation_policy_verifier );

      _db.create< xtt_ico_tier_object >( [&]( xtt_ico_tier_object& ito )
      {
         ito.symbol                   = token.liquid_symbol;
         ito.xgt_units_cap          = o.xgt_units_cap;
         ito.generation_unit          = generation_policy.generation_unit;
      });
   }
}

void xtt_set_setup_parameters_evaluator::do_apply( const xtt_set_setup_parameters_operation& o )
{
   const xtt_token_object& xtt_token = common_pre_setup_evaluation( _db, o.symbol, o.control_account );

   _db.modify( xtt_token, [&]( xtt_token_object& token )
   {
      xtt_setup_parameters_visitor visitor( token );

      for ( auto& param : o.setup_parameters )
         param.visit( visitor );
   });
}

struct xtt_set_runtime_parameters_evaluator_visitor
{
   xtt_token_object& _token;

   xtt_set_runtime_parameters_evaluator_visitor( xtt_token_object& token ) : _token( token ) {}

   typedef void result_type;

   void operator()( const xtt_param_windows_v1& param_windows )const
   {
      _token.reverse_auction_window_seconds = param_windows.reverse_auction_window_seconds;
   }

   void operator()( const xtt_param_vote_regeneration_period_seconds_v1& vote_regeneration )const
   {
      _token.vote_regeneration_period_seconds = vote_regeneration.vote_regeneration_period_seconds;
      _token.votes_per_regeneration_period = vote_regeneration.votes_per_regeneration_period;
   }

   void operator()( const xtt_param_allow_downvotes& param_allow_downvotes )const
   {
      _token.allow_downvotes = param_allow_downvotes.value;
   }
};

void xtt_set_runtime_parameters_evaluator::do_apply( const xtt_set_runtime_parameters_operation& o )
{
   const xtt_token_object& token = common_pre_setup_evaluation(_db, o.symbol, o.control_account);

   _db.modify( token, [&]( xtt_token_object& t )
   {
      xtt_set_runtime_parameters_evaluator_visitor visitor( t );

      for( auto& param: o.runtime_parameters )
         param.visit( visitor );
   });
}

void xtt_contribute_evaluator::do_apply( const xtt_contribute_operation& o )
{
   try
   {
      const xtt_token_object* token = util::xtt::find_token( _db, o.symbol );
      FC_ASSERT( token != nullptr, "Cannot contribute to an unknown XTT" );
      FC_ASSERT( token->phase >= xtt_phase::ico, "XTT has not begun accepting contributions" );
      FC_ASSERT( token->phase < xtt_phase::ico_completed, "XTT is no longer accepting contributions" );

      auto possible_hard_cap = util::xtt::ico::xgt_units_hard_cap( _db, o.symbol );
      FC_ASSERT( possible_hard_cap.valid(), "The specified token does not feature an ICO" );
      share_type hard_cap = *possible_hard_cap;

      const xtt_ico_object* token_ico = _db.find< xtt_ico_object, by_symbol >( token->liquid_symbol );
      FC_ASSERT( token_ico != nullptr, "Unable to find ICO data for symbol: ${sym}", ("sym", token->liquid_symbol) );
      FC_ASSERT( token_ico->contributed.amount < hard_cap, "XTT ICO has reached its hard cap and no longer accepts contributions" );
      FC_ASSERT( token_ico->contributed.amount + o.contribution.amount <= hard_cap,
         "The proposed contribution would exceed the ICO hard cap, maximum possible contribution: ${c}",
         ("c", asset( hard_cap - token_ico->contributed.amount, XGT_SYMBOL )) );

      auto key = boost::tuple< asset_symbol_type, wallet_name_type, uint32_t >( o.contribution.symbol, o.contributor, o.contribution_id );
      auto contrib_ptr = _db.find< xtt_contribution_object, by_symbol_contributor >( key );
      FC_ASSERT( contrib_ptr == nullptr, "The provided contribution ID must be unique. Current: ${id}", ("id", o.contribution_id) );

      _db.adjust_balance( o.contributor, -o.contribution );

      _db.create< xtt_contribution_object >( [&] ( xtt_contribution_object& obj )
      {
         obj.contributor = o.contributor;
         obj.symbol = o.symbol;
         obj.contribution_id = o.contribution_id;
         obj.contribution = o.contribution;
      } );

      _db.modify( *token_ico, [&]( xtt_ico_object& ico )
      {
         ico.contributed += o.contribution;
      } );
   }
   FC_CAPTURE_AND_RETHROW( (o) )
}

} }
