#include <xgt/protocol/xtt_operations.hpp>
#include <xgt/protocol/validation.hpp>
#include <xgt/protocol/xtt_util.hpp>

namespace xgt { namespace protocol {

template < class Operation >
void xtt_admin_operation_validate( const Operation& o )
{
   validate_wallet_name( o.control_account );
   validate_xtt_symbol( o.symbol );
}

void xtt_create_operation::validate()const
{
   xtt_admin_operation_validate( *this );

   if( desired_ticker != xtt_ticker_type() )
   {
      FC_ASSERT( desired_ticker != XGT_SYMBOL_STR, "XTT Ticker Symbol cannot be ${s}", ("s", XGT_SYMBOL_STR) );

      fc::string ticker_str = desired_ticker;

      FC_ASSERT( ticker_str.length() >= 3, "XTT Ticker Symbol must be at least 3 characters long." );
      for( size_t i = 0; i < ticker_str.length(); i++ )
      {
         FC_ASSERT( ticker_str[i] >= 'A' && ticker_str[i] <= 'Z', "XTT Ticker Symbol can only contain capital letters." );
      }
   }

   FC_ASSERT( xtt_creation_fee.amount >= 0, "fee cannot be negative" );
   FC_ASSERT( xtt_creation_fee.amount <= XGT_MAX_SHARE_SUPPLY, "Fee must be smaller than XGT_MAX_SHARE_SUPPLY" );
   FC_ASSERT( is_asset_type( xtt_creation_fee, XGT_SYMBOL ), "Fee must be XGT" );
   FC_ASSERT( symbol.decimals() == precision, "Mismatch between redundantly provided precision ${prec1} vs ${prec2}",
      ("prec1",symbol.decimals())("prec2",precision) );
}

bool is_valid_unit_target( const unit_target_type& unit_target )
{
   if ( is_valid_wallet_name( unit_target ) )
      return true;
   if ( xtt::unit_target::is_contributor( unit_target ) )
      return true;
   return false;
}

bool is_valid_xtt_ico_xgt_destination( const unit_target_type& unit_target )
{
   if ( is_valid_wallet_name( unit_target ) )
      return true;
   return false;
}

bool is_valid_xtt_ico_token_destination( const unit_target_type& unit_target )
{
   if ( is_valid_wallet_name( unit_target ) )
      return true;
   if ( xtt::unit_target::is_contributor( unit_target ) )
      return true;
   return false;
}

uint32_t xtt_generation_unit::xgt_unit_sum()const
{
   uint32_t result = 0;
   for(const std::pair< unit_target_type, uint16_t >& e : xgt_unit )
      result += e.second;
   return result;
}

uint32_t xtt_generation_unit::token_unit_sum()const
{
   uint32_t result = 0;
   for(const std::pair< unit_target_type, uint16_t >& e : token_unit )
      result += e.second;
   return result;
}

void xtt_generation_unit::validate()const
{
   FC_ASSERT( xgt_unit.size() <= XTT_MAX_UNIT_ROUTES );
   for(const std::pair< unit_target_type, uint16_t >& e : xgt_unit )
   {
      FC_ASSERT( is_valid_unit_target( e.first ) );
      FC_ASSERT( e.second > 0 );
   }
   FC_ASSERT( token_unit.size() <= XTT_MAX_UNIT_ROUTES );
   for(const std::pair< unit_target_type, uint16_t >& e : token_unit )
   {
      FC_ASSERT( is_valid_unit_target( e.first ) );
      FC_ASSERT( e.second > 0 );
   }
}

void xtt_capped_generation_policy::validate()const
{
   generation_unit.validate();

   FC_ASSERT( generation_unit.xgt_unit.size() > 0 );
   FC_ASSERT( generation_unit.token_unit.size() >= 0 );
   FC_ASSERT( generation_unit.xgt_unit.size() <= XTT_MAX_UNIT_COUNT );
   FC_ASSERT( generation_unit.token_unit.size() <= XTT_MAX_UNIT_COUNT );

   for ( auto& unit : generation_unit.xgt_unit )
      FC_ASSERT( is_valid_xtt_ico_xgt_destination( unit.first ),
         "${unit_target} is not a valid XGT unit target.", ("unit_target", unit.first) );

   for ( auto& unit : generation_unit.token_unit )
      FC_ASSERT( is_valid_xtt_ico_token_destination( unit.first ),
         "${unit_target} is not a valid token unit target.", ("unit_target", unit.first) );
}

struct validate_visitor
{
   typedef void result_type;

   template< typename T >
   void operator()( const T& x )
   {
      x.validate();
   }
};

void xtt_setup_ico_tier_operation::validate()const
{
   xtt_admin_operation_validate( *this );

   FC_ASSERT( xgt_units_cap >= 0, "Xgt units cap must be greater than or equal to 0" );
   FC_ASSERT( xgt_units_cap <= XGT_MAX_SHARE_SUPPLY, "Xgt units cap must be less than or equal to ${n}", ("n", XGT_MAX_SHARE_SUPPLY) );

   validate_visitor vtor;
   generation_policy.visit( vtor );
}

void xtt_setup_operation::validate()const
{
   xtt_admin_operation_validate( *this );

   FC_ASSERT( max_supply > 0, "Max supply must be greater than 0" );
   FC_ASSERT( max_supply <= XGT_MAX_SHARE_SUPPLY, "Max supply must be less than ${n}", ("n", XGT_MAX_SHARE_SUPPLY) );
   FC_ASSERT( contribution_begin_time > XGT_GENESIS_TIME, "Contribution begin time must be greater than ${t}", ("t", XGT_GENESIS_TIME) );
   FC_ASSERT( contribution_end_time >= contribution_begin_time, "Contribution end time must be equal to or later than contribution begin time" );
   FC_ASSERT( launch_time >= contribution_end_time, "Launch time must be equal to or later than the contribution end time" );
   FC_ASSERT( xgt_units_min >= 0, "Xgt units min must be greater than or equal to 0" );
}

struct xtt_set_runtime_parameters_operation_visitor
{
   xtt_set_runtime_parameters_operation_visitor(){}

   typedef void result_type;

   void operator()( const xtt_param_windows_v1& param_windows )const
   {
   }

   void operator()( const xtt_param_vote_regeneration_period_seconds_v1& vote_regeneration )const
   {
      // With previous assertion, this value will not overflow a 32 bit integer
      // This calculation is designed to round up
      uint32_t nominal_votes_per_day = ( vote_regeneration.votes_per_regeneration_period * 86400 + vote_regeneration.vote_regeneration_period_seconds - 1 )
         / vote_regeneration.vote_regeneration_period_seconds;

      FC_ASSERT( nominal_votes_per_day <= XTT_MAX_NOMINAL_VOTES_PER_DAY,
         "Nominal votes per day exceeds maximum (${max}). Was ${v}",
         ("max", XTT_MAX_NOMINAL_VOTES_PER_DAY)
         ("v", nominal_votes_per_day) );
   }

   void operator()( const xtt_param_allow_downvotes& )const
   {
      //Nothing to do
   }
};

void xtt_set_runtime_parameters_operation::validate()const
{
   xtt_admin_operation_validate( *this );
   FC_ASSERT( !runtime_parameters.empty() );

   xtt_set_runtime_parameters_operation_visitor visitor;
   for( auto& param: runtime_parameters )
      param.visit( visitor );
}

void xtt_set_setup_parameters_operation::validate() const
{
   xtt_admin_operation_validate( *this );
   FC_ASSERT( setup_parameters.empty() == false );
}

void xtt_contribute_operation::validate() const
{
   validate_wallet_name( contributor );
   validate_xtt_symbol( symbol );
   FC_ASSERT( contribution.symbol == XGT_SYMBOL, "Contributions must be made in XGT" );
   FC_ASSERT( contribution.amount > 0, "Contribution amount must be greater than 0" );
}

} }
