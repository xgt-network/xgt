#include <fc/macros.hpp>

#if defined IS_TEST_NET

FC_TODO(Extend testing scenarios to support multiple NAIs per account)

#include <boost/test/unit_test.hpp>

#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/protocol/exceptions.hpp>
#include <xgt/protocol/hardfork.hpp>
#include <xgt/protocol/xtt_util.hpp>

#include <xgt/chain/database.hpp>
#include <xgt/chain/database_exceptions.hpp>
#include <xgt/chain/xgt_objects.hpp>
#include <xgt/chain/xtt_objects.hpp>

#include <xgt/chain/util/xtt_token.hpp>

#include "../db_fixture/database_fixture.hpp"

using namespace xgt::chain;
using namespace xgt::protocol;
using fc::string;
using boost::container::flat_set;
using boost::container::flat_map;

BOOST_FIXTURE_TEST_SUITE( xtt_tests, clean_database_fixture )
/*
BOOST_AUTO_TEST_CASE( xtt_transfer_validate )
{
   try
   {
      ACTORS( (alice) )

      generate_block();

      asset_symbol_type alice_symbol = create_xtt("alice", alice_private_key, 0);

      transfer_operation op;
      op.from = "alice";
      op.to = "bob";
      op.amount = asset(100, alice_symbol);
      op.validate();

      validate_database();
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( xtt_transfer_apply )
{
   // This simple test touches XTT account balance objects, related functions (get/adjust)
   // and transfer operation that builds on them.
   try
   {
      ACTORS( (alice)(bob) )

      generate_block();

      // Create XTT.
      asset_symbol_type alice_symbol = create_xtt("alice", alice_private_key, 0);
      asset_symbol_type bob_symbol = create_xtt("bob", bob_private_key, 1);

      // Give some XTT to creators.
      FUND( "alice", asset( 100, alice_symbol ) );
      FUND( "bob", asset( 110, bob_symbol ) );

      // Check pre-tranfer amounts.
      FC_ASSERT( db->get_balance( "alice", alice_symbol ).amount == 100, "XTT balance adjusting error" );
      FC_ASSERT( db->get_balance( "alice", bob_symbol ).amount == 0, "XTT balance adjusting error" );
      FC_ASSERT( db->get_balance( "bob", alice_symbol ).amount == 0, "XTT balance adjusting error" );
      FC_ASSERT( db->get_balance( "bob", bob_symbol ).amount == 110, "XTT balance adjusting error" );

      // Transfer XTT.
      transfer( "alice", "bob", asset(20, alice_symbol) );
      transfer( "bob", "alice", asset(50, bob_symbol) );

      // Check transfer outcome.
      FC_ASSERT( db->get_balance( "alice", alice_symbol ).amount == 80, "XTT transfer error" );
      FC_ASSERT( db->get_balance( "alice", bob_symbol ).amount == 50, "XTT transfer error" );
      FC_ASSERT( db->get_balance( "bob", alice_symbol ).amount == 20, "XTT transfer error" );
      FC_ASSERT( db->get_balance( "bob", bob_symbol ).amount == 60, "XTT transfer error" );

      validate_database();
   }
   FC_LOG_AND_RETHROW()
}*/
/*
BOOST_AUTO_TEST_CASE( asset_symbol_vesting_methods )
{
   try
   {
      BOOST_TEST_MESSAGE( "Test asset_symbol vesting methods" );

      asset_symbol_type Xgt = XGT_SYMBOL;
      FC_ASSERT( Xgt.is_vesting() == false );
      FC_ASSERT( Xgt.get_paired_symbol() == VESTS_SYMBOL );

      asset_symbol_type Vests = VESTS_SYMBOL;
      FC_ASSERT( Vests.is_vesting() );
      FC_ASSERT( Vests.get_paired_symbol() == XGT_SYMBOL );

      asset_symbol_type Sbd = SBD_SYMBOL;
      FC_ASSERT( Sbd.is_vesting() == false );
      FC_ASSERT( Sbd.get_paired_symbol() == SBD_SYMBOL );

      ACTORS( (alice) )
      generate_block();
      auto xtts = create_xtt_3("alice", alice_private_key);
      {
         for( const asset_symbol_type& liquid_xtt : xtts )
         {
            FC_ASSERT( liquid_xtt.is_vesting() == false );
            auto vesting_xtt = liquid_xtt.get_paired_symbol();
            FC_ASSERT( vesting_xtt != liquid_xtt );
            FC_ASSERT( vesting_xtt.is_vesting() );
            FC_ASSERT( vesting_xtt.get_paired_symbol() == liquid_xtt );
         }
      }
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( vesting_xtt_creation )
{
   try
   {
      BOOST_TEST_MESSAGE( "Test Creation of vesting XTT" );

      ACTORS((alice));
      generate_block();

      asset_symbol_type liquid_symbol = create_xtt("alice", alice_private_key, 6);
      // Use liquid symbol/NAI to confirm xtt object was created.
      auto liquid_object_by_symbol = util::xtt::find_token( *db, liquid_symbol );
      FC_ASSERT( liquid_object_by_symbol != nullptr );

      asset_symbol_type vesting_symbol = liquid_symbol.get_paired_symbol();
      // Use vesting symbol/NAI to confirm xtt object was created.
      auto vesting_object_by_symbol = util::xtt::find_token( *db, vesting_symbol );
      FC_ASSERT( vesting_object_by_symbol != nullptr );

      // Check that liquid and vesting objects are the same one.
      FC_ASSERT( liquid_object_by_symbol == vesting_object_by_symbol );
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( xtt_founder_vesting )
{
   using namespace xgt::protocol;
   BOOST_TEST_MESSAGE( "Testing: is_founder_vesting and get_unit_target_account" );

   BOOST_TEST_MESSAGE( " -- Valid founder vesting" );
   BOOST_REQUIRE( xtt::unit_target::is_founder_vesting( "$!alice.vesting" ) );

   BOOST_TEST_MESSAGE( " -- Account name parsing" );
   BOOST_REQUIRE( xtt::unit_target::get_unit_target_account( "$!alice.vesting" ) == wallet_name_type( "alice" ) );

   BOOST_TEST_MESSAGE( " -- No possible room for an account name" );
   BOOST_REQUIRE( xtt::unit_target::is_founder_vesting( "$!.vesting" ) == false );

   BOOST_TEST_MESSAGE( " -- Meant to be founder vesting" );
   BOOST_REQUIRE( xtt::unit_target::is_founder_vesting( "$!@.vesting" ) );

   BOOST_TEST_MESSAGE( " -- Invalid account name upon retrieval" );
   BOOST_REQUIRE_THROW( xtt::unit_target::get_unit_target_account( "$!@.vesting" ), fc::assert_exception );

   BOOST_TEST_MESSAGE( " -- XTT special destinations" );
   BOOST_REQUIRE( xtt::unit_target::is_founder_vesting( XTT_DESTINATION_FROM_VESTING ) == false );
   BOOST_REQUIRE( xtt::unit_target::is_founder_vesting( XTT_DESTINATION_REWARDS ) == false );
   BOOST_REQUIRE( xtt::unit_target::is_founder_vesting( XTT_DESTINATION_MARKET_MAKER ) == false );
   BOOST_REQUIRE( xtt::unit_target::is_founder_vesting( XTT_DESTINATION_FROM ) == false );
   BOOST_REQUIRE( xtt::unit_target::is_founder_vesting( XTT_DESTINATION_FROM_VESTING ) == false );
   BOOST_REQUIRE( xtt::unit_target::is_founder_vesting( XTT_DESTINATION_VESTING ) == false );

   BOOST_TEST_MESSAGE( " -- Partial founder vesting special name" );
   BOOST_REQUIRE( xtt::unit_target::is_founder_vesting( "$!bob" ) == false );
   BOOST_REQUIRE_THROW( xtt::unit_target::get_unit_target_account( "$!bob" ), fc::assert_exception );

   BOOST_REQUIRE( xtt::unit_target::is_founder_vesting( "bob.vesting" ) == false );

   BOOST_TEST_MESSAGE( " -- Valid account name that appears to be founder vesting" );
   BOOST_REQUIRE( xtt::unit_target::get_unit_target_account( "bob.vesting" ) == wallet_name_type( "bob.vesting" ) );
}
*/
/*
BOOST_AUTO_TEST_CASE( tick_pricing_rules_validation )
{
   BOOST_TEST_MESSAGE( "Testing: tick_pricing_rules_validatation" );

   auto symbol = get_new_xtt_symbol( 5, db );

   price tick_price;
   tick_price.base  = asset( 56, XGT_SYMBOL );
   tick_price.quote = asset( 10, symbol );

   BOOST_TEST_MESSAGE( " -- Test success when XGT is base symbol when paired with a token" );
   validate_tick_pricing( tick_price );

   BOOST_TEST_MESSAGE( " -- Test failure XGT must be the base symbol when trading tokens" );
   tick_price = ~tick_price;
   BOOST_REQUIRE_THROW( validate_tick_pricing( tick_price ), fc::assert_exception );
   tick_price = ~tick_price;

   BOOST_TEST_MESSAGE( " -- Test failure quote symbol must be a power of 10" );
   tick_price.quote = asset( 11, symbol );
   BOOST_REQUIRE_THROW( validate_tick_pricing( tick_price ), fc::assert_exception );
   tick_price.quote = asset( 10, symbol );

   BOOST_TEST_MESSAGE( " -- Test failure when VESTS are the base symbol." );
   tick_price.quote = asset( 10, VESTS_SYMBOL );
   tick_price.base  = asset( 10, XGT_SYMBOL );
   BOOST_REQUIRE_THROW( validate_tick_pricing( tick_price ), fc::assert_exception );

   BOOST_TEST_MESSAGE( " -- Test failure when VESTS are the quote symbol." );
   tick_price = ~tick_price;
   BOOST_REQUIRE_THROW( validate_tick_pricing( tick_price ), fc::assert_exception );

   BOOST_TEST_MESSAGE( " -- Test failure when XTT vesting symbol is the base symbol." );
   tick_price.quote = asset( 10, symbol.get_paired_symbol() );
   tick_price.base  = asset( 10, XGT_SYMBOL );
   BOOST_REQUIRE_THROW( validate_tick_pricing( tick_price ), fc::assert_exception );

   BOOST_TEST_MESSAGE( " -- Test failure when XTT vesting symbol is the quote symbol." );
   tick_price = ~tick_price;
   BOOST_REQUIRE_THROW( validate_tick_pricing( tick_price ), fc::assert_exception );

   BOOST_TEST_MESSAGE( " -- Test failure SBD must be the base symbol when trading SBDs" );
   tick_price.quote = asset( 10, SBD_SYMBOL );
   tick_price.base  = asset( 10, XGT_SYMBOL );
   BOOST_REQUIRE_THROW( validate_tick_pricing( tick_price ), fc::assert_exception );

   BOOST_TEST_MESSAGE( " -- Test success when SBD is base symbol when paired with XGT" );
   tick_price = ~tick_price;
   tick_price.base.amount = 11;
   validate_tick_pricing( tick_price );
}

BOOST_AUTO_TEST_CASE( tick_pricing_rules )
{
   BOOST_TEST_MESSAGE( "Testing: tick_pricing_rules" );

   ACTORS( (alice)(bob)(charlie)(creator) )
   const auto& token = create_xtt( "creator", creator_private_key, 3 );
   fund( "alice", asset( 1000000, XGT_SYMBOL) );
   fund( "alice", asset( 1000000, SBD_SYMBOL) );
   fund( "alice", asset( 1000000, token) );
   fund( "bob", asset( 1000000, XGT_SYMBOL) );
   fund( "bob", asset( 1000000, SBD_SYMBOL) );
   fund( "bob", asset( 1000000, token) );
   fund( "charlie", asset( 1000000, XGT_SYMBOL) );

   witness_create( "charlie", charlie_private_key, "foo.bar", charlie_private_key.get_public_key(), 1000 );
   signed_transaction tx;

   BOOST_TEST_MESSAGE( "--- Test failure publishing price feed when quote is not power of 10" );
   feed_publish_operation fop;
   fop.publisher = "charlie";
   fop.exchange_rate = price( asset( 1000, SBD_SYMBOL ), asset( 1100000, XGT_SYMBOL ) );
   tx.operations.push_back( fop );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, charlie_private_key );
   BOOST_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), fc::assert_exception );
   tx.operations.clear();
   tx.signatures.clear();

   BOOST_TEST_MESSAGE( "--- Test failure publishing price feed when SBD is not base" );
   fop.exchange_rate = price( asset( 1000000, XGT_SYMBOL ), asset( 1000, SBD_SYMBOL ) );
   tx.operations.push_back( fop );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, charlie_private_key );
   BOOST_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), fc::assert_exception );
   tx.operations.clear();
   tx.signatures.clear();

   BOOST_TEST_MESSAGE( "--- Test success publishing price feed" );
   fop.exchange_rate = ~fop.exchange_rate;
   tx.operations.push_back( fop );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, charlie_private_key );
   db->push_transaction( tx, database::skip_transaction_dupe_check );
   tx.operations.clear();
   tx.signatures.clear();

   limit_order_create2_operation op;

   BOOST_TEST_MESSAGE( " -- Test success when matching two orders of the same token" );
   op.owner = "alice";
   op.amount_to_sell = asset( 1000, token );
   op.exchange_rate = price( asset( 125000, XGT_SYMBOL ), asset( 1000, token ) );
   op.expiration = db->head_block_time() + fc::seconds( XGT_MAX_LIMIT_ORDER_EXPIRATION );
   tx.operations.push_back( op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   db->push_transaction( tx, database::skip_transaction_dupe_check );
   tx.operations.clear();
   tx.signatures.clear();

   op.owner = "bob";
   op.amount_to_sell = asset( 125000, XGT_SYMBOL );
   op.exchange_rate = price( asset( 125000, XGT_SYMBOL ), asset( 1000, token ) );
   op.expiration = db->head_block_time() + fc::seconds( XGT_MAX_LIMIT_ORDER_EXPIRATION );
   tx.operations.push_back( op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, bob_private_key );
   db->push_transaction( tx, database::skip_transaction_dupe_check );
   tx.operations.clear();
   tx.signatures.clear();

   BOOST_REQUIRE( db->get_balance( "alice", XGT_SYMBOL ) == asset( 1000000 + 125000, XGT_SYMBOL ) );
   BOOST_REQUIRE( db->get_balance( "alice", token )        == asset( 1000000 - 1000, token ) );

   BOOST_REQUIRE( db->get_balance( "bob", XGT_SYMBOL ) == asset( 1000000 - 125000, XGT_SYMBOL ) );
   BOOST_REQUIRE( db->get_balance( "bob", token )        == asset( 1000000 + 1000, token ) );

   BOOST_TEST_MESSAGE( " -- Test failure when quote is not a power of 10" );
   op.owner = "bob";
   op.amount_to_sell = asset( 125000, XGT_SYMBOL );
   op.exchange_rate = price( asset( 1000, SBD_SYMBOL ), asset( 1100000, XGT_SYMBOL ) );
   op.expiration = db->head_block_time() + fc::seconds( XGT_MAX_LIMIT_ORDER_EXPIRATION );
   tx.operations.push_back( op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, bob_private_key );
   BOOST_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), fc::assert_exception );
   tx.operations.clear();
   tx.signatures.clear();

   BOOST_TEST_MESSAGE( " -- Test failure when XGT is not base" );
   op.owner = "bob";
   op.amount_to_sell = asset( 125000, XGT_SYMBOL );
   op.exchange_rate = price( asset( 10000, token ), asset( 125000, XGT_SYMBOL ) );
   op.expiration = db->head_block_time() + fc::seconds( XGT_MAX_LIMIT_ORDER_EXPIRATION );
   tx.operations.push_back( op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, bob_private_key );
   BOOST_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), fc::assert_exception );
   tx.operations.clear();
   tx.signatures.clear();

   BOOST_TEST_MESSAGE( " -- Test failure when SBD is not base" );
   op.owner = "bob";
   op.amount_to_sell = asset( 1000000, XGT_SYMBOL );
   op.exchange_rate = price( asset( 10000, XGT_SYMBOL ), asset( 1000000, SBD_SYMBOL ) );
   op.expiration = db->head_block_time() + fc::seconds( XGT_MAX_LIMIT_ORDER_EXPIRATION );
   tx.operations.push_back( op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, bob_private_key );
   BOOST_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), fc::assert_exception );
   tx.operations.clear();
   tx.signatures.clear();

   BOOST_TEST_MESSAGE( " -- Test success when SBD is base" );
   op.owner = "bob";
   op.amount_to_sell = asset( 10000, XGT_SYMBOL );
   op.exchange_rate = price( asset( 1000000, SBD_SYMBOL ), asset( 10000, XGT_SYMBOL ) );
   op.expiration = db->head_block_time() + fc::seconds( XGT_MAX_LIMIT_ORDER_EXPIRATION );
   tx.operations.push_back( op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, bob_private_key );
   db->push_transaction( tx, database::skip_transaction_dupe_check );
   tx.operations.clear();
   tx.signatures.clear();

   BOOST_TEST_MESSAGE( " -- Test matching SBD:XGT" );
   op.owner = "alice";
   op.amount_to_sell = asset( 1000000, SBD_SYMBOL );
   op.exchange_rate = price( asset( 1000000, SBD_SYMBOL ), asset( 10000, XGT_SYMBOL ) );
   op.expiration = db->head_block_time() + fc::seconds( XGT_MAX_LIMIT_ORDER_EXPIRATION );
   tx.operations.push_back( op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   db->push_transaction( tx, database::skip_transaction_dupe_check );
   tx.operations.clear();
   tx.signatures.clear();

   BOOST_REQUIRE( db->get_balance( "alice", XGT_SYMBOL ) == asset( 1000000 + 125000 + 10000, XGT_SYMBOL ) );
   BOOST_REQUIRE( db->get_balance( "alice", SBD_SYMBOL )   == asset( 1000000 - 1000000, SBD_SYMBOL ) );

   BOOST_REQUIRE( db->get_balance( "bob", XGT_SYMBOL ) == asset( 1000000 - 125000 - 10000, XGT_SYMBOL ) );
   BOOST_REQUIRE( db->get_balance( "bob", SBD_SYMBOL )   == asset( 1000000 + 1000000, SBD_SYMBOL ) );
}
*/
/*
BOOST_AUTO_TEST_CASE( price_as_decimal_and_real )
{
   BOOST_TEST_MESSAGE( "Testing: price_as_decimal_and_real" );

   const auto symbol = get_new_xtt_symbol( 3, this->db );

   const auto float_cmp = []( double a, double b, double epsilon = 0.00005f ) -> bool
   {
      return ( std::fabs( a - b ) < epsilon );
   };

   price p;
   p.base  = asset( 123456, SBD_SYMBOL );
   p.quote = asset( 1000, XGT_SYMBOL );

   BOOST_TEST_MESSAGE( " -- Testing SBD:XGT pairing with Tick Pricing Rules" );

   BOOST_REQUIRE( p.as_decimal() == "123.456" );
   BOOST_REQUIRE( float_cmp( p.as_real(), 123.456f ) );

   BOOST_TEST_MESSAGE( " -- Testing SBD:XGT inverse pairing with Tick Pricing Rules" );

   p = ~p;

   BOOST_REQUIRE( p.as_decimal() == "123.456" );
   BOOST_REQUIRE( float_cmp( p.as_real(), 123.456f ) );

   BOOST_TEST_MESSAGE( " -- Testing XGT:XTT pairing with Tick Pricing Rules" );

   p.base  = asset( 123, XGT_SYMBOL );
   p.quote = asset( 10000, symbol );

   BOOST_REQUIRE( p.as_decimal() == "0.0123" );
   BOOST_REQUIRE( float_cmp( p.as_real(), 0.0123f ) );

   BOOST_TEST_MESSAGE( " -- Testing XGT:XTT inverse pairing with Tick Pricing Rules" );

   p = ~p;

   BOOST_REQUIRE( p.as_decimal() == "0.0123" );
   BOOST_REQUIRE( float_cmp( p.as_real(), 0.0123f ) );

   BOOST_TEST_MESSAGE( " -- Testing XGT:XTT pairing without Tick Pricing Rules" );

   p.base  = asset( 123, XGT_SYMBOL );
   p.quote = asset( 10001, symbol );

   BOOST_REQUIRE( p.as_decimal() == "0.0122987701229877?" );
   BOOST_REQUIRE( float_cmp( p.as_real(), 0.0122987701229877f ) );

   BOOST_TEST_MESSAGE( " -- Testing XGT:XTT inverse pairing without Tick Pricing Rules" );

   p = ~p;

   BOOST_REQUIRE( p.as_decimal() == "0.0122987701229877?" );
   BOOST_REQUIRE( float_cmp( p.as_real(), 0.0122987701229877f ) );

   BOOST_TEST_MESSAGE( " -- Testing SBD:XGT pairing without Tick Pricing Rules" );

   p.base  = asset( 123, SBD_SYMBOL );
   p.quote = asset( 10001, XGT_SYMBOL );

   BOOST_REQUIRE( p.as_decimal() == "0.0122987701229877?" );
   BOOST_REQUIRE( float_cmp( p.as_real(), 0.0122987701229877f ) );

   BOOST_TEST_MESSAGE( " -- Testing SBD:XGT inverse pairing without Tick Pricing Rules" );

   p = ~p;

   BOOST_REQUIRE( p.as_decimal() == "0.0122987701229877?" );
   BOOST_REQUIRE( float_cmp( p.as_real(), 0.0122987701229877f ) );
}

BOOST_AUTO_TEST_CASE( token_emission_timing )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_standard_token_emissions" );
      ACTORS( (alice)(bob)(sam)(dave) )

      generate_block();

      BOOST_TEST_MESSAGE( " -- XTT creation" );
      auto symbol = create_xtt( "alice", alice_private_key, 3 );
      signed_transaction tx;

      generate_block();

      xtt_setup_emissions_operation op;
      op.control_account = "alice";
      op.symbol          = symbol;
      op.schedule_time   = db->head_block_time() + fc::days( 1 );
      op.emissions_unit.token_unit[ "alice" ] = 1;
      op.interval_seconds = XTT_EMISSION_MIN_INTERVAL_SECONDS;
      op.emission_count   = 1;
      op.lep_time = op.schedule_time;
      op.rep_time = op.schedule_time;
      op.lep_abs_amount = 100;
      op.rep_abs_amount = 200;
      op.lep_rel_amount_numerator = 1;
      op.rep_rel_amount_numerator = 2;
      op.rel_amount_denom_bits    = 7;
      tx.operations.push_back( op );

      op.schedule_time = op.schedule_time + fc::hours( 6 );
      op.lep_time = op.schedule_time;
      op.rep_time = op.schedule_time;
      op.emission_count = 4;
      tx.operations.push_back( op );

      op.schedule_time = op.schedule_time + fc::days( 1 );
      op.lep_time = op.schedule_time;
      op.rep_time = op.schedule_time;
      op.emission_count = 4;
      tx.operations.push_back( op );

      op.schedule_time = op.schedule_time + fc::days( 1 );
      op.lep_time = op.schedule_time;
      op.rep_time = op.schedule_time;
      op.emission_count = 8;
      tx.operations.push_back( op );

      op.schedule_time = op.schedule_time + fc::days( 2 );
      op.lep_time = op.schedule_time;
      op.rep_time = op.schedule_time;
      op.emission_count = 1;
      tx.operations.push_back( op );

      op.schedule_time = op.schedule_time + fc::hours( 6 );
      op.lep_time = op.schedule_time;
      op.rep_time = op.schedule_time;
      op.emission_count = 1;
      tx.operations.push_back( op );

      BOOST_TEST_MESSAGE( " -- Success on valid token emission setup" );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );
      tx.operations.clear();
      tx.signatures.clear();

      xtt_setup_emissions_operation fail_op;
      fail_op = op;
      fail_op.schedule_time = op.schedule_time + fc::hours( 5 ) + fc::minutes( 59 ) + fc::seconds( 59 );
      fail_op.lep_time = fail_op.schedule_time;
      fail_op.rep_time = fail_op.schedule_time;
      fail_op.emission_count = 1;
      tx.operations.push_back( fail_op );

      BOOST_TEST_MESSAGE( " -- Failure on token emission emitting too soon" );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
      tx.operations.clear();
      tx.signatures.clear();

      op.schedule_time = op.schedule_time + fc::hours( 6 );
      op.lep_time = op.schedule_time;
      op.rep_time = op.schedule_time;
      op.emission_count = 4;
      tx.operations.push_back( op );

      BOOST_TEST_MESSAGE( " -- Success on valid additional token emission" );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );
      tx.operations.clear();
      tx.signatures.clear();

      fail_op = op;
      fail_op.schedule_time = op.schedule_time + fc::days( 1 ) - fc::seconds( 1 );
      fail_op.lep_time = fail_op.schedule_time;
      fail_op.rep_time = fail_op.schedule_time;
      fail_op.emission_count = 1;
      tx.operations.push_back( fail_op );

      BOOST_TEST_MESSAGE( " -- Failure on token emission emitting too soon" );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
      tx.operations.clear();
      tx.signatures.clear();

      op.schedule_time = op.schedule_time + fc::days( 1 );
      op.lep_time = op.schedule_time;
      op.rep_time = op.schedule_time;
      op.emission_count = 1;
      tx.operations.push_back( op );

      BOOST_TEST_MESSAGE( " -- Success on valid additional token emission" );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );
      tx.operations.clear();
      tx.signatures.clear();
   }
   FC_LOG_AND_RETHROW()
}
*/
/*
 * XTT legacy tests
 *
 * The logic tests in legacy tests *should* be entirely duplicated in xtt_operation_tests.cpp
 * We are keeping these tests around to provide an additional layer re-assurance for the time being
 */
FC_TODO( "Remove XTT legacy tests and ensure code coverage is not reduced" );
/*
BOOST_AUTO_TEST_CASE( setup_validate )
{
   try
   {
      xtt_setup_operation op;

      ACTORS( (alice) )
      generate_block();
      asset_symbol_type alice_symbol = create_xtt("alice", alice_private_key, 4);

      op.control_account = "";
      XGT_REQUIRE_THROW( op.validate(), fc::exception );

      //Invalid account
      op.control_account = "&&&&&&";
      XGT_REQUIRE_THROW( op.validate(), fc::exception );

      //FC_ASSERT( max_supply > 0 )
      op.control_account = "abcd";
      op.max_supply = -1;
      XGT_REQUIRE_THROW( op.validate(), fc::exception );

      op.symbol = alice_symbol;

      //FC_ASSERT( max_supply > 0 )
      op.max_supply = 0;
      XGT_REQUIRE_THROW( op.validate(), fc::exception );

      //FC_ASSERT( max_supply <= XGT_MAX_SHARE_SUPPLY )
      op.max_supply = XGT_MAX_SHARE_SUPPLY + 1;
      XGT_REQUIRE_THROW( op.validate(), fc::exception );

      //FC_ASSERT( generation_begin_time > XGT_GENESIS_TIME )
      op.max_supply = XGT_MAX_SHARE_SUPPLY / 1000;
      op.contribution_begin_time = XGT_GENESIS_TIME;
      XGT_REQUIRE_THROW( op.validate(), fc::exception );

      fc::time_point_sec start_time = fc::variant( "2018-03-07T00:00:00" ).as< fc::time_point_sec >();
      fc::time_point_sec t50 = start_time + fc::seconds( 50 );
      fc::time_point_sec t100 = start_time + fc::seconds( 100 );
      fc::time_point_sec t200 = start_time + fc::seconds( 200 );
      fc::time_point_sec t300 = start_time + fc::seconds( 300 );

      op.contribution_begin_time = t100;
      op.contribution_end_time = t50;
      XGT_REQUIRE_THROW( op.validate(), fc::exception );

      op.contribution_begin_time = t100;
      XGT_REQUIRE_THROW( op.validate(), fc::exception );

      op.launch_time = t200;
      op.contribution_end_time = t300;
      XGT_REQUIRE_THROW( op.validate(), fc::exception );

      op.contribution_begin_time = t50;
      op.contribution_end_time = t100;
      op.launch_time = t300;
      op.validate();
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( setup_authorities )
{
   try
   {
      xtt_setup_operation op;
      op.control_account = "alice";

      flat_set< wallet_name_type > auths;
      flat_set< wallet_name_type > expected;

      op.get_required_recovery_authorities( auths );
      BOOST_REQUIRE( auths == expected );

      op.get_required_social_authorities( auths );
      BOOST_REQUIRE( auths == expected );

      expected.insert( "alice" );
      op.get_required_money_authorities( auths );
      BOOST_REQUIRE( auths == expected );
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( setup_apply )
{
   try
   {
      ACTORS( (alice)(bob)(xyz)(xyz2) )

      generate_block();

      FUND( "alice", 10 * 1000 * 1000 );
      FUND( "bob", 10 * 1000 * 1000 );

      set_price_feed( price( ASSET( "1.000 TBD" ), ASSET( "1.000 TESTS" ) ) );

      xtt_setup_operation op;
      op.control_account = "alice";

      fc::time_point_sec start_time        = fc::variant( "2021-01-01T00:00:00" ).as< fc::time_point_sec >();
      fc::time_point_sec start_time_plus_1 = start_time + fc::seconds(1);

      op.contribution_begin_time = start_time;
      op.contribution_end_time = op.launch_time = start_time_plus_1;

      signed_transaction tx;

      //XTT doesn't exist
      tx.operations.push_back( op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );
      tx.operations.clear();
      tx.signatures.clear();

      //Try to elevate account
      asset_symbol_type alice_symbol = create_xtt( "alice", alice_private_key, 3 );
      tx.operations.clear();
      tx.signatures.clear();

      //Make transaction again. Everything is correct.
      op.symbol = alice_symbol;
      tx.operations.push_back( op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );
      tx.operations.clear();
      tx.signatures.clear();
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( xtt_create_apply )
{
   try
   {
      ACTORS( (alice)(bob) )

      generate_block();

      set_price_feed( price( ASSET( "1.000 TBD" ), ASSET( "1.000 TESTS" ) ) );

      const dynamic_global_property_object& dgpo = db->get_dynamic_global_properties();
      asset required_creation_fee = dgpo.xtt_creation_fee;
      unsigned int test_amount = required_creation_fee.amount.value;

      xtt_create_operation op;
      op.control_account = "alice";
      op.symbol = get_new_xtt_symbol( 3, db );
      op.precision = op.symbol.decimals();

      BOOST_TEST_MESSAGE( " -- XTT create with insufficient SBD balance" );
      // Fund with XGT, and set fee with SBD.
      FUND( "alice", test_amount );
      // Declare fee in SBD/TBD though alice has none.
      op.xtt_creation_fee = asset( test_amount, SBD_SYMBOL );
      // Throw due to insufficient balance of SBD/TBD.
      FAIL_WITH_OP(op, alice_private_key, fc::assert_exception);

      BOOST_TEST_MESSAGE( " -- XTT create with insufficient XGT balance" );
      // Now fund with SBD, and set fee with XGT.
      convert( "alice", asset( test_amount, XGT_SYMBOL ) );
      // Declare fee in XGT though alice has none.
      op.xtt_creation_fee = asset( test_amount, XGT_SYMBOL );
      // Throw due to insufficient balance of XGT.
      FAIL_WITH_OP(op, alice_private_key, fc::assert_exception);

      BOOST_TEST_MESSAGE( " -- XTT create with available funds" );
      // Push valid operation.
      op.xtt_creation_fee = asset( test_amount, SBD_SYMBOL );
      PUSH_OP( op, alice_private_key );

      BOOST_TEST_MESSAGE( " -- XTT cannot be created twice even with different precision" );
      create_conflicting_xtt(op.symbol, "alice", alice_private_key);

      BOOST_TEST_MESSAGE( " -- Another user cannot create an XTT twice even with different precision" );
      // Check that another user/account can't be used to create duplicating XTT even with different precision.
      create_conflicting_xtt(op.symbol, "bob", bob_private_key);

      BOOST_TEST_MESSAGE( " -- Check that an XTT cannot be created with decimals greater than XGT_MAX_DECIMALS" );
      // Check that invalid XTT can't be created
      create_invalid_xtt("alice", alice_private_key);

      BOOST_TEST_MESSAGE( " -- Check that an XTT cannot be created with a creation fee lower than required" );
      // Check fee set too low.
      asset fee_too_low = required_creation_fee;
      unsigned int too_low_fee_amount = required_creation_fee.amount.value-1;
      fee_too_low.amount -= 1;

      XTT_SYMBOL( bob, 0, db );
      op.control_account = "bob";
      op.symbol = bob_symbol;
      op.precision = op.symbol.decimals();

      BOOST_TEST_MESSAGE( " -- Check that we cannot create an XTT with an insufficent XGT creation fee" );
      // Check too low fee in XGT.
      FUND( "bob", too_low_fee_amount );
      op.xtt_creation_fee = asset( too_low_fee_amount, XGT_SYMBOL );
      FAIL_WITH_OP(op, bob_private_key, fc::assert_exception);

      BOOST_TEST_MESSAGE( " -- Check that we cannot create an XTT with an insufficent SBD creation fee" );
      // Check too low fee in SBD.
      convert( "bob", asset( too_low_fee_amount, XGT_SYMBOL ) );
      op.xtt_creation_fee = asset( too_low_fee_amount, SBD_SYMBOL );
      FAIL_WITH_OP(op, bob_private_key, fc::assert_exception);

      validate_database();
   }
   FC_LOG_AND_RETHROW()
}
*/
BOOST_AUTO_TEST_SUITE_END()
#endif
