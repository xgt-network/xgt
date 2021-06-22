
#if defined IS_TEST_NET

#include <boost/test/unit_test.hpp>

#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/protocol/exceptions.hpp>
#include <xgt/protocol/hardfork.hpp>
#include <xgt/protocol/xtt_util.hpp>

#include <xgt/chain/block_summary_object.hpp>
#include <xgt/chain/database.hpp>
#include <xgt/chain/history_object.hpp>
#include <xgt/chain/xgt_objects.hpp>
#include <xgt/chain/xtt_objects.hpp>

// #include <xgt/chain/util/reward.hpp>
// #include <xgt/plugins/rc/rc_objects.hpp>
// #include <xgt/plugins/rc/rc_operations.hpp>
// #include <xgt/plugins/rc/rc_plugin.hpp>
#include <xgt/plugins/debug_node/debug_node_plugin.hpp>

#include <fc/crypto/digest.hpp>

#include "../db_fixture/database_fixture.hpp"

#include <cmath>

using namespace xgt;
using namespace xgt::chain;
using namespace xgt::protocol;

BOOST_FIXTURE_TEST_SUITE( xtt_operation_time_tests, clean_database_fixture )

BOOST_AUTO_TEST_CASE( xtt_refunds )
{
   try
   {

      BOOST_TEST_MESSAGE( "Testing XTT contribution refunds" );
      ACTORS( (alice)(bob)(sam)(dave) )

      generate_block();

      auto bobs_balance  = asset( 1000000, XGT_SYMBOL );
      auto sams_balance  = asset( 800000, XGT_SYMBOL );
      auto daves_balance = asset( 600000, XGT_SYMBOL );

      FUND( "bob", bobs_balance );
      FUND( "sam", sams_balance );
      FUND( "dave", daves_balance );

      generate_block();

      BOOST_TEST_MESSAGE( " --- XTT creation" );
      auto symbol = create_xtt( "alice", alice_private_key, 3 );
      const auto& token = db->get< xtt_token_object, by_symbol >( symbol );

      BOOST_TEST_MESSAGE( " --- XTT setup" );
      signed_transaction tx;
      xtt_setup_operation setup_op;

      uint64_t contribution_window_blocks = 10;
      setup_op.control_account = "alice";
      setup_op.symbol = symbol;
      setup_op.contribution_begin_time = db->head_block_time() + XGT_BLOCK_INTERVAL;
      setup_op.contribution_end_time = setup_op.contribution_begin_time + ( XGT_BLOCK_INTERVAL * contribution_window_blocks );
      setup_op.xgt_units_min      = 2400001;
      setup_op.max_supply = XGT_MAX_SHARE_SUPPLY;
      setup_op.min_unit_ratio = 1;
      setup_op.max_unit_ratio = 2;
      setup_op.launch_time = setup_op.contribution_end_time + XGT_BLOCK_INTERVAL;

      xtt_capped_generation_policy capped_generation_policy;
      capped_generation_policy.generation_unit.xgt_unit[ "alice" ] = 1;
      capped_generation_policy.generation_unit.token_unit[ "alice" ] = 2;

      xtt_setup_ico_tier_operation ico_tier_op1;
      ico_tier_op1.control_account = "alice";
      ico_tier_op1.symbol = symbol;
      ico_tier_op1.generation_policy = capped_generation_policy;
      ico_tier_op1.xgt_units_cap = 2400001;

      xtt_setup_ico_tier_operation ico_tier_op2;
      ico_tier_op2.control_account = "alice";
      ico_tier_op2.symbol = symbol;
      ico_tier_op2.generation_policy = capped_generation_policy;
      ico_tier_op2.xgt_units_cap = 4000000;

      tx.operations.push_back( ico_tier_op1 );
      tx.operations.push_back( ico_tier_op2 );
      tx.operations.push_back( setup_op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );
      tx.operations.clear();
      tx.signatures.clear();

      BOOST_REQUIRE( token.phase == xtt_phase::setup_completed );

      generate_block();

      BOOST_REQUIRE( token.phase == xtt_phase::ico );

      BOOST_TEST_MESSAGE( " --- XTT contributions" );
      uint32_t num_contributions = 0;
      for ( uint64_t i = 0; i < contribution_window_blocks; i++ )
      {
         xtt_contribute_operation contrib_op;

         contrib_op.symbol = symbol;
         contrib_op.contribution_id = i;

         contrib_op.contributor = "bob";
         contrib_op.contribution = asset( bobs_balance.amount / contribution_window_blocks, XGT_SYMBOL );


         tx.operations.push_back( contrib_op );
         tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
         sign( tx, bob_private_key );
         db->push_transaction( tx, 0 );
         tx.operations.clear();
         tx.signatures.clear();
         num_contributions++;

         contrib_op.contributor = "sam";
         contrib_op.contribution = asset( sams_balance.amount / contribution_window_blocks, XGT_SYMBOL );

         tx.operations.push_back( contrib_op );
         tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
         sign( tx, sam_private_key );
         db->push_transaction( tx, 0 );
         tx.operations.clear();
         tx.signatures.clear();
         num_contributions++;

         contrib_op.contributor = "dave";
         contrib_op.contribution = asset( daves_balance.amount / contribution_window_blocks, XGT_SYMBOL );

         tx.operations.push_back( contrib_op );
         tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
         sign( tx, dave_private_key );
         db->push_transaction( tx, 0 );
         tx.operations.clear();
         tx.signatures.clear();
         num_contributions++;

         if ( i < contribution_window_blocks - 1 )
            generate_block();

         validate_database();
      }

      BOOST_REQUIRE( token.phase == xtt_phase::ico );

      BOOST_TEST_MESSAGE( " --- Checking contributor balances" );

      BOOST_REQUIRE( db->get_balance( "bob", XGT_SYMBOL ) == asset( 0, XGT_SYMBOL ) );
      BOOST_REQUIRE( db->get_balance( "sam", XGT_SYMBOL ) == asset( 0, XGT_SYMBOL ) );
      BOOST_REQUIRE( db->get_balance( "dave", XGT_SYMBOL ) == asset( 0, XGT_SYMBOL ) );

      generate_block();

      BOOST_REQUIRE( token.phase == xtt_phase::launch_failed );

      validate_database();

      BOOST_TEST_MESSAGE( " --- Starting the cascading refunds" );

      generate_blocks( num_contributions / 2 );

      validate_database();

      generate_blocks( num_contributions / 2 + 1 );

      BOOST_TEST_MESSAGE( " --- Checking contributor balances" );

      BOOST_REQUIRE( db->get_balance( "bob", XGT_SYMBOL ) == bobs_balance );
      BOOST_REQUIRE( db->get_balance( "sam", XGT_SYMBOL ) == sams_balance );
      BOOST_REQUIRE( db->get_balance( "dave", XGT_SYMBOL ) == daves_balance );

      validate_database();

      auto& ico_idx = db->get_index< xtt_ico_index, by_symbol >();
      BOOST_REQUIRE( ico_idx.find( symbol ) == ico_idx.end() );
      auto& contribution_idx = db->get_index< xtt_contribution_index, by_symbol_id >();
      BOOST_REQUIRE( contribution_idx.find( boost::make_tuple( symbol, 0 ) ) == contribution_idx.end() );
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( xtt_ico_payouts )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing XTT ICO payouts" );
      ACTORS( (creator)(alice)(bob)(charlie)(dan)(elaine)(fred)(george)(henry) )

      generate_block();

      auto alices_balance    = asset( 5000000, XGT_SYMBOL );
      auto bobs_balance      = asset( 25000000, XGT_SYMBOL );
      auto charlies_balance  = asset( 10000000, XGT_SYMBOL );
      auto dans_balance      = asset( 25000000, XGT_SYMBOL );
      auto elaines_balance   = asset( 60000000, XGT_SYMBOL );
      auto freds_balance     = asset( 0, XGT_SYMBOL );
      auto georges_balance   = asset( 0, XGT_SYMBOL );
      auto henrys_balance    = asset( 0, XGT_SYMBOL );

      std::map< std ::string, std::tuple< share_type, fc::ecc::private_key > > contributor_contributions {
         { "alice",   { alices_balance.amount,   alice_private_key   } },
         { "bob",     { bobs_balance.amount,     bob_private_key     } },
         { "charlie", { charlies_balance.amount, charlie_private_key } },
         { "dan",     { dans_balance.amount,     dan_private_key     } },
         { "elaine",  { elaines_balance.amount,  elaine_private_key  } },
         { "fred",    { freds_balance.amount,    fred_private_key    } },
         { "george",  { georges_balance.amount,  george_private_key  } },
         { "henry",   { henrys_balance.amount,   henry_private_key   } }
      };

      for ( auto& e : contributor_contributions )
      {
         FUND( e.first, asset( std::get< 0 >( e.second ), XGT_SYMBOL ) );
      }

      generate_block();

      BOOST_TEST_MESSAGE( " --- XTT creation" );
      auto symbol = create_xtt( "creator", creator_private_key, 3 );
      const auto& token = db->get< xtt_token_object, by_symbol >( symbol );

      BOOST_TEST_MESSAGE( " --- XTT setup" );
      signed_transaction tx;
      xtt_setup_operation setup_op;

      uint64_t contribution_window_blocks = 5;
      setup_op.control_account         = "creator";
      setup_op.symbol                  = symbol;
      setup_op.contribution_begin_time = db->head_block_time() + XGT_BLOCK_INTERVAL;
      setup_op.contribution_end_time   = setup_op.contribution_begin_time + ( XGT_BLOCK_INTERVAL * contribution_window_blocks );
      setup_op.xgt_units_min         = 0;
      setup_op.min_unit_ratio          = 50;
      setup_op.max_unit_ratio          = 100;
      setup_op.max_supply              = XGT_MAX_SHARE_SUPPLY;
      setup_op.launch_time             = setup_op.contribution_end_time + XGT_BLOCK_INTERVAL;

      xtt_capped_generation_policy capped_generation_policy;
      capped_generation_policy.generation_unit.xgt_unit[ "fred" ] = 3;
      capped_generation_policy.generation_unit.xgt_unit[ "george" ] = 2;

      capped_generation_policy.generation_unit.token_unit[ XTT_DESTINATION_FROM ] = 7;
      capped_generation_policy.generation_unit.token_unit[ "george" ] = 1;
      capped_generation_policy.generation_unit.token_unit[ "henry" ] = 2;

      xtt_setup_ico_tier_operation ico_tier_op1;
      ico_tier_op1.control_account = "creator";
      ico_tier_op1.symbol = symbol;
      ico_tier_op1.generation_policy = capped_generation_policy;
      ico_tier_op1.xgt_units_cap = 100000000;

      xtt_setup_ico_tier_operation ico_tier_op2;
      ico_tier_op2.control_account = "creator";
      ico_tier_op2.symbol = symbol;
      ico_tier_op2.generation_policy = capped_generation_policy;
      ico_tier_op2.xgt_units_cap = 150000000;

      tx.operations.push_back( ico_tier_op1 );
      tx.operations.push_back( ico_tier_op2 );
      tx.operations.push_back( setup_op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, creator_private_key );
      db->push_transaction( tx, 0 );
      tx.operations.clear();
      tx.signatures.clear();

      BOOST_REQUIRE( token.phase == xtt_phase::setup_completed );

      generate_block();

      BOOST_REQUIRE( token.phase == xtt_phase::ico );

      BOOST_TEST_MESSAGE( " --- XTT contributions" );

      uint32_t num_contributions = 0;
      for ( auto& e : contributor_contributions )
      {
         if ( std::get< 0 >( e.second ) == 0 )
            continue;

         xtt_contribute_operation contrib_op;

         contrib_op.symbol = symbol;
         contrib_op.contribution_id = 0;
         contrib_op.contributor = e.first;
         contrib_op.contribution = asset( std::get< 0 >( e.second ), XGT_SYMBOL );

         tx.operations.push_back( contrib_op );
         tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
         sign( tx, std::get< 1 >( e.second ) );
         db->push_transaction( tx, 0 );
         tx.operations.clear();
         tx.signatures.clear();

         generate_block();
         num_contributions++;
      }

      validate_database();

      generate_block();

      BOOST_REQUIRE( token.phase == xtt_phase::launch_success );

      BOOST_TEST_MESSAGE( " --- Starting the cascading payouts" );

      generate_blocks( num_contributions / 2 );

      validate_database();

      generate_blocks( num_contributions / 2 + 1 );

      BOOST_TEST_MESSAGE( " --- Checking contributor balances" );

      BOOST_REQUIRE( db->get_balance( "alice", XGT_SYMBOL ).amount == 0 );
      BOOST_REQUIRE( db->get_balance( "bob", XGT_SYMBOL ).amount == 0 );
      BOOST_REQUIRE( db->get_balance( "charlie", XGT_SYMBOL ).amount == 0 );
      BOOST_REQUIRE( db->get_balance( "dan", XGT_SYMBOL ).amount == 0 );
      BOOST_REQUIRE( db->get_balance( "elaine", XGT_SYMBOL ).amount == 0 );
      BOOST_REQUIRE( db->get_balance( "fred", XGT_SYMBOL ).amount == 75000000 );
      BOOST_REQUIRE( db->get_balance( "george", XGT_SYMBOL ).amount == 50000000 );
      BOOST_REQUIRE( db->get_balance( "henry", XGT_SYMBOL ).amount == 0 );

      BOOST_REQUIRE( db->get_balance( "alice", symbol ).amount == 420000000 );
      BOOST_REQUIRE( db->get_balance( "bob", symbol ).amount == 2100000000 );
      BOOST_REQUIRE( db->get_balance( "charlie", symbol ).amount == 840000000 );
      BOOST_REQUIRE( db->get_balance( "dan", symbol ).amount == 2100000000 );
      BOOST_REQUIRE( db->get_balance( "elaine", symbol ).amount == 5040000000 );
      BOOST_REQUIRE( db->get_balance( "fred", symbol ).amount == 0 );
      BOOST_REQUIRE( db->get_balance( "george", symbol ).amount == 1500000000 );
      BOOST_REQUIRE( db->get_balance( "henry", symbol ).amount == 3000000000 );

      validate_database();

      auto& ico_idx = db->get_index< xtt_ico_index, by_symbol >();
      BOOST_REQUIRE( ico_idx.find( symbol ) == ico_idx.end() );
      auto& contribution_idx = db->get_index< xtt_contribution_index, by_symbol_id >();
      BOOST_REQUIRE( contribution_idx.find( boost::make_tuple( symbol, 0 ) ) == contribution_idx.end() );
   }
   FC_LOG_AND_RETHROW()
}
/*
BOOST_AUTO_TEST_CASE( xtt_ico_payouts_special_destinations )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing XTT ICO payouts special destinations" );
      ACTORS( (creator)(alice)(bob)(charlie)(dan)(elaine)(fred)(george)(henry) )

      generate_block();

      auto alices_balance    = asset( 5000000, XGT_SYMBOL );
      auto bobs_balance      = asset( 25000000, XGT_SYMBOL );
      auto charlies_balance  = asset( 10000000, XGT_SYMBOL );
      auto dans_balance      = asset( 25000000, XGT_SYMBOL );
      auto elaines_balance   = asset( 60000000, XGT_SYMBOL );
      auto freds_balance     = asset( 0, XGT_SYMBOL );
      auto georges_balance   = asset( 0, XGT_SYMBOL );
      auto henrys_balance    = asset( 0, XGT_SYMBOL );

      std::map< std ::string, std::tuple< share_type, fc::ecc::private_key > > contributor_contributions {
         { "alice",   { alices_balance.amount,   alice_private_key   } },
         { "bob",     { bobs_balance.amount,     bob_private_key     } },
         { "charlie", { charlies_balance.amount, charlie_private_key } },
         { "dan",     { dans_balance.amount,     dan_private_key     } },
         { "elaine",  { elaines_balance.amount,  elaine_private_key  } },
         { "fred",    { freds_balance.amount,    fred_private_key    } },
         { "george",  { georges_balance.amount,  george_private_key  } },
         { "henry",   { henrys_balance.amount,   henry_private_key   } }
      };

      for ( auto& e : contributor_contributions )
      {
         FUND( e.first, asset( std::get< 0 >( e.second ), XGT_SYMBOL ) );
      }

      generate_block();

      BOOST_TEST_MESSAGE( " --- XTT creation" );
      auto symbol = create_xtt( "creator", creator_private_key, 3 );
      const auto& token = db->get< xtt_token_object, by_symbol >( symbol );

      BOOST_TEST_MESSAGE( " --- XTT setup" );
      signed_transaction tx;
      xtt_setup_operation setup_op;

      uint64_t contribution_window_blocks = 5;
      setup_op.control_account         = "creator";
      setup_op.symbol                  = symbol;
      setup_op.contribution_begin_time = db->head_block_time() + XGT_BLOCK_INTERVAL;
      setup_op.contribution_end_time   = setup_op.contribution_begin_time + ( XGT_BLOCK_INTERVAL * contribution_window_blocks );
      setup_op.xgt_units_min         = 0;
      setup_op.min_unit_ratio          = 50;
      setup_op.max_unit_ratio          = 100;
      setup_op.max_supply              = XGT_MAX_SHARE_SUPPLY;
      setup_op.launch_time             = setup_op.contribution_end_time + XGT_BLOCK_INTERVAL;

      xtt_capped_generation_policy capped_generation_policy1;
      capped_generation_policy1.generation_unit.xgt_unit[ XTT_DESTINATION_MARKET_MAKER ] = 3;
      capped_generation_policy1.generation_unit.xgt_unit[ "george" ] = 2;

      capped_generation_policy1.generation_unit.token_unit[ XTT_DESTINATION_FROM ] = 7;
      capped_generation_policy1.generation_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] = 1;
      capped_generation_policy1.generation_unit.token_unit[ XTT_DESTINATION_REWARDS ] = 2;

      xtt_setup_ico_tier_operation ico_tier_op1;
      ico_tier_op1.control_account = "creator";
      ico_tier_op1.symbol = symbol;
      ico_tier_op1.generation_policy = capped_generation_policy1;
      ico_tier_op1.xgt_units_cap = 100000000;

      xtt_capped_generation_policy capped_generation_policy2;
      capped_generation_policy2.generation_unit.xgt_unit[ XTT_DESTINATION_MARKET_MAKER ] = 3;
      capped_generation_policy2.generation_unit.xgt_unit[ "$!george.vesting" ] = 2;

      capped_generation_policy2.generation_unit.token_unit[ XTT_DESTINATION_FROM ] = 5;
      capped_generation_policy2.generation_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] = 1;
      capped_generation_policy2.generation_unit.token_unit[ XTT_DESTINATION_REWARDS ] = 2;
      capped_generation_policy2.generation_unit.token_unit[ "$!george.vesting" ] = 2;

      xtt_setup_ico_tier_operation ico_tier_op2;
      ico_tier_op2.control_account = "creator";
      ico_tier_op2.symbol = symbol;
      ico_tier_op2.generation_policy = capped_generation_policy2;
      ico_tier_op2.xgt_units_cap = 150000000;

      tx.operations.push_back( ico_tier_op1 );
      tx.operations.push_back( ico_tier_op2 );
      tx.operations.push_back( setup_op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, creator_private_key );
      db->push_transaction( tx, 0 );
      tx.operations.clear();
      tx.signatures.clear();

      BOOST_REQUIRE( token.phase == xtt_phase::setup_completed );

      generate_block();

      BOOST_REQUIRE( token.phase == xtt_phase::ico );

      BOOST_TEST_MESSAGE( " --- XTT contributions" );

      uint32_t num_contributions = 0;
      for ( auto& e : contributor_contributions )
      {
         if ( std::get< 0 >( e.second ) == 0 )
            continue;

         xtt_contribute_operation contrib_op;

         contrib_op.symbol = symbol;
         contrib_op.contribution_id = 0;
         contrib_op.contributor = e.first;
         contrib_op.contribution = asset( std::get< 0 >( e.second ), XGT_SYMBOL );

         tx.operations.push_back( contrib_op );
         tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
         sign( tx, std::get< 1 >( e.second ) );
         db->push_transaction( tx, 0 );
         tx.operations.clear();
         tx.signatures.clear();

         generate_block();
         num_contributions++;
      }

      validate_database();

      generate_block();

      BOOST_REQUIRE( token.phase == xtt_phase::launch_success );

      BOOST_TEST_MESSAGE( " --- Starting the cascading payouts" );

      generate_blocks( num_contributions / 2 );

      validate_database();

      generate_blocks( num_contributions / 2 + 1 );

      BOOST_TEST_MESSAGE( " --- Checking contributor balances" );

      BOOST_REQUIRE( db->get_balance( "alice", XGT_SYMBOL ).amount == 0 );
      BOOST_REQUIRE( db->get_balance( "bob", XGT_SYMBOL ).amount == 0 );
      BOOST_REQUIRE( db->get_balance( "charlie", XGT_SYMBOL ).amount == 0 );
      BOOST_REQUIRE( db->get_balance( "dan", XGT_SYMBOL ).amount == 0 );
      BOOST_REQUIRE( db->get_balance( "elaine", XGT_SYMBOL ).amount == 0 );
      BOOST_REQUIRE( db->get_balance( "fred", XGT_SYMBOL ).amount == 0 );
      BOOST_REQUIRE( db->get_balance( "george", XGT_SYMBOL ).amount == 40000000 );
      BOOST_REQUIRE( db->get_balance( "henry", XGT_SYMBOL ).amount == 0 );

      BOOST_REQUIRE( db->get_account( "george" ).vesting_shares.amount == 5076086140430482 );

      BOOST_REQUIRE( db->get_balance( "alice", symbol ).amount == 420000000 );
      BOOST_REQUIRE( db->get_balance( "bob", symbol ).amount == 2100000000 );
      BOOST_REQUIRE( db->get_balance( "charlie", symbol ).amount == 840000000 );
      BOOST_REQUIRE( db->get_balance( "dan", symbol ).amount == 2100000000 );
      BOOST_REQUIRE( db->get_balance( "elaine", symbol ).amount == 4440000000 );
      BOOST_REQUIRE( db->get_balance( "fred", symbol ).amount == 0 );
      BOOST_REQUIRE( db->get_balance( "george", symbol ).amount == 0 );
      BOOST_REQUIRE( db->get_balance( "henry", symbol ).amount == 0 );

      BOOST_REQUIRE( db->get_balance( "george", symbol.get_paired_symbol() ).amount == 600000000000000 );

      BOOST_TEST_MESSAGE( " --- Checking market maker and rewards fund balances" );

      BOOST_REQUIRE( token.market_maker.xgt_balance == asset( 75000000, XGT_SYMBOL ) );
      BOOST_REQUIRE( token.market_maker.token_balance == asset( 1500000000, symbol ) );
      BOOST_REQUIRE( token.reward_balance == asset( 3000000000, symbol ) );

      validate_database();

      auto& ico_idx = db->get_index< xtt_ico_index, by_symbol >();
      BOOST_REQUIRE( ico_idx.find( symbol ) == ico_idx.end() );
      auto& contribution_idx = db->get_index< xtt_contribution_index, by_symbol_id >();
      BOOST_REQUIRE( contribution_idx.find( boost::make_tuple( symbol, 0 ) ) == contribution_idx.end() );
   }
   FC_LOG_AND_RETHROW()
}*/
/*
BOOST_AUTO_TEST_CASE( xt_vesting_withdrawals )
{
   BOOST_TEST_MESSAGE( "Testing: XTT vesting withdrawals" );

   ACTORS( (alice)(creator) )
   generate_block();

   auto symbol = create_xtt( "creator", creator_private_key, 3 );
   fund( "alice", asset( 100000, symbol ) );
   vest( "alice", asset( 100000, symbol ) );

   const auto& token = db->get< xtt_token_object, by_symbol >( symbol );

   auto key = boost::make_tuple( "alice", symbol );
   const auto* balance_obj = db->find< account_regular_balance_object, by_name_liquid_symbol >( key );
   BOOST_REQUIRE( balance_obj != nullptr );

   BOOST_TEST_MESSAGE( " -- Setting up withdrawal" );

   signed_transaction tx;
   withdraw_vesting_operation op;
   op.account = "alice";
   op.vesting_shares = asset( balance_obj->vesting_shares.amount / 2, symbol.get_paired_symbol() );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   tx.operations.push_back( op );
   sign( tx, alice_private_key );
   db->push_transaction( tx, 0 );

   auto next_withdrawal = db->head_block_time() + XTT_VESTING_WITHDRAW_INTERVAL_SECONDS;
   asset vesting_shares = balance_obj->vesting_shares;
   asset original_vesting = vesting_shares;
   asset withdraw_rate = balance_obj->vesting_withdraw_rate;

   BOOST_TEST_MESSAGE( " -- Generating block up to first withdrawal" );
   generate_blocks( next_withdrawal - ( XGT_BLOCK_INTERVAL / 2 ), true);

   BOOST_REQUIRE( balance_obj->vesting_shares.amount.value == vesting_shares.amount.value );

   BOOST_TEST_MESSAGE( " -- Generating block to cause withdrawal" );
   generate_block();

   auto fill_op = get_last_operations( 1 )[ 0 ].get< fill_vesting_withdraw_operation >();

   BOOST_REQUIRE( balance_obj->vesting_shares.amount.value == ( vesting_shares - withdraw_rate ).amount.value );
   BOOST_REQUIRE( ( withdraw_rate * token.get_vesting_share_price() ).amount.value - balance_obj->liquid.amount.value <= 1 ); // Check a range due to differences in the share price
   BOOST_REQUIRE( fill_op.from_account == "alice" );
   BOOST_REQUIRE( fill_op.to_account == "alice" );
   BOOST_REQUIRE( fill_op.withdrawn.amount.value == withdraw_rate.amount.value );
   BOOST_REQUIRE( std::abs( ( fill_op.deposited - fill_op.withdrawn * token.get_vesting_share_price() ).amount.value ) <= 1 );
   validate_database();

   BOOST_TEST_MESSAGE( " -- Generating the rest of the blocks in the withdrawal" );

   vesting_shares = balance_obj->vesting_shares;
   auto balance = balance_obj->liquid;
   auto old_next_vesting = balance_obj->next_vesting_withdrawal;

   for( int i = 1; i < XGT_VESTING_WITHDRAW_INTERVALS - 1; i++ )
   {
      generate_blocks( db->head_block_time() + XTT_VESTING_WITHDRAW_INTERVAL_SECONDS );

      fill_op = get_last_operations( 2 )[ 1 ].get< fill_vesting_withdraw_operation >();

      BOOST_REQUIRE( balance_obj->vesting_shares.amount.value == ( vesting_shares - withdraw_rate ).amount.value );
      BOOST_REQUIRE( balance.amount.value + ( withdraw_rate * token.get_vesting_share_price() ).amount.value - balance_obj->liquid.amount.value <= 1 );
      BOOST_REQUIRE( fill_op.from_account == "alice" );
      BOOST_REQUIRE( fill_op.to_account == "alice" );
      BOOST_REQUIRE( fill_op.withdrawn.amount.value == withdraw_rate.amount.value );
      BOOST_REQUIRE( std::abs( ( fill_op.deposited - fill_op.withdrawn * token.get_vesting_share_price() ).amount.value ) <= 1 );

      if ( i == XGT_VESTING_WITHDRAW_INTERVALS - 1 )
         BOOST_REQUIRE( balance_obj->next_vesting_withdrawal == fc::time_point_sec::maximum() );
      else
         BOOST_REQUIRE( balance_obj->next_vesting_withdrawal.sec_since_epoch() == ( old_next_vesting + XTT_VESTING_WITHDRAW_INTERVAL_SECONDS ).sec_since_epoch() );

      validate_database();

      vesting_shares = balance_obj->vesting_shares;
      balance = balance_obj->liquid;
      old_next_vesting = balance_obj->next_vesting_withdrawal;
   }

   BOOST_TEST_MESSAGE( " -- Generating one more block to finish vesting withdrawal" );
   generate_blocks( db->head_block_time() + XTT_VESTING_WITHDRAW_INTERVAL_SECONDS, true );

   BOOST_REQUIRE( balance_obj->next_vesting_withdrawal.sec_since_epoch() == fc::time_point_sec::maximum().sec_since_epoch() );
   BOOST_REQUIRE( balance_obj->vesting_shares.amount.value == ( original_vesting - op.vesting_shares ).amount.value );

   validate_database();
}
*/
/*
BOOST_AUTO_TEST_CASE( recent_claims_decay )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: recent_rshares_2decay" );
      ACTORS( (alice)(bob)(charlie) )
      generate_block();

      auto alice_symbol = create_xtt( "alice", alice_private_key, 3 );
      auto bob_symbol = create_xtt( "bob", bob_private_key, 3 );
      auto charlie_symbol = create_xtt( "charlie", charlie_private_key, 3 );

      set_price_feed( price( ASSET( "1.000 TBD" ), ASSET( "1.000 TESTS" ) ) );
      generate_block();

      uint64_t alice_recent_claims = 1000000ull;
      uint64_t bob_recent_claims = 1000000ull;
      uint64_t charlie_recent_claims = 1000000ull;
      time_point_sec last_claims_update = db->head_block_time();

      db_plugin->debug_update( [=]( database& db )
      {
         auto alice_vests = db.create_vesting( db.get_account( "alice" ), asset( 100000, alice_symbol ), false );
         auto bob_vests = db.create_vesting( db.get_account( "bob" ), asset( 100000, bob_symbol ), false );
         auto now = db.head_block_time();

         db.modify( db.get< xtt_token_object, by_symbol >( alice_symbol ), [=]( xtt_token_object& xtt )
         {
            xtt.phase = xtt_phase::launch_success;
            xtt.current_supply = 100000;
            xtt.total_vesting_shares = alice_vests.amount;
            xtt.total_vesting_fund_xtt = 100000;
            xtt.votes_per_regeneration_period = 50;
            xtt.vote_regeneration_period_seconds = 5*24*60*60;
            xtt.recent_claims = uint128_t( 0, alice_recent_claims );
            xtt.last_reward_update = now;
         });

         db.modify( db.get< xtt_token_object, by_symbol >( bob_symbol ), [=]( xtt_token_object& xtt )
         {
            xtt.phase = xtt_phase::launch_success;
            xtt.current_supply = 100000;
            xtt.total_vesting_shares = bob_vests.amount;
            xtt.total_vesting_fund_xtt = 100000;
            xtt.votes_per_regeneration_period = 50;
            xtt.vote_regeneration_period_seconds = 5*24*60*60;
            xtt.recent_claims = uint128_t( 0, bob_recent_claims );
            xtt.last_reward_update = now;
            xtt.author_reward_curve = curve_id::linear;
         });

         db.modify( db.get< xtt_token_object, by_symbol >( charlie_symbol ), [=]( xtt_token_object& xtt )
         {
            xtt.phase = xtt_phase::launch_success;
            xtt.current_supply = 0;
            xtt.total_vesting_shares = 0;
            xtt.total_vesting_fund_xtt = 0;
            xtt.votes_per_regeneration_period = 50;
            xtt.vote_regeneration_period_seconds = 5*24*60*60;
            xtt.recent_claims = uint128_t( 0, charlie_recent_claims );
            xtt.last_reward_update = now;
         });
      });
      generate_block();

      comment_operation comment;
      vote2_operation vote;
      signed_transaction tx;

      comment.author = "alice";
      comment.permlink = "test";
      comment.parent_permlink = "test";
      comment.title = "foo";
      comment.body = "bar";
      allowed_vote_assets ava;
      ava.votable_assets[ alice_symbol ] = votable_asset_options();
      ava.votable_assets[ bob_symbol ] = votable_asset_options();
      comment_options_operation comment_opts;
      comment_opts.author = "alice";
      comment_opts.permlink = "test";
      comment_opts.extensions.insert( ava );
      vote.voter = "alice";
      vote.author = "alice";
      vote.permlink = "test";
      vote.rshares[ XGT_SYMBOL ] = 10000 + XGT_VOTE_DUST_THRESHOLD;
      vote.rshares[ alice_symbol ] = -5000 - XGT_VOTE_DUST_THRESHOLD;
      tx.operations.push_back( comment );
      tx.operations.push_back( comment_opts );
      tx.operations.push_back( vote );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );

      auto alice_vshares = util::evaluate_reward_curve( db->get_comment( "alice", string( "test" ) ).net_rshares.value,
         db->get< reward_fund_object, by_name >( XGT_POST_REWARD_FUND_NAME ).author_reward_curve,
         db->get< reward_fund_object, by_name >( XGT_POST_REWARD_FUND_NAME ).content_constant );

      generate_blocks( 5 );

      comment.author = "bob";
      ava.votable_assets.clear();
      ava.votable_assets[ bob_symbol ] = votable_asset_options();
      comment_opts.author = "bob";
      comment_opts.extensions.clear();
      comment_opts.extensions.insert( ava );
      vote.voter = "bob";
      vote.author = "bob";
      vote.rshares.clear();
      vote.rshares[ XGT_SYMBOL ] = 10000 + XGT_VOTE_DUST_THRESHOLD;
      vote.rshares[ bob_symbol ] = 5000 + XGT_VOTE_DUST_THRESHOLD;
      tx.clear();
      tx.operations.push_back( comment );
      tx.operations.push_back( comment_opts );
      tx.operations.push_back( vote );
      sign( tx, bob_private_key );
      db->push_transaction( tx, 0 );

      generate_blocks( db->get_comment( "alice", string( "test" ) ).cashout_time );

      {
         const auto& post_rf = db->get< reward_fund_object, by_name >( XGT_POST_REWARD_FUND_NAME );
         BOOST_REQUIRE( post_rf.recent_claims == alice_vshares );

         fc::microseconds decay_time = XGT_RECENT_RSHARES_DECAY_TIME_HF19;
         const auto& alice_rf = db->get< xtt_token_object, by_symbol >( alice_symbol );
         alice_recent_claims -= ( ( db->head_block_time() - last_claims_update ).to_seconds() * alice_recent_claims ) / decay_time.to_seconds();

         BOOST_REQUIRE( alice_rf.recent_claims.to_uint64() == alice_recent_claims );
         BOOST_REQUIRE( alice_rf.last_reward_update == db->head_block_time() );

         const auto& bob_rf = db->get< xtt_token_object, by_symbol >( bob_symbol );
         BOOST_REQUIRE( bob_rf.recent_claims.to_uint64() == bob_recent_claims );
         BOOST_REQUIRE( bob_rf.last_reward_update == last_claims_update );

         const auto& charlie_rf = db->get< xtt_token_object, by_symbol >( charlie_symbol );
         BOOST_REQUIRE( charlie_rf.recent_claims.to_uint64() == charlie_recent_claims );
         BOOST_REQUIRE( charlie_rf.last_reward_update == last_claims_update );

         validate_database();
      }

      auto bob_cashout_time = db->get_comment( "bob", string( "test" ) ).cashout_time;
      auto bob_vshares = util::evaluate_reward_curve( db->get_comment( "bob", string( "test" ) ).net_rshares.value,
         db->get< reward_fund_object, by_name >( XGT_POST_REWARD_FUND_NAME ).author_reward_curve,
         db->get< reward_fund_object, by_name >( XGT_POST_REWARD_FUND_NAME ).content_constant );

      generate_block();

      while( db->head_block_time() < bob_cashout_time )
      {
         alice_vshares -= ( alice_vshares * XGT_BLOCK_INTERVAL ) / XGT_RECENT_RSHARES_DECAY_TIME_HF19.to_seconds();
         const auto& post_rf = db->get< reward_fund_object, by_name >( XGT_POST_REWARD_FUND_NAME );

         BOOST_REQUIRE( post_rf.recent_claims == alice_vshares );

         generate_block();
      }

      {
         alice_vshares -= ( alice_vshares * XGT_BLOCK_INTERVAL ) / XGT_RECENT_RSHARES_DECAY_TIME_HF19.to_seconds();
         const auto& post_rf = db->get< reward_fund_object, by_name >( XGT_POST_REWARD_FUND_NAME );

         BOOST_REQUIRE( post_rf.recent_claims == alice_vshares + bob_vshares );

         const auto& alice_rf = db->get< xtt_token_object, by_symbol >( alice_symbol );
         BOOST_REQUIRE( alice_rf.recent_claims.to_uint64() == alice_recent_claims );

         const auto& bob_rf = db->get< xtt_token_object, by_symbol >( bob_symbol );
         fc::microseconds decay_time = XGT_RECENT_RSHARES_DECAY_TIME_HF19;
         bob_recent_claims -= ( ( db->head_block_time() - last_claims_update ).to_seconds() * bob_recent_claims ) / decay_time.to_seconds();
         bob_recent_claims += util::evaluate_reward_curve(
            vote.rshares[ bob_symbol ] - XGT_VOTE_DUST_THRESHOLD,
            bob_rf.author_reward_curve,
            bob_rf.content_constant ).to_uint64();

         BOOST_REQUIRE( bob_rf.recent_claims.to_uint64() == bob_recent_claims );
         BOOST_REQUIRE( bob_rf.last_reward_update == db->head_block_time() );

         const auto& charlie_rf = db->get< xtt_token_object, by_symbol >( charlie_symbol );
         BOOST_REQUIRE( charlie_rf.recent_claims.to_uint64() == charlie_recent_claims );
         BOOST_REQUIRE( charlie_rf.last_reward_update == last_claims_update );

         validate_database();
      }
   }
   FC_LOG_AND_RETHROW()
}*/
/*
BOOST_AUTO_TEST_CASE( xtt_rewards )
{
   try
   {
      ACTORS( (alice)(bob)(charlie)(dave) )
      fund( "alice", 100000 );
      vest( "alice", 100000 );
      fund( "bob", 100000 );
      vest( "bob", 100000 );
      fund( "charlie", 100000 );
      vest( "charlie", 100000 );
      fund( "dave", 100000 );
      vest( "dave", 100000 );

      auto alice_symbol = create_xtt( "alice", alice_private_key, 3 );

      set_price_feed( price( ASSET( "1.000 TBD" ), ASSET( "1.000 TESTS" ) ) );
      generate_block();

      uint64_t recent_claims = 10000000000ull;

      db_plugin->debug_update( [=]( database& db )
      {
         auto alice_xtt_vests = db.create_vesting( db.get_account( "alice" ), asset( 1000000, alice_symbol ), false );
         alice_xtt_vests     += db.create_vesting( db.get_account( "bob" ), asset( 1000000, alice_symbol ), false );
         alice_xtt_vests     += db.create_vesting( db.get_account( "charlie" ), asset( 1000000, alice_symbol ), false );
         alice_xtt_vests     += db.create_vesting( db.get_account( "dave" ), asset( 1000000, alice_symbol ), false );
         auto now = db.head_block_time();

         db.modify( db.get< xtt_token_object, by_symbol >( alice_symbol ), [&]( xtt_token_object& xtt )
         {
            xtt.phase = xtt_phase::launch_success;
            xtt.current_supply = 4000000;
            xtt.total_vesting_shares = alice_xtt_vests.amount;
            xtt.total_vesting_fund_xtt = 4000000;
            xtt.votes_per_regeneration_period = 50;
            xtt.vote_regeneration_period_seconds = 5*24*60*60;
            xtt.recent_claims = uint128_t( 0, recent_claims );
            xtt.author_reward_curve = curve_id::convergent_linear;
            xtt.curation_reward_curve = convergent_square_root;
            xtt.content_constant = XGT_CONTENT_CONSTANT_HF21;
            xtt.percent_curation_rewards = 50 * XGT_1_PERCENT;
            xtt.last_reward_update = now;
         });

         db.modify( db.get( reward_fund_id_type() ), [&]( reward_fund_object& rfo )
         {
            rfo.recent_claims = uint128_t( 0, recent_claims );
            rfo.last_update = now;
         });
      });
      generate_block();

      comment_operation comment;
      comment.author = "alice";
      comment.permlink = "test";
      comment.parent_permlink = "test";
      comment.title = "foo";
      comment.body = "bar";
      allowed_vote_assets ava;
      ava.votable_assets[ alice_symbol ] = votable_asset_options();
      comment_options_operation comment_opts;
      comment_opts.author = "alice";
      comment_opts.permlink = "test";
      comment_opts.percent_xgt_dollars = 0;
      comment_opts.extensions.insert( ava );
      vote2_operation vote;
      vote.voter = "alice";
      vote.author = comment.author;
      vote.permlink = comment.permlink;
      vote.rshares[ XGT_SYMBOL ] = 100ull * XGT_VOTE_DUST_THRESHOLD;
      vote.rshares[ alice_symbol ] = vote.rshares[ XGT_SYMBOL ];

      signed_transaction tx;
      tx.operations.push_back( comment );
      tx.operations.push_back( comment_opts );
      tx.operations.push_back( vote );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );

      comment.author = "bob";
      comment_opts.author = "bob";
      vote.voter = "bob";
      vote.author = comment.author;
      tx.clear();
      tx.operations.push_back( comment );
      tx.operations.push_back( comment_opts );
      tx.operations.push_back( vote );
      sign( tx, bob_private_key );
      db->push_transaction( tx, 0 );

      generate_blocks( db->head_block_time() + ( XGT_REVERSE_AUCTION_WINDOW_SECONDS_HF21 / 2 ), true );

      tx.clear();
      vote.voter = "bob";
      vote.rshares[ XGT_SYMBOL ] = 50ull * XGT_VOTE_DUST_THRESHOLD;
      vote.rshares[ alice_symbol ] = vote.rshares[ XGT_SYMBOL ];
      tx.operations.push_back( vote );
      sign( tx, bob_private_key );
      db->push_transaction( tx, 0 );

      generate_blocks( db->head_block_time() + XGT_REVERSE_AUCTION_WINDOW_SECONDS_HF21, true );

      tx.clear();
      vote.voter = "charlie";
      vote.rshares[ XGT_SYMBOL ] = 50ull * XGT_VOTE_DUST_THRESHOLD;
      vote.rshares[ alice_symbol ] = vote.rshares[ XGT_SYMBOL ];
      tx.operations.push_back( vote );
      sign( tx, charlie_private_key );
      db->push_transaction( tx, 0 );

      generate_blocks( db->get_comment( comment.author, comment.permlink ).cashout_time - ( XGT_UPVOTE_LOCKOUT_SECONDS / 2 ), true );

      tx.clear();
      vote.voter = "dave";
      vote.author = "alice";
      vote.rshares[ XGT_SYMBOL ] = 100ull * XGT_VOTE_DUST_THRESHOLD;
      vote.rshares[ alice_symbol ] = vote.rshares[ XGT_SYMBOL ];
      tx.operations.push_back( vote );
      sign( tx, dave_private_key );
      db->push_transaction( tx, 0 );

      generate_blocks( db->get_comment( comment.author, comment.permlink ).cashout_time - XGT_BLOCK_INTERVAL, true );

      share_type reward_fund = 10000000;

      db_plugin->debug_update( [=]( database& db )
      {
         auto now = db.head_block_time();

         db.modify( db.get< xtt_token_object, by_symbol >( alice_symbol ), [=]( xtt_token_object& xtt )
         {
            xtt.recent_claims = uint128_t( 0, recent_claims );
            xtt.reward_balance = asset( reward_fund, alice_symbol );
            xtt.current_supply += reward_fund;
            xtt.last_reward_update = now;
         });

         share_type reward_delta = 0;

         db.modify( db.get( reward_fund_id_type() ), [&]( reward_fund_object& rfo )
         {
            reward_delta = reward_fund - rfo.reward_balance.amount - 60; //60 adjusts for inflation
            rfo.reward_balance += asset( reward_delta, XGT_SYMBOL );
            rfo.recent_claims = uint128_t( 0, recent_claims );
            rfo.last_update = now;
         });

         db.modify( db.get_dynamic_global_properties(), [&]( dynamic_global_property_object& gpo )
         {
            gpo.current_supply += asset( reward_delta, XGT_SYMBOL );
         });
      });

      generate_block();

      const auto& rf = db->get( reward_fund_id_type() );
      const auto& alice_xtt = db->get< xtt_token_object, by_symbol >( alice_symbol );

      BOOST_REQUIRE( rf.recent_claims == alice_xtt.recent_claims );
      BOOST_REQUIRE( rf.reward_balance.amount == alice_xtt.reward_balance.amount );

      const auto& alice_xtt_balance = db->get< account_rewards_balance_object, by_name_liquid_symbol >( boost::make_tuple( "alice", alice_symbol ) );
      const auto& alice_reward_balance = db->get_account( "alice" );
      BOOST_REQUIRE( alice_reward_balance.reward_vesting_xgt.amount == alice_xtt_balance.pending_vesting_value.amount );

      const auto& bob_xtt_balance = db->get< account_rewards_balance_object, by_name_liquid_symbol >( boost::make_tuple( "bob", alice_symbol ) );
      const auto& bob_reward_balance = db->get_account( "bob" );
      BOOST_REQUIRE( bob_reward_balance.reward_vesting_xgt.amount == bob_xtt_balance.pending_vesting_value.amount );

      const auto& charlie_xtt_balance = db->get< account_rewards_balance_object, by_name_liquid_symbol >( boost::make_tuple( "charlie", alice_symbol ) );
      const auto& charlie_reward_balance = db->get_account( "charlie" );
      BOOST_REQUIRE( charlie_reward_balance.reward_vesting_xgt.amount == charlie_xtt_balance.pending_vesting_value.amount );

      const auto& dave_xtt_balance = db->get< account_rewards_balance_object, by_name_liquid_symbol >( boost::make_tuple( "dave", alice_symbol ) );
      const auto& dave_reward_balance = db->get_account( "dave" );
      BOOST_REQUIRE( dave_reward_balance.reward_vesting_xgt.amount == dave_xtt_balance.pending_vesting_value.amount );

      validate_database();
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( xtt_token_emissions )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing XTT token emissions" );
      ACTORS( (creator)(alice)(bob)(charlie)(dan)(elaine)(fred)(george)(henry) )

      vest( XGT_INIT_MINER_NAME, "creator", ASSET( "100000.000 TESTS" ) );

      generate_block();

      auto alices_balance    = asset( 5000000, XGT_SYMBOL );
      auto bobs_balance      = asset( 25000000, XGT_SYMBOL );
      auto charlies_balance  = asset( 10000000, XGT_SYMBOL );
      auto dans_balance      = asset( 25000000, XGT_SYMBOL );
      auto elaines_balance   = asset( 60000000, XGT_SYMBOL );
      auto freds_balance     = asset( 0, XGT_SYMBOL );
      auto georges_balance   = asset( 0, XGT_SYMBOL );
      auto henrys_balance    = asset( 0, XGT_SYMBOL );

      std::map< std ::string, std::tuple< share_type, fc::ecc::private_key > > contributor_contributions {
         { "alice",   { alices_balance.amount,   alice_private_key   } },
         { "bob",     { bobs_balance.amount,     bob_private_key     } },
         { "charlie", { charlies_balance.amount, charlie_private_key } },
         { "dan",     { dans_balance.amount,     dan_private_key     } },
         { "elaine",  { elaines_balance.amount,  elaine_private_key  } },
         { "fred",    { freds_balance.amount,    fred_private_key    } },
         { "george",  { georges_balance.amount,  george_private_key  } },
         { "henry",   { henrys_balance.amount,   henry_private_key   } }
      };

      for ( auto& e : contributor_contributions )
      {
         FUND( e.first, asset( std::get< 0 >( e.second ), XGT_SYMBOL ) );
      }

      generate_block();

      BOOST_TEST_MESSAGE( " --- XTT creation" );
      auto symbol = create_xtt( "creator", creator_private_key, 3 );
      const auto& token = db->get< xtt_token_object, by_symbol >( symbol );


      signed_transaction tx;
      BOOST_TEST_MESSAGE( " --- XTT setup emissions" );
      xtt_setup_emissions_operation emissions_op;
      emissions_op.control_account = "creator";
      emissions_op.emissions_unit.token_unit[ XTT_DESTINATION_REWARDS ]      = 2;
      emissions_op.emissions_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] = 2;
      emissions_op.emissions_unit.token_unit[ XTT_DESTINATION_VESTING ]      = 2;
      emissions_op.emissions_unit.token_unit[ "george" ]                     = 1;
      emissions_op.interval_seconds = XTT_EMISSION_MIN_INTERVAL_SECONDS;
      emissions_op.emission_count   = 25;
      emissions_op.symbol = symbol;
      emissions_op.schedule_time  = db->head_block_time() + ( XGT_BLOCK_INTERVAL * 10 );
      emissions_op.lep_time       = emissions_op.schedule_time + ( XGT_BLOCK_INTERVAL * XGT_BLOCKS_PER_DAY * 2 );
      emissions_op.rep_time       = emissions_op.lep_time + ( XGT_BLOCK_INTERVAL * XGT_BLOCKS_PER_DAY * 2 );
      emissions_op.lep_abs_amount = 20000000;
      emissions_op.rep_abs_amount = 40000000;
      emissions_op.lep_rel_amount_numerator = 1;
      emissions_op.rep_rel_amount_numerator = 2;
      emissions_op.rel_amount_denom_bits    = 7;
      emissions_op.floor_emissions = false;

      tx.operations.push_back( emissions_op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, creator_private_key );
      db->push_transaction( tx, 0 );
      tx.operations.clear();
      tx.signatures.clear();

      xtt_setup_emissions_operation emissions_op2;
      emissions_op2.control_account = "creator";
      emissions_op2.emissions_unit.token_unit[ XTT_DESTINATION_REWARDS ]      = 1;
      emissions_op2.emissions_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] = 1;
      emissions_op2.emissions_unit.token_unit[ XTT_DESTINATION_VESTING ]      = 1;
      emissions_op2.emissions_unit.token_unit[ "george" ]                     = 1;
      emissions_op2.interval_seconds = XTT_EMISSION_MIN_INTERVAL_SECONDS;
      emissions_op2.emission_count   = 25;
      emissions_op2.symbol = symbol;
      emissions_op2.schedule_time  = emissions_op.schedule_time + ( emissions_op.interval_seconds * ( emissions_op.emission_count - 1 ) ) + XTT_EMISSION_MIN_INTERVAL_SECONDS;
      emissions_op2.lep_time       = emissions_op2.schedule_time + ( XGT_BLOCK_INTERVAL * XGT_BLOCKS_PER_DAY * 2 );
      emissions_op2.rep_time       = emissions_op2.lep_time + ( XGT_BLOCK_INTERVAL * XGT_BLOCKS_PER_DAY * 2 );
      emissions_op2.lep_abs_amount = 50000000;
      emissions_op2.rep_abs_amount = 100000000;
      emissions_op2.lep_rel_amount_numerator = 1;
      emissions_op2.rep_rel_amount_numerator = 2;
      emissions_op2.rel_amount_denom_bits    = 10;
      emissions_op2.floor_emissions = false;

      tx.operations.push_back( emissions_op2 );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, creator_private_key );
      db->push_transaction( tx, 0 );
      tx.operations.clear();
      tx.signatures.clear();

      xtt_setup_emissions_operation emissions_op3;
      emissions_op3.control_account = "creator";
      emissions_op3.emissions_unit.token_unit[ XTT_DESTINATION_REWARDS ]      = 1;
      emissions_op3.emissions_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] = 1;
      emissions_op3.emissions_unit.token_unit[ XTT_DESTINATION_VESTING ]      = 1;
      emissions_op3.emissions_unit.token_unit[ "george" ]                     = 1;
      emissions_op3.interval_seconds = XTT_EMISSION_MIN_INTERVAL_SECONDS;
      emissions_op3.emission_count   = XTT_EMIT_INDEFINITELY;
      emissions_op3.symbol = symbol;
      emissions_op3.schedule_time  = emissions_op2.schedule_time + ( emissions_op2.interval_seconds * ( emissions_op2.emission_count - 1 ) ) + XTT_EMISSION_MIN_INTERVAL_SECONDS;
      emissions_op3.lep_time       = emissions_op3.schedule_time;
      emissions_op3.rep_time       = emissions_op3.schedule_time;
      emissions_op3.lep_abs_amount = 100000000;
      emissions_op3.rep_abs_amount = 100000000;
      emissions_op3.lep_rel_amount_numerator = 0;
      emissions_op3.rep_rel_amount_numerator = 0;
      emissions_op3.rel_amount_denom_bits    = 0;
      emissions_op3.floor_emissions = false;

      tx.operations.push_back( emissions_op3 );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, creator_private_key );
      db->push_transaction( tx, 0 );
      tx.operations.clear();
      tx.signatures.clear();

      BOOST_TEST_MESSAGE( " --- XTT setup" );
      xtt_setup_operation setup_op;

      uint64_t contribution_window_blocks = 5;

      setup_op.control_account = "creator";
      setup_op.symbol = symbol;
      setup_op.contribution_begin_time = db->head_block_time() + XGT_BLOCK_INTERVAL;
      setup_op.contribution_end_time   = setup_op.contribution_begin_time + ( XGT_BLOCK_INTERVAL * contribution_window_blocks );
      setup_op.xgt_units_min         = 0;
      setup_op.max_supply              = 22400000000;
      setup_op.max_unit_ratio          = 100;
      setup_op.min_unit_ratio          = 50;
      setup_op.launch_time             = setup_op.contribution_end_time + XGT_BLOCK_INTERVAL;

      xtt_capped_generation_policy capped_generation_policy;
      capped_generation_policy.generation_unit.xgt_unit[ "fred" ] = 3;
      capped_generation_policy.generation_unit.xgt_unit[ "george" ] = 2;

      capped_generation_policy.generation_unit.token_unit[ XTT_DESTINATION_FROM ] = 7;
      capped_generation_policy.generation_unit.token_unit[ "george" ] = 1;
      capped_generation_policy.generation_unit.token_unit[ "henry" ] = 2;

      xtt_setup_ico_tier_operation ico_tier_op1;
      ico_tier_op1.control_account = "creator";
      ico_tier_op1.symbol = symbol;
      ico_tier_op1.generation_policy = capped_generation_policy;
      ico_tier_op1.xgt_units_cap = 100000000;

      xtt_setup_ico_tier_operation ico_tier_op2;
      ico_tier_op2.control_account = "creator";
      ico_tier_op2.symbol = symbol;
      ico_tier_op2.generation_policy = capped_generation_policy;
      ico_tier_op2.xgt_units_cap = 150000000;

      tx.operations.push_back( ico_tier_op1 );
      tx.operations.push_back( ico_tier_op2 );
      tx.operations.push_back( setup_op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, creator_private_key );
      db->push_transaction( tx, 0 );
      tx.operations.clear();
      tx.signatures.clear();

      BOOST_REQUIRE( token.phase == xtt_phase::setup_completed );

      generate_block();

      BOOST_REQUIRE( token.phase == xtt_phase::ico );

      BOOST_TEST_MESSAGE( " --- XTT contributions" );

      uint32_t num_contributions = 0;
      for ( auto& e : contributor_contributions )
      {
         if ( std::get< 0 >( e.second ) == 0 )
            continue;

         xtt_contribute_operation contrib_op;

         contrib_op.symbol = symbol;
         contrib_op.contribution_id = 0;
         contrib_op.contributor = e.first;
         contrib_op.contribution = asset( std::get< 0 >( e.second ), XGT_SYMBOL );

         tx.operations.push_back( contrib_op );
         tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
         sign( tx, std::get< 1 >( e.second ) );
         db->push_transaction( tx, 0 );
         tx.operations.clear();
         tx.signatures.clear();

         generate_block();
         num_contributions++;
      }

      validate_database();

      generate_block();

      BOOST_REQUIRE( token.phase == xtt_phase::launch_success );

      xgt::plugins::rc::rc_plugin_skip_flags rc_skip;
      rc_skip.skip_reject_not_enough_rc = 0;
      rc_skip.skip_deduct_rc = 0;
      rc_skip.skip_negative_rc_balance = 0;
      rc_skip.skip_reject_unknown_delta_vests = 0;
      appbase::app().get_plugin< xgt::plugins::rc::rc_plugin >().set_rc_plugin_skip_flags( rc_skip );

      xgt::plugins::rc::delegate_to_pool_operation del_op;
      custom_json_operation custom_op;

      del_op.from_account = "creator";
      del_op.to_pool = symbol.to_nai_string();
      del_op.amount = asset( db->get_account( "creator" ).vesting_shares.amount / 2, VESTS_SYMBOL );
      custom_op.json = fc::json::to_string( xgt::plugins::rc::rc_plugin_operation( del_op ) );
      custom_op.id = XGT_RC_PLUGIN_NAME;
      custom_op.required_auths.insert( "creator" );

      tx.operations.push_back( custom_op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, creator_private_key );
      db->push_transaction( tx, 0 );
      tx.operations.clear();
      tx.signatures.clear();

      BOOST_TEST_MESSAGE( " --- XTT token emissions" );

      share_type supply = token.current_supply;
      share_type rewards = token.reward_balance.amount;
      share_type market_maker = token.market_maker.token_balance.amount;
      share_type vesting = token.total_vesting_fund_xtt;
      share_type george_share = db->get_balance( db->get_account( "george" ), symbol ).amount;

      auto emission_time = emissions_op.schedule_time;
      generate_blocks( emission_time );
      generate_block();

      auto approximately_equal = []( share_type a, share_type b, uint32_t epsilon = 10 ) { return std::abs( a.value - b.value ) < epsilon; };

      for ( uint32_t i = 0; i <= ( emissions_op.emission_count - 1 ); i++ )
      {
         validate_database();

         uint32_t rel_amount_numerator;
         share_type abs_amount;
         if ( emission_time <= emissions_op.lep_time )
         {
            abs_amount = emissions_op.lep_abs_amount;
            rel_amount_numerator = emissions_op.lep_rel_amount_numerator;
         }
         else if ( emission_time >= emissions_op.rep_time )
         {
            abs_amount = emissions_op.rep_abs_amount;
            rel_amount_numerator = emissions_op.rep_rel_amount_numerator;
         }
         else
         {
            fc::uint128 lep_abs_val{ emissions_op.lep_abs_amount.value },
                        rep_abs_val{ emissions_op.rep_abs_amount.value },
                        lep_rel_num{ emissions_op.lep_rel_amount_numerator    },
                        rep_rel_num{ emissions_op.rep_rel_amount_numerator    };

            uint32_t lep_dist    = emission_time.sec_since_epoch() - emissions_op.lep_time.sec_since_epoch();
            uint32_t rep_dist    = emissions_op.rep_time.sec_since_epoch() - emission_time.sec_since_epoch();
            uint32_t total_dist  = emissions_op.rep_time.sec_since_epoch() - emissions_op.lep_time.sec_since_epoch();
            abs_amount           = ( ( lep_abs_val * lep_dist + rep_abs_val * rep_dist ) / total_dist ).to_int64();
            rel_amount_numerator = ( ( lep_rel_num * lep_dist + rep_rel_num * rep_dist ) / total_dist ).to_uint64();
         }

         share_type rel_amount = ( fc::uint128( supply.value ) * rel_amount_numerator >> emissions_op.rel_amount_denom_bits ).to_int64();

         share_type new_token_supply;
         if ( emissions_op.floor_emissions )
            new_token_supply = std::min( abs_amount, rel_amount );
         else
            new_token_supply = std::max( abs_amount, rel_amount );

         share_type new_rewards = new_token_supply * emissions_op.emissions_unit.token_unit[ XTT_DESTINATION_REWARDS ] / emissions_op.emissions_unit.token_unit_sum();
         share_type new_market_maker = new_token_supply * emissions_op.emissions_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] / emissions_op.emissions_unit.token_unit_sum();
         share_type new_vesting = new_token_supply * emissions_op.emissions_unit.token_unit[ XTT_DESTINATION_VESTING ] / emissions_op.emissions_unit.token_unit_sum();
         share_type new_george = new_token_supply * emissions_op.emissions_unit.token_unit[ "george" ] / emissions_op.emissions_unit.token_unit_sum();

         BOOST_REQUIRE( approximately_equal( new_rewards + new_market_maker + new_vesting + new_george, new_token_supply ) );

         supply += new_token_supply;
         BOOST_REQUIRE( approximately_equal( token.current_supply, supply ) );

         market_maker += new_market_maker;
         BOOST_REQUIRE( approximately_equal( token.market_maker.token_balance.amount, market_maker ) );

         vesting += new_vesting;
         BOOST_REQUIRE( approximately_equal( token.total_vesting_fund_xtt, vesting ) );

         rewards += new_rewards;
         BOOST_REQUIRE( approximately_equal( token.reward_balance.amount, rewards ) );

         george_share += new_george;
         BOOST_REQUIRE( approximately_equal( db->get_balance( db->get_account( "george" ), symbol ).amount, george_share ) );

         // Prevent any sort of drift
         supply = token.current_supply;
         market_maker = token.market_maker.token_balance.amount;
         vesting = token.total_vesting_fund_xtt;
         rewards = token.reward_balance.amount;
         george_share = db->get_balance( db->get_account( "george" ), symbol ).amount;

         emission_time += XTT_EMISSION_MIN_INTERVAL_SECONDS;
         generate_blocks( emission_time );
         generate_block();
      }

      for ( uint32_t i = 0; i <= ( emissions_op2.emission_count - 1 ); i++ )
      {
         validate_database();

         uint32_t rel_amount_numerator;
         share_type abs_amount;
         if ( emission_time <= emissions_op2.lep_time )
         {
            abs_amount = emissions_op2.lep_abs_amount;
            rel_amount_numerator = emissions_op2.lep_rel_amount_numerator;
         }
         else if ( emission_time >= emissions_op2.rep_time )
         {
            abs_amount = emissions_op2.rep_abs_amount;
            rel_amount_numerator = emissions_op2.rep_rel_amount_numerator;
         }
         else
         {
            fc::uint128 lep_abs_val{ emissions_op2.lep_abs_amount.value },
                        rep_abs_val{ emissions_op2.rep_abs_amount.value },
                        lep_rel_num{ emissions_op2.lep_rel_amount_numerator    },
                        rep_rel_num{ emissions_op2.rep_rel_amount_numerator    };

            uint32_t lep_dist    = emission_time.sec_since_epoch() - emissions_op2.lep_time.sec_since_epoch();
            uint32_t rep_dist    = emissions_op2.rep_time.sec_since_epoch() - emission_time.sec_since_epoch();
            uint32_t total_dist  = emissions_op2.rep_time.sec_since_epoch() - emissions_op2.lep_time.sec_since_epoch();
            abs_amount           = ( ( lep_abs_val * lep_dist + rep_abs_val * rep_dist ) / total_dist ).to_int64();
            rel_amount_numerator = ( ( lep_rel_num * lep_dist + rep_rel_num * rep_dist ) / total_dist ).to_uint64();
         }

         share_type rel_amount = ( fc::uint128( supply.value ) * rel_amount_numerator >> emissions_op2.rel_amount_denom_bits ).to_int64();

         share_type new_token_supply;
         if ( emissions_op2.floor_emissions )
            new_token_supply = std::min( abs_amount, rel_amount );
         else
            new_token_supply = std::max( abs_amount, rel_amount );

         share_type new_rewards = new_token_supply * emissions_op2.emissions_unit.token_unit[ XTT_DESTINATION_REWARDS ] / emissions_op2.emissions_unit.token_unit_sum();
         share_type new_market_maker = new_token_supply * emissions_op2.emissions_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] / emissions_op2.emissions_unit.token_unit_sum();
         share_type new_vesting = new_token_supply * emissions_op2.emissions_unit.token_unit[ XTT_DESTINATION_VESTING ] / emissions_op2.emissions_unit.token_unit_sum();
         share_type new_george = new_token_supply * emissions_op2.emissions_unit.token_unit[ "george" ] / emissions_op2.emissions_unit.token_unit_sum();

         BOOST_REQUIRE( approximately_equal( new_rewards + new_market_maker + new_vesting + new_george, new_token_supply ) );

         supply += new_token_supply;
         BOOST_REQUIRE( approximately_equal( token.current_supply, supply ) );

         market_maker += new_market_maker;
         BOOST_REQUIRE( approximately_equal( token.market_maker.token_balance.amount, market_maker ) );

         vesting += new_vesting;
         BOOST_REQUIRE( approximately_equal( token.total_vesting_fund_xtt, vesting ) );

         rewards += new_rewards;
         BOOST_REQUIRE( approximately_equal( token.reward_balance.amount, rewards ) );

         george_share += new_george;
         BOOST_REQUIRE( approximately_equal( db->get_balance( db->get_account( "george" ), symbol ).amount, george_share ) );

         // Prevent any sort of drift
         supply = token.current_supply;
         market_maker = token.market_maker.token_balance.amount;
         vesting = token.total_vesting_fund_xtt;
         rewards = token.reward_balance.amount;
         george_share = db->get_balance( db->get_account( "george" ), symbol ).amount;

         emission_time += XTT_EMISSION_MIN_INTERVAL_SECONDS;
         generate_blocks( emission_time );
         generate_block();
      }

      BOOST_TEST_MESSAGE( " --- XTT token emissions catch-up logic" );

      share_type new_token_supply = emissions_op3.lep_abs_amount;
      share_type new_rewards = new_token_supply * emissions_op3.emissions_unit.token_unit[ XTT_DESTINATION_REWARDS ] / emissions_op3.emissions_unit.token_unit_sum();
      share_type new_market_maker = new_token_supply * emissions_op3.emissions_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] / emissions_op3.emissions_unit.token_unit_sum();
      share_type new_vesting = new_token_supply * emissions_op3.emissions_unit.token_unit[ XTT_DESTINATION_VESTING ] / emissions_op3.emissions_unit.token_unit_sum();
      share_type new_george = new_token_supply * emissions_op3.emissions_unit.token_unit[ "george" ] / emissions_op3.emissions_unit.token_unit_sum();

      BOOST_REQUIRE( approximately_equal( new_rewards + new_market_maker + new_vesting + new_george, new_token_supply ) );

      supply += new_token_supply;
      BOOST_REQUIRE( approximately_equal( token.current_supply, supply ) );

      market_maker += new_market_maker;
      BOOST_REQUIRE( approximately_equal( token.market_maker.token_balance.amount, market_maker ) );

      vesting += new_vesting;
      BOOST_REQUIRE( approximately_equal( token.total_vesting_fund_xtt, vesting ) );

      rewards += new_rewards;
      BOOST_REQUIRE( approximately_equal( token.reward_balance.amount, rewards ) );

      george_share += new_george;
      BOOST_REQUIRE( approximately_equal( db->get_balance( db->get_account( "george" ), symbol ).amount, george_share ) );

      // Prevent any sort of drift
      supply = token.current_supply;
      market_maker = token.market_maker.token_balance.amount;
      vesting = token.total_vesting_fund_xtt;
      rewards = token.reward_balance.amount;
      george_share = db->get_balance( db->get_account( "george" ), symbol ).amount;

      emission_time += XTT_EMISSION_MIN_INTERVAL_SECONDS * 2;
      generate_blocks( emission_time );
      generate_blocks( 3 );

      new_token_supply = ( emissions_op3.lep_abs_amount * 2 );

      new_rewards = new_token_supply * emissions_op3.emissions_unit.token_unit[ XTT_DESTINATION_REWARDS ] / emissions_op3.emissions_unit.token_unit_sum();
      new_market_maker = new_token_supply * emissions_op3.emissions_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] / emissions_op3.emissions_unit.token_unit_sum();
      new_vesting = new_token_supply * emissions_op3.emissions_unit.token_unit[ XTT_DESTINATION_VESTING ] / emissions_op3.emissions_unit.token_unit_sum();
      new_george = new_token_supply * emissions_op3.emissions_unit.token_unit[ "george" ] / emissions_op3.emissions_unit.token_unit_sum();

      BOOST_REQUIRE( approximately_equal( new_rewards + new_market_maker + new_vesting + new_george, new_token_supply ) );

      supply += new_token_supply;
      BOOST_REQUIRE( approximately_equal( token.current_supply, supply ) );

      market_maker += new_market_maker;
      BOOST_REQUIRE( approximately_equal( token.market_maker.token_balance.amount, market_maker ) );

      vesting += new_vesting;
      BOOST_REQUIRE( approximately_equal( token.total_vesting_fund_xtt, vesting ) );

      rewards += new_rewards;
      BOOST_REQUIRE( approximately_equal( token.reward_balance.amount, rewards ) );

      george_share += new_george;
      BOOST_REQUIRE( approximately_equal( db->get_balance( db->get_account( "george" ), symbol ).amount, george_share ) );

      // Prevent any sort of drift
      supply = token.current_supply;
      market_maker = token.market_maker.token_balance.amount;
      vesting = token.total_vesting_fund_xtt;
      rewards = token.reward_balance.amount;
      george_share = db->get_balance( db->get_account( "george" ), symbol ).amount;

      emission_time += XTT_EMISSION_MIN_INTERVAL_SECONDS * 6;
      generate_blocks( emission_time );
      generate_blocks( 11 );

      new_token_supply = emissions_op3.lep_abs_amount * 6;

      new_rewards = new_token_supply * emissions_op3.emissions_unit.token_unit[ XTT_DESTINATION_REWARDS ] / emissions_op3.emissions_unit.token_unit_sum();
      new_market_maker = new_token_supply * emissions_op3.emissions_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] / emissions_op3.emissions_unit.token_unit_sum();
      new_vesting = new_token_supply * emissions_op3.emissions_unit.token_unit[ XTT_DESTINATION_VESTING ] / emissions_op3.emissions_unit.token_unit_sum();
      new_george = new_token_supply * emissions_op3.emissions_unit.token_unit[ "george" ] / emissions_op3.emissions_unit.token_unit_sum();

      BOOST_REQUIRE( approximately_equal( new_rewards + new_market_maker + new_vesting + new_george, new_token_supply ) );

      supply += new_token_supply;
      BOOST_REQUIRE( approximately_equal( token.current_supply, supply ) );

      market_maker += new_market_maker;
      BOOST_REQUIRE( approximately_equal( token.market_maker.token_balance.amount, market_maker ) );

      vesting += new_vesting;
      BOOST_REQUIRE( approximately_equal( token.total_vesting_fund_xtt, vesting ) );

      rewards += new_rewards;
      BOOST_REQUIRE( approximately_equal( token.reward_balance.amount, rewards ) );

      george_share += new_george;
      BOOST_REQUIRE( approximately_equal( db->get_balance( db->get_account( "george" ), symbol ).amount, george_share ) );

      BOOST_REQUIRE( token.current_supply == 22307937127 );

      new_token_supply = token.max_supply - token.current_supply;

      emission_time += XTT_EMISSION_MIN_INTERVAL_SECONDS * 6;
      generate_blocks( emission_time );
      generate_blocks( 11 );

      BOOST_TEST_MESSAGE( " --- XTT token emissions do not emit passed max supply" );

      new_rewards = new_token_supply * emissions_op3.emissions_unit.token_unit[ XTT_DESTINATION_REWARDS ] / emissions_op3.emissions_unit.token_unit_sum();
      new_market_maker = new_token_supply * emissions_op3.emissions_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] / emissions_op3.emissions_unit.token_unit_sum();
      new_vesting = new_token_supply * emissions_op3.emissions_unit.token_unit[ XTT_DESTINATION_VESTING ] / emissions_op3.emissions_unit.token_unit_sum();
      new_george = new_token_supply * emissions_op3.emissions_unit.token_unit[ "george" ] / emissions_op3.emissions_unit.token_unit_sum();

      BOOST_REQUIRE( approximately_equal( new_rewards + new_market_maker + new_vesting + new_george, new_token_supply ) );

      supply += new_token_supply;
      BOOST_REQUIRE( approximately_equal( token.current_supply, supply ) );

      market_maker += new_market_maker;
      BOOST_REQUIRE( approximately_equal( token.market_maker.token_balance.amount, market_maker ) );

      vesting += new_vesting;
      BOOST_REQUIRE( approximately_equal( token.total_vesting_fund_xtt, vesting ) );

      rewards += new_rewards;
      BOOST_REQUIRE( approximately_equal( token.reward_balance.amount, rewards ) );

      george_share += new_george;
      BOOST_REQUIRE( approximately_equal( db->get_balance( db->get_account( "george" ), symbol ).amount, george_share ) );

      BOOST_REQUIRE( token.current_supply == 22399999999 );
      BOOST_REQUIRE( token.max_supply >= token.current_supply );

      validate_database();

      share_type georges_burn = 500000;
      transfer_operation transfer;
      transfer.from = "george";
      transfer.to = XGT_NULL_ACCOUNT;
      transfer.amount = asset( georges_burn, symbol );

      BOOST_TEST_MESSAGE( " --- XTT token burn" );

      tx.clear();
      tx.operations.push_back( transfer );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, george_private_key );
      db->push_transaction( tx, 0 );

      george_share -= georges_burn;
      supply       -= georges_burn;

      BOOST_REQUIRE( approximately_equal( db->get_balance( db->get_account( "george" ), symbol ).amount, george_share ) );
      BOOST_REQUIRE( db->get_balance( XGT_NULL_ACCOUNT, symbol ).amount == 0 );
      BOOST_REQUIRE( approximately_equal( token.current_supply, supply ) );

      validate_database();

      new_token_supply = token.max_supply - token.current_supply;

      emission_time += XTT_EMISSION_MIN_INTERVAL_SECONDS * 6;
      generate_blocks( emission_time );
      generate_blocks( 11 );

      BOOST_TEST_MESSAGE( " --- XTT token emissions re-emit after token burn" );

      new_rewards = new_token_supply * emissions_op3.emissions_unit.token_unit[ XTT_DESTINATION_REWARDS ] / emissions_op3.emissions_unit.token_unit_sum();
      new_market_maker = new_token_supply * emissions_op3.emissions_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] / emissions_op3.emissions_unit.token_unit_sum();
      new_vesting = new_token_supply * emissions_op3.emissions_unit.token_unit[ XTT_DESTINATION_VESTING ] / emissions_op3.emissions_unit.token_unit_sum();
      new_george = new_token_supply * emissions_op3.emissions_unit.token_unit[ "george" ] / emissions_op3.emissions_unit.token_unit_sum();

      BOOST_REQUIRE( approximately_equal( new_rewards + new_market_maker + new_vesting + new_george, new_token_supply ) );

      supply += new_token_supply;
      BOOST_REQUIRE( approximately_equal( token.current_supply, supply ) );

      market_maker += new_market_maker;
      BOOST_REQUIRE( approximately_equal( token.market_maker.token_balance.amount, market_maker ) );

      vesting += new_vesting;
      BOOST_REQUIRE( approximately_equal( token.total_vesting_fund_xtt, vesting ) );

      rewards += new_rewards;
      BOOST_REQUIRE( approximately_equal( token.reward_balance.amount, rewards ) );

      george_share += new_george;
      BOOST_REQUIRE( approximately_equal( db->get_balance( db->get_account( "george" ), symbol ).amount, george_share ) );

      BOOST_REQUIRE( token.current_supply == 22399999999 );
      BOOST_REQUIRE( token.max_supply >= token.current_supply );

      validate_database();
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( xtt_without_ico )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing XTT without ICO" );
      ACTORS( (creator)(alice) )

      generate_block();

      BOOST_TEST_MESSAGE( " --- XTT creation" );
      auto symbol = create_xtt( "creator", creator_private_key, 3 );
      const auto& token = db->get< xtt_token_object, by_symbol >( symbol );

      BOOST_TEST_MESSAGE( " --- XTT setup and token emission" );
      signed_transaction tx;
      xtt_setup_operation setup_op;

      setup_op.control_account         = "creator";
      setup_op.symbol                  = symbol;
      setup_op.contribution_begin_time = db->head_block_time() + XGT_BLOCK_INTERVAL;
      setup_op.contribution_end_time   = setup_op.contribution_begin_time;
      setup_op.xgt_units_min         = 0;
      setup_op.min_unit_ratio          = 50;
      setup_op.max_unit_ratio          = 100;
      setup_op.max_supply              = XGT_MAX_SHARE_SUPPLY;
      setup_op.launch_time             = setup_op.contribution_end_time;

      xtt_setup_emissions_operation token_emission_op;
      token_emission_op.symbol = symbol;
      token_emission_op.control_account = "creator";
      token_emission_op.emissions_unit.token_unit[ "alice" ] = 1;
      token_emission_op.schedule_time = setup_op.launch_time + XGT_BLOCK_INTERVAL;
      token_emission_op.emission_count = 1;
      token_emission_op.interval_seconds = XTT_EMISSION_MIN_INTERVAL_SECONDS;
      token_emission_op.lep_abs_amount = 1000000;
      token_emission_op.rep_abs_amount = 1000000;

      tx.operations.push_back( token_emission_op );
      tx.operations.push_back( setup_op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, creator_private_key );
      db->push_transaction( tx, 0 );
      tx.operations.clear();
      tx.signatures.clear();

      BOOST_REQUIRE( token.phase == xtt_phase::setup_completed );

      generate_blocks( setup_op.launch_time );
      generate_blocks( token_emission_op.schedule_time );

      BOOST_REQUIRE( token.phase == xtt_phase::launch_success );

      generate_block();

      BOOST_TEST_MESSAGE( " --- Checking account balance" );

      BOOST_REQUIRE( db->get_balance( "alice", symbol ).amount == 1000000 );

      validate_database();
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( xtt_action_delay )
{ try {
   [> This test case is a little complicated.
    *
    * If block generation is delayed, there are pending required actions that
    * cascade and are delayed, block generation can fail to include the
    * required automated actions in correct order. This was due to a bug where
    * generate_block was not updating the head block time. When including
    * required actions, it was using the new block time, but when those actions
    * cascade, they would use head_block_time + XGT_BLOCK_INTERVAL.
    *
    * In this case the head block is at time 1 and the only pending required
    * action is an ico_launch_action for XTT1 at time 4.
    *
    * We then push the setup for XTT2 and create a block at time 8. In this
    * time, XTT 1 should complete its ICO, evaulate the ICO, and
    * launch, happening at times 4 and 5.
    *
    * When we push the XTT2 setup ops, we want to push an ico_launch_action for
    * XTT2 at time3, an xtt_ico_evaulation_action at time 8 and finally
    *
    * In the buggy code, block generation does not update the head block time
    * during generation.
    *
    * We evaluate the XTT2 op and push ico_launch_action at time 3.
    *
    * The required actions queue is:
    *
    * xtt_ico_launch_action, XTT2: time 3
    * xtt_ico_evaluation_action, XTT1: time 4
    *
    * When evaluating actions, we will add the following actions in this order:
    *
    * xtt_token_launch_action, XTT1: time 5
    * xtt_ico_evaluation_action, XTT2: time 8
    * xtt_token_launch_action, XTT2: time 9
    *
    * When pushing the block, head_block_time is updated, which floors action
    * execution times to time 9. Because all of the actions are now added 'in
    * the past' they share the same time and so their queue order is different.
    *
    * The added actions to the queue are:
    *
    * xtt_ico_evaluation_action, XTT2: time 9
    * xtt_token_launch_action, XTT2: time 9
    * xtt_token_launch_action, XTT1: time 9
    *
    * These actions do not match the generated block and so the test fails.
    *
    * The fix that causes this test to pass is setting head_block_time in
    * generate_block after transactions have been evaluted. This way actions
    * are queued identically in generate_block and push_block.
    *
    * TL;DR This passes if generate_block and push_block have the same block
    * time mechanics.
    <]
   ACTORS( (alice) );
   generate_block();

   FUND( "alice", asset( 1000000, SBD_SYMBOL ) );
   FUND( "alice", asset( 1000000, XGT_SYMBOL ) );

   example_required_action req_action;

   signed_transaction tx;

   xtt_create_operation create_op;
   create_op.control_account  = "alice";
   create_op.xtt_creation_fee = db->get_dynamic_global_properties().xtt_creation_fee;
   create_op.symbol           = get_new_xtt_symbol( 3, db );
   create_op.precision        = create_op.symbol.decimals();

   xtt_setup_operation setup_op;
   setup_op.control_account         = "alice";
   setup_op.symbol                  = create_op.symbol;
   setup_op.contribution_begin_time = db->head_block_time() + XGT_BLOCK_INTERVAL; // time 1
   setup_op.contribution_end_time   = setup_op.contribution_begin_time + 3 * XGT_BLOCK_INTERVAL; // time 4
   setup_op.xgt_units_min         = 0;
   setup_op.min_unit_ratio          = 50;
   setup_op.max_unit_ratio          = 100;
   setup_op.max_supply              = XGT_MAX_SHARE_SUPPLY;
   setup_op.launch_time             = setup_op.contribution_end_time + XGT_BLOCK_INTERVAL; // time 5

   tx.operations.push_back( create_op );
   tx.operations.push_back( setup_op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   db->push_transaction( tx, 0 );

   generate_block();

   create_op.symbol                 = get_new_xtt_symbol( 6, db );
   create_op.precision              = create_op.symbol.decimals();
   setup_op.symbol                  = create_op.symbol;
   setup_op.contribution_begin_time = db->head_block_time() + 2 * XGT_BLOCK_INTERVAL; // time 3
   setup_op.contribution_end_time   = setup_op.contribution_begin_time + 5 * XGT_BLOCK_INTERVAL; // time 8
   setup_op.launch_time             = setup_op.contribution_end_time + XGT_BLOCK_INTERVAL; // time 9

   tx.clear();
   tx.operations.push_back( create_op );
   tx.operations.push_back( setup_op );
   sign( tx, alice_private_key );
   db->push_transaction( tx, 0 );

   generate_blocks( setup_op.launch_time, true );
} FC_LOG_AND_RETHROW() }
*/
BOOST_AUTO_TEST_SUITE_END()
#endif
