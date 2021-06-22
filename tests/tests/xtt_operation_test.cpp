#include <fc/macros.hpp>

#if defined IS_TEST_NET

#include <boost/test/unit_test.hpp>

#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/protocol/exceptions.hpp>
#include <xgt/protocol/hardfork.hpp>
#include <xgt/protocol/xtt_util.hpp>

#include <xgt/chain/database.hpp>
#include <xgt/chain/database_exceptions.hpp>
#include <xgt/chain/xgt_objects.hpp>
#include <xgt/chain/xtt_objects.hpp>

#include <xgt/chain/util/nai_generator.hpp>
#include <xgt/chain/util/xtt_token.hpp>

#include "../db_fixture/database_fixture.hpp"

#include <fc/uint128.hpp>

using namespace xgt::chain;
using namespace xgt::protocol;
using fc::string;
using fc::uint128_t;
using boost::container::flat_set;

BOOST_FIXTURE_TEST_SUITE( xtt_operation_tests, clean_database_fixture )
/*
BOOST_AUTO_TEST_CASE( xtt_limit_order_create_authorities )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_limit_order_create_authorities" );

      ACTORS( (alice)(bob) )

      limit_order_create2_operation op;

      //Create XTT and give some XTT to creator.
      signed_transaction tx;
      asset_symbol_type alice_symbol = create_xtt( "alice", alice_private_key, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      op.owner = "alice";
      op.amount_to_sell = asset( 1000, XGT_SYMBOL );
      op.exchange_rate  = price( asset( 1000, XGT_SYMBOL ), asset( 1000, alice_symbol ) );
      op.expiration = db->head_block_time() + fc::seconds( XGT_MAX_TIME_UNTIL_EXPIRATION );

      tx.operations.push_back( op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );

      BOOST_TEST_MESSAGE( "--- Test failure when no signature." );
      XGT_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), tx_missing_active_auth );

      BOOST_TEST_MESSAGE( "--- Test success with account signature" );
      sign( tx, alice_private_key );
      db->push_transaction( tx, database::skip_transaction_dupe_check );

      BOOST_TEST_MESSAGE( "--- Test failure with duplicate signature" );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), tx_duplicate_sig );

      BOOST_TEST_MESSAGE( "--- Test failure with additional incorrect signature" );
      tx.signatures.clear();
      sign( tx, alice_private_key );
      sign( tx, bob_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), tx_irrelevant_sig );

      BOOST_TEST_MESSAGE( "--- Test failure with incorrect signature" );
      tx.signatures.clear();
      sign( tx, alice_post_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), tx_missing_active_auth );

      validate_database();
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( xtt_limit_order_create2_authorities )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_limit_order_create2_authorities" );

      ACTORS( (alice)(bob) )

      limit_order_create2_operation op;

      //Create XTT and give some XTT to creator.
      signed_transaction tx;
      asset_symbol_type alice_symbol = create_xtt( "alice", alice_private_key, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      op.owner = "alice";
      op.amount_to_sell = ASSET( "1.000 TESTS" );
      op.exchange_rate = price( ASSET( "1.000 TESTS" ), asset( 1000, alice_symbol ) );
      op.expiration = db->head_block_time() + fc::seconds( XGT_MAX_LIMIT_ORDER_EXPIRATION );

      tx.operations.push_back( op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );

      BOOST_TEST_MESSAGE( "--- Test failure when no signature." );
      XGT_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), tx_missing_active_auth );

      BOOST_TEST_MESSAGE( "--- Test success with account signature" );
      sign( tx, alice_private_key );
      db->push_transaction( tx, database::skip_transaction_dupe_check );

      BOOST_TEST_MESSAGE( "--- Test failure with duplicate signature" );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), tx_duplicate_sig );

      BOOST_TEST_MESSAGE( "--- Test failure with additional incorrect signature" );
      tx.signatures.clear();
      sign( tx, alice_private_key );
      sign( tx, bob_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), tx_irrelevant_sig );

      BOOST_TEST_MESSAGE( "--- Test failure with incorrect signature" );
      tx.signatures.clear();
      sign( tx, alice_post_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), tx_missing_active_auth );

      validate_database();
   }
   FC_LOG_AND_RETHROW()
}

//BOOST_AUTO_TEST_CASE( xtt_limit_order_create_apply )
//{
//   try
//   {
//      BOOST_TEST_MESSAGE( "Testing: limit_order_create_apply" );
//
//      ACTORS( (alice)(bob) )
//
//      //Create XTT and give some XTT to creators.
//      signed_transaction tx;
//      asset_symbol_type alice_symbol = create_xtt( "alice", alice_private_key, 3);
//
//      const wallet_object& alice_account = db->get_account( "alice" );
//      const wallet_object& bob_account = db->get_account( "bob" );
//
//      asset alice_0 = asset( 0, alice_symbol );
//
//      FUND( "bob", 1000000 );
//      convert( "bob", ASSET("1000.000 TESTS" ) );
//      generate_block();
//
//      asset alice_xtt_balance = asset( 1000000, alice_symbol );
//      asset bob_xtt_balance = asset( 1000000, alice_symbol );
//
//      asset alice_balance = alice_account.balance;
//
//      asset bob_balance = bob_account.balance;
//
//      FUND( "alice", alice_xtt_balance );
//      FUND( "bob", bob_xtt_balance );
//
//      tx.operations.clear();
//      tx.signatures.clear();
//
//      const auto& limit_order_idx = db->get_index< limit_order_index >().indices().get< by_account >();
//
//      BOOST_TEST_MESSAGE( "--- Test failure when account does not have required funds" );
//      limit_order_create_operation op;
//
//      op.owner = "bob";
//      op.orderid = 1;
//      op.amount_to_sell = ASSET( "10.000 TESTS" );
//      op.min_to_receive = asset( 10000, alice_symbol );
//      op.fill_or_kill = false;
//      op.expiration = db->head_block_time() + fc::seconds( XGT_MAX_LIMIT_ORDER_EXPIRATION );
//      tx.operations.push_back( op );
//      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
//      sign( tx, bob_private_key );
//      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );
//
//      BOOST_REQUIRE( limit_order_idx.find( boost::make_tuple( "bob", op.orderid ) ) == limit_order_idx.end() );
//      BOOST_REQUIRE( db->get_balance( bob_account, XGT_SYMBOL ).amount.value == bob_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( bob_account, alice_symbol ).amount.value == bob_xtt_balance.amount.value );
//      validate_database();
//
//      BOOST_TEST_MESSAGE( "--- Test failure when amount to receive is 0" );
//
//      op.owner = "alice";
//      op.min_to_receive = alice_0;
//      tx.operations.clear();
//      tx.signatures.clear();
//      tx.operations.push_back( op );
//      sign( tx, alice_private_key );
//      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );
//
//      BOOST_REQUIRE( limit_order_idx.find( boost::make_tuple( "alice", op.orderid ) ) == limit_order_idx.end() );
//      BOOST_REQUIRE( db->get_balance( bob_account, XGT_SYMBOL ).amount.value == bob_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( bob_account, alice_symbol ).amount.value == bob_xtt_balance.amount.value );
//      validate_database();
//
//      BOOST_TEST_MESSAGE( "--- Test failure when amount to sell is 0" );
//
//      op.amount_to_sell = alice_0;
//      op.min_to_receive = ASSET( "10.000 TESTS" ) ;
//      tx.operations.clear();
//      tx.signatures.clear();
//      tx.operations.push_back( op );
//      sign( tx, alice_private_key );
//      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );
//
//      BOOST_REQUIRE( limit_order_idx.find( boost::make_tuple( "alice", op.orderid ) ) == limit_order_idx.end() );
//      BOOST_REQUIRE( db->get_balance( bob_account, XGT_SYMBOL ).amount.value == bob_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
//      validate_database();
//
//      BOOST_TEST_MESSAGE( "--- Test failure when expiration is too long" );
//      op.amount_to_sell = ASSET( "10.000 TESTS" );
//      op.min_to_receive = ASSET( "15.000 TBD" );
//      op.expiration = db->head_block_time() + fc::seconds( XGT_MAX_LIMIT_ORDER_EXPIRATION + 1 );
//      tx.operations.clear();
//      tx.signatures.clear();
//      tx.operations.push_back( op );
//      sign( tx, alice_private_key );
//      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );
//
//      BOOST_TEST_MESSAGE( "--- Test success creating limit order that will not be filled" );
//
//      op.expiration = db->head_block_time() + fc::seconds( XGT_MAX_LIMIT_ORDER_EXPIRATION );
//      op.amount_to_sell = ASSET( "10.000 TESTS" );
//      op.min_to_receive = asset( 15000, alice_symbol );
//      tx.operations.clear();
//      tx.signatures.clear();
//      tx.operations.push_back( op );
//      sign( tx, alice_private_key );
//      db->push_transaction( tx, 0 );
//
//      alice_balance -= ASSET( "10.000 TESTS" );
//
//      auto limit_order = limit_order_idx.find( boost::make_tuple( "alice", op.orderid ) );
//      BOOST_REQUIRE( limit_order != limit_order_idx.end() );
//      BOOST_REQUIRE( limit_order->seller == op.owner );
//      BOOST_REQUIRE( limit_order->orderid == op.orderid );
//      BOOST_REQUIRE( limit_order->for_sale == op.amount_to_sell.amount );
//      BOOST_REQUIRE( limit_order->sell_price == price( op.amount_to_sell / op.min_to_receive ) );
//      BOOST_REQUIRE( limit_order->get_market() == std::make_pair( alice_symbol, XGT_SYMBOL ) );
//      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( alice_account, XGT_SYMBOL ).amount.value == alice_balance.amount.value );
//      validate_database();
//
//      BOOST_TEST_MESSAGE( "--- Test failure creating limit order with duplicate id" );
//
//      op.amount_to_sell = ASSET( "20.000 TESTS" );
//      tx.operations.clear();
//      tx.signatures.clear();
//      tx.operations.push_back( op );
//      sign( tx, alice_private_key );
//      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );
//
//      limit_order = limit_order_idx.find( boost::make_tuple( "alice", op.orderid ) );
//      BOOST_REQUIRE( limit_order != limit_order_idx.end() );
//      BOOST_REQUIRE( limit_order->seller == op.owner );
//      BOOST_REQUIRE( limit_order->orderid == op.orderid );
//      BOOST_REQUIRE( limit_order->for_sale == 10000 );
//      BOOST_REQUIRE( limit_order->sell_price == price( ASSET( "10.000 TESTS" ), op.min_to_receive ) );
//      BOOST_REQUIRE( limit_order->get_market() == std::make_pair( alice_symbol, XGT_SYMBOL ) );
//      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( alice_account, XGT_SYMBOL ).amount.value == alice_balance.amount.value );
//      validate_database();
//
//      BOOST_TEST_MESSAGE( "--- Test sucess killing an order that will not be filled" );
//
//      op.orderid = 2;
//      op.fill_or_kill = true;
//      tx.operations.clear();
//      tx.signatures.clear();
//      tx.operations.push_back( op );
//      sign( tx, alice_private_key );
//      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );
//
//      BOOST_REQUIRE( limit_order_idx.find( boost::make_tuple( "alice", op.orderid ) ) == limit_order_idx.end() );
//      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( alice_account, XGT_SYMBOL ).amount.value == alice_balance.amount.value );
//      validate_database();
//
//      // BOOST_TEST_MESSAGE( "--- Test having a partial match to limit order" );
//      // // Alice has order for 15 XTT at a price of 2:3
//      // // Fill 5 XGT for 7.5 XTT
//
//      op.owner = "bob";
//      op.orderid = 1;
//      op.amount_to_sell = asset (7500, alice_symbol );
//      op.min_to_receive = ASSET( "5.000 TESTS" );
//      op.fill_or_kill = false;
//      tx.operations.clear();
//      tx.signatures.clear();
//      tx.operations.push_back( op );
//      sign( tx, bob_private_key );
//      db->push_transaction( tx, 0 );
//
//      bob_xtt_balance -= asset (7500, alice_symbol );
//      alice_xtt_balance += asset (7500, alice_symbol );
//      bob_balance += ASSET( "5.000 TESTS" );
//
//      auto recent_ops = get_last_operations( 1 );
//      auto fill_order_op = recent_ops[0].get< fill_order_operation >();
//
//      limit_order = limit_order_idx.find( boost::make_tuple( "alice", 1 ) );
//      BOOST_REQUIRE( limit_order != limit_order_idx.end() );
//      BOOST_REQUIRE( limit_order->seller == "alice" );
//      BOOST_REQUIRE( limit_order->orderid == op.orderid );
//      BOOST_REQUIRE( limit_order->for_sale == 5000 );
//      BOOST_REQUIRE( limit_order->sell_price == price( ASSET( "10.000 TESTS" ), asset( 15000, alice_symbol ) ) );
//      BOOST_REQUIRE( limit_order->get_market() == std::make_pair( alice_symbol, XGT_SYMBOL ) );
//      BOOST_REQUIRE( limit_order_idx.find( boost::make_tuple( "bob", op.orderid ) ) == limit_order_idx.end() );
//      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( alice_account, XGT_SYMBOL ).amount.value == alice_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( bob_account, alice_symbol ).amount.value == bob_xtt_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( bob_account, XGT_SYMBOL ).amount.value == bob_balance.amount.value );
//      BOOST_REQUIRE( fill_order_op.open_owner == "alice" );
//      BOOST_REQUIRE( fill_order_op.open_orderid == 1 );
//      BOOST_REQUIRE( fill_order_op.open_pays.amount.value == ASSET( "5.000 TESTS").amount.value );
//      BOOST_REQUIRE( fill_order_op.current_owner == "bob" );
//      BOOST_REQUIRE( fill_order_op.current_orderid == 1 );
//      BOOST_REQUIRE( fill_order_op.current_pays.amount.value == asset(7500, alice_symbol ).amount.value );
//      validate_database();
//
//      BOOST_TEST_MESSAGE( "--- Test filling an existing order fully, but the new order partially" );
//
//      op.amount_to_sell = asset( 15000, alice_symbol );
//      op.min_to_receive = ASSET( "10.000 TESTS" );
//      tx.operations.clear();
//      tx.signatures.clear();
//      tx.operations.push_back( op );
//      sign( tx, bob_private_key );
//      db->push_transaction( tx, 0 );
//
//      bob_xtt_balance -= asset( 15000, alice_symbol );
//      alice_xtt_balance += asset( 7500, alice_symbol );
//      bob_balance += ASSET( "5.000 TESTS" );
//
//      limit_order = limit_order_idx.find( boost::make_tuple( "bob", 1 ) );
//      BOOST_REQUIRE( limit_order != limit_order_idx.end() );
//      BOOST_REQUIRE( limit_order->seller == "bob" );
//      BOOST_REQUIRE( limit_order->orderid == 1 );
//      BOOST_REQUIRE( limit_order->for_sale.value == 7500 );
//      BOOST_REQUIRE( limit_order->sell_price == price( asset( 15000, alice_symbol ), ASSET( "10.000 TESTS" ) ) );
//      BOOST_REQUIRE( limit_order->get_market() == std::make_pair( alice_symbol, XGT_SYMBOL ) );
//      BOOST_REQUIRE( limit_order_idx.find( boost::make_tuple( "alice", 1 ) ) == limit_order_idx.end() );
//      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( alice_account, XGT_SYMBOL ).amount.value == alice_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( bob_account, alice_symbol ).amount.value == bob_xtt_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( bob_account, XGT_SYMBOL ).amount.value == bob_balance.amount.value );
//      validate_database();
//
//      BOOST_TEST_MESSAGE( "--- Test filling an existing order and new order fully" );
//
//      op.owner = "alice";
//      op.orderid = 3;
//      op.amount_to_sell = ASSET( "5.000 TESTS" );
//      op.min_to_receive = asset( 7500, alice_symbol );
//      tx.operations.clear();
//      tx.signatures.clear();
//      tx.operations.push_back( op );
//      sign( tx, alice_private_key );
//      db->push_transaction( tx, 0 );
//
//      alice_balance -= ASSET( "5.000 TESTS" );
//      alice_xtt_balance += asset( 7500, alice_symbol );
//      bob_balance += ASSET( "5.000 TESTS" );
//
//      BOOST_REQUIRE( limit_order_idx.find( boost::make_tuple( "alice", 3 ) ) == limit_order_idx.end() );
//      BOOST_REQUIRE( limit_order_idx.find( boost::make_tuple( "bob", 1 ) ) == limit_order_idx.end() );
//      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( alice_account, XGT_SYMBOL ).amount.value == alice_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( bob_account, alice_symbol ).amount.value == bob_xtt_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( bob_account, XGT_SYMBOL ).amount.value == bob_balance.amount.value );
//      validate_database();
//
//      BOOST_TEST_MESSAGE( "--- Test filling limit order with better order when partial order is better." );
//
//      op.owner = "alice";
//      op.orderid = 4;
//      op.amount_to_sell = ASSET( "10.000 TESTS" );
//      op.min_to_receive = asset( 11000, alice_symbol );
//      tx.operations.clear();
//      tx.signatures.clear();
//      tx.operations.push_back( op );
//      sign( tx, alice_private_key );
//      db->push_transaction( tx, 0 );
//
//      op.owner = "bob";
//      op.orderid = 4;
//      op.amount_to_sell = asset( 12000, alice_symbol );
//      op.min_to_receive = ASSET( "10.000 TESTS" );
//      tx.operations.clear();
//      tx.signatures.clear();
//      tx.operations.push_back( op );
//      sign( tx, bob_private_key );
//      db->push_transaction( tx, 0 );
//
//      alice_balance -= ASSET( "10.000 TESTS" );
//      alice_xtt_balance += asset( 11000, alice_symbol );
//      bob_xtt_balance -= asset( 12000, alice_symbol );
//      bob_balance += ASSET( "10.000 TESTS" );
//
//      limit_order = limit_order_idx.find( boost::make_tuple( "bob", 4 ) );
//      BOOST_REQUIRE( limit_order != limit_order_idx.end() );
//      BOOST_REQUIRE( limit_order_idx.find(boost::make_tuple( "alice", 4 ) ) == limit_order_idx.end() );
//      BOOST_REQUIRE( limit_order->seller == "bob" );
//      BOOST_REQUIRE( limit_order->orderid == 4 );
//      BOOST_REQUIRE( limit_order->for_sale.value == 1000 );
//      BOOST_REQUIRE( limit_order->sell_price == price( asset( 12000, alice_symbol ), ASSET( "10.000 TESTS" ) ) );
//      BOOST_REQUIRE( limit_order->get_market() == std::make_pair( alice_symbol, XGT_SYMBOL ) );
//      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( alice_account, XGT_SYMBOL ).amount.value == alice_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( bob_account, alice_symbol ).amount.value == bob_xtt_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( bob_account, XGT_SYMBOL ).amount.value == bob_balance.amount.value );
//      validate_database();
//
//      limit_order_cancel_operation can;
//      can.owner = "bob";
//      can.orderid = 4;
//      tx.operations.clear();
//      tx.signatures.clear();
//      tx.operations.push_back( can );
//      sign( tx, bob_private_key );
//      db->push_transaction( tx, 0 );
//
//      BOOST_TEST_MESSAGE( "--- Test filling limit order with better order when partial order is worse." );
//
//      //auto gpo = db->get_dynamic_global_properties();
//      //auto start_sbd = gpo.current_sbd_supply;
//
//      op.owner = "alice";
//      op.orderid = 5;
//      op.amount_to_sell = ASSET( "20.000 TESTS" );
//      op.min_to_receive = asset( 22000, alice_symbol );
//      tx.operations.clear();
//      tx.signatures.clear();
//      tx.operations.push_back( op );
//      sign( tx, alice_private_key );
//      db->push_transaction( tx, 0 );
//
//      op.owner = "bob";
//      op.orderid = 5;
//      op.amount_to_sell = asset( 12000, alice_symbol );
//      op.min_to_receive = ASSET( "10.000 TESTS" );
//      tx.operations.clear();
//      tx.signatures.clear();
//      tx.operations.push_back( op );
//      sign( tx, bob_private_key );
//      db->push_transaction( tx, 0 );
//
//      alice_balance -= ASSET( "20.000 TESTS" );
//      alice_xtt_balance += asset( 12000, alice_symbol );
//
//      bob_xtt_balance -= asset( 11000, alice_symbol );
//      bob_balance += ASSET( "10.909 TESTS" );
//
//      limit_order = limit_order_idx.find( boost::make_tuple( "alice", 5 ) );
//      BOOST_REQUIRE( limit_order != limit_order_idx.end() );
//      BOOST_REQUIRE( limit_order_idx.find(boost::make_tuple( "bob", 5 ) ) == limit_order_idx.end() );
//      BOOST_REQUIRE( limit_order->seller == "alice" );
//      BOOST_REQUIRE( limit_order->orderid == 5 );
//      BOOST_REQUIRE( limit_order->for_sale.value == 9091 );
//      BOOST_REQUIRE( limit_order->sell_price == price( ASSET( "20.000 TESTS" ), asset( 22000, alice_symbol ) ) );
//      BOOST_REQUIRE( limit_order->get_market() == std::make_pair( alice_symbol, XGT_SYMBOL ) );
//      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( alice_account, XGT_SYMBOL ).amount.value == alice_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( bob_account, alice_symbol ).amount.value == bob_xtt_balance.amount.value );
//      BOOST_REQUIRE( db->get_balance( bob_account, XGT_SYMBOL ).amount.value == bob_balance.amount.value );
//      validate_database();
//   }
//   FC_LOG_AND_RETHROW()
//}

BOOST_AUTO_TEST_CASE( xtt_limit_order_create_apply )
{
   BOOST_TEST_MESSAGE( "Testing: limit_order_create_apply" );

   ACTORS( (alice) )

   //Create XTT and give some XTT to creators.
   signed_transaction tx;
   asset_symbol_type alice_symbol = create_xtt( "alice", alice_private_key, 3);

   FUND( "alice", 1000000 );
   FUND( "alice", asset( 1000000, alice_symbol ) );
   convert( "alice", ASSET("1000.000 TESTS" ) );
   generate_block();

   BOOST_TEST_MESSAGE( " -- Testing deprecation of 'limit_order_create'" );
   limit_order_create_operation op;

   op.owner = "alice";
   op.orderid = 1;
   op.amount_to_sell = asset( 7500, alice_symbol );
   op.min_to_receive = asset( 5000, XGT_SYMBOL );
   op.fill_or_kill = false;
   tx.operations.clear();
   tx.signatures.clear();
   tx.operations.push_back( op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
}

BOOST_AUTO_TEST_CASE( xtt_limit_order_cancel_authorities )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_limit_order_cancel_authorities" );

      ACTORS( (alice)(bob) )

      //Create XTT and give some XTT to creator.
      signed_transaction tx;
      asset_symbol_type alice_symbol = create_xtt( "alice", alice_private_key, 3 );

      FUND( "alice", asset( 100000, alice_symbol ) );

      tx.operations.clear();
      tx.signatures.clear();

      limit_order_create2_operation c;
      c.owner = "alice";
      c.orderid = 1;
      c.amount_to_sell = asset( 1000, XGT_SYMBOL );
      c.exchange_rate  = price( asset( 1000, XGT_SYMBOL ), asset( 1000, alice_symbol ) );
      c.expiration = db->head_block_time() + fc::seconds( XGT_MAX_LIMIT_ORDER_EXPIRATION );

      tx.operations.push_back( c );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );

      limit_order_cancel_operation op;
      op.owner = "alice";
      op.orderid = 1;

      tx.operations.clear();
      tx.signatures.clear();
      tx.operations.push_back( op );

      BOOST_TEST_MESSAGE( "--- Test failure when no signature." );
      XGT_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), tx_missing_active_auth );

      BOOST_TEST_MESSAGE( "--- Test success with account signature" );
      sign( tx, alice_private_key );
      db->push_transaction( tx, database::skip_transaction_dupe_check );

      BOOST_TEST_MESSAGE( "--- Test failure with duplicate signature" );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), tx_duplicate_sig );

      BOOST_TEST_MESSAGE( "--- Test failure with additional incorrect signature" );
      tx.signatures.clear();
      sign( tx, alice_private_key );
      sign( tx, bob_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), tx_irrelevant_sig );

      BOOST_TEST_MESSAGE( "--- Test failure with incorrect signature" );
      tx.signatures.clear();
      sign( tx, alice_post_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), tx_missing_active_auth );

      validate_database();
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( xtt_limit_order_cancel_apply )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_limit_order_cancel_apply" );

      ACTORS( (alice) )

      //Create XTT and give some XTT to creator.
      signed_transaction tx;
      asset_symbol_type alice_symbol = create_xtt( "alice", alice_private_key, 3 );

      const wallet_object& alice_account = db->get_account( "alice" );

      tx.operations.clear();
      tx.signatures.clear();

      asset alice_xtt_balance = asset( 1000000, alice_symbol );
      asset alice_balance = alice_account.balance;

      FUND( "alice", alice_xtt_balance );

      const auto& limit_order_idx = db->get_index< limit_order_index >().indices().get< by_account >();

      BOOST_TEST_MESSAGE( "--- Test cancel non-existent order" );

      limit_order_cancel_operation op;

      op.owner = "alice";
      op.orderid = 5;
      tx.operations.push_back( op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );

      BOOST_TEST_MESSAGE( "--- Test cancel order" );

      limit_order_create2_operation create;
      create.owner = "alice";
      create.orderid = 5;
      create.amount_to_sell = asset( 5000, XGT_SYMBOL );
      create.exchange_rate  = price( asset( 1000, XGT_SYMBOL ), asset( 1000, alice_symbol ) );
      create.expiration = db->head_block_time() + fc::seconds( XGT_MAX_LIMIT_ORDER_EXPIRATION );
      tx.operations.clear();
      tx.signatures.clear();
      tx.operations.push_back( create );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );

      BOOST_REQUIRE( limit_order_idx.find( boost::make_tuple( "alice", 5 ) ) != limit_order_idx.end() );

      tx.operations.clear();
      tx.signatures.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );

      BOOST_REQUIRE( limit_order_idx.find( boost::make_tuple( "alice", 5 ) ) == limit_order_idx.end() );
      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( alice_account, XGT_SYMBOL ).amount.value == alice_balance.amount.value );
   }
   FC_LOG_AND_RETHROW()
}

[>
BOOST_AUTO_TEST_CASE( xtt_limit_order_create2_apply )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: limit_order_create2_apply" );

      ACTORS( (alice)(bob) )

      //Create XTT and give some XTT to creators.
      signed_transaction tx;
      asset_symbol_type alice_symbol = create_xtt( "alice", alice_private_key, 3);

      const wallet_object& alice_account = db->get_account( "alice" );
      const wallet_object& bob_account = db->get_account( "bob" );

//      asset alice_0 = asset( 0, alice_symbol );

      FUND( "bob", 1000000 );
      convert( "bob", ASSET("1000.000 TESTS" ) );
      generate_block();

      asset alice_xtt_balance = asset( 1000000, alice_symbol );
      asset bob_xtt_balance = asset( 1000000, alice_symbol );

      asset alice_balance = alice_account.balance;

      asset bob_balance = bob_account.balance;

      FUND( "alice", alice_xtt_balance );
      FUND( "bob", bob_xtt_balance );

      tx.operations.clear();
      tx.signatures.clear();

      const auto& limit_order_idx = db->get_index< limit_order_index >().indices().get< by_account >();

      BOOST_TEST_MESSAGE( "--- Test failure when account does not have required funds" );
      limit_order_create2_operation op;

      op.owner = "bob";
      op.orderid = 1;
      op.amount_to_sell = ASSET( "10.000 TESTS" );
      op.exchange_rate = price( ASSET( "1.000 TESTS" ), asset( 1000, alice_symbol ) );
      op.fill_or_kill = false;
      op.expiration = db->head_block_time() + fc::seconds( XGT_MAX_LIMIT_ORDER_EXPIRATION );
      tx.operations.push_back( op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, bob_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );

      BOOST_REQUIRE( limit_order_idx.find( boost::make_tuple( "bob", op.orderid ) ) == limit_order_idx.end() );
      BOOST_REQUIRE( db->get_balance( bob_account, XGT_SYMBOL ).amount.value == bob_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( bob_account, alice_symbol ).amount.value == bob_xtt_balance.amount.value );
      validate_database();

      BOOST_TEST_MESSAGE( "--- Test failure when amount to receive is 0" );

      op.owner = "alice";
      op.exchange_rate.base = ASSET( "0.000 TESTS" );
      op.exchange_rate.quote = asset( 1000, alice_symbol );
      tx.operations.clear();
      tx.signatures.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );

      BOOST_REQUIRE( limit_order_idx.find( boost::make_tuple( "alice", op.orderid ) ) == limit_order_idx.end() );
      BOOST_REQUIRE( db->get_balance( bob_account, XGT_SYMBOL ).amount.value == bob_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( bob_account, alice_symbol ).amount.value == bob_xtt_balance.amount.value );
      validate_database();

      BOOST_TEST_MESSAGE( "--- Test failure when amount to sell is 0" );

      op.amount_to_sell = asset( 0, XGT_SYMBOL );
      op.exchange_rate = price( ASSET( "1.000 TESTS" ), asset( 1000, alice_symbol ) );
      tx.operations.clear();
      tx.signatures.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );

      BOOST_REQUIRE( limit_order_idx.find( boost::make_tuple( "alice", op.orderid ) ) == limit_order_idx.end() );
      BOOST_REQUIRE( db->get_balance( bob_account, XGT_SYMBOL ).amount.value == bob_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
      validate_database();

      BOOST_TEST_MESSAGE( "--- Test failure when expiration is too long" );
      op.amount_to_sell = ASSET( "10.000 TESTS" );
      op.exchange_rate = price( ASSET( "2.000 TESTS" ), ASSET( "3.000 TBD" ) );
      op.expiration = db->head_block_time() + fc::seconds( XGT_MAX_LIMIT_ORDER_EXPIRATION + 1 );
      tx.operations.clear();
      tx.signatures.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );

      BOOST_TEST_MESSAGE( "--- Test success creating limit order that will not be filled" );

      op.expiration = db->head_block_time() + fc::seconds( XGT_MAX_LIMIT_ORDER_EXPIRATION );
      op.amount_to_sell = ASSET( "10.000 TESTS" );
      op.exchange_rate = price( ASSET( "0.666 TESTS" ), asset( 1000, alice_symbol ) );
      tx.operations.clear();
      tx.signatures.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );

      alice_balance -= ASSET( "10.000 TESTS" );

      auto limit_order = limit_order_idx.find( boost::make_tuple( "alice", op.orderid ) );
      BOOST_REQUIRE( limit_order != limit_order_idx.end() );
      BOOST_REQUIRE( limit_order->seller == op.owner );
      BOOST_REQUIRE( limit_order->orderid == op.orderid );
      BOOST_REQUIRE( limit_order->for_sale == op.amount_to_sell.amount );
      BOOST_REQUIRE( limit_order->sell_price == op.exchange_rate );
      BOOST_REQUIRE( limit_order->get_market() == std::make_pair( alice_symbol, XGT_SYMBOL ) );
      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( alice_account, XGT_SYMBOL ).amount.value == alice_balance.amount.value );
      validate_database();

      BOOST_TEST_MESSAGE( "--- Test failure creating limit order with duplicate id" );

      op.amount_to_sell = ASSET( "20.000 TESTS" );
      tx.operations.clear();
      tx.signatures.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );

      limit_order = limit_order_idx.find( boost::make_tuple( "alice", op.orderid ) );
      BOOST_REQUIRE( limit_order != limit_order_idx.end() );
      BOOST_REQUIRE( limit_order->seller == op.owner );
      BOOST_REQUIRE( limit_order->orderid == op.orderid );
      BOOST_REQUIRE( limit_order->for_sale == 10000 );
      BOOST_REQUIRE( limit_order->sell_price == op.exchange_rate );
      BOOST_REQUIRE( limit_order->get_market() == std::make_pair( alice_symbol, XGT_SYMBOL ) );
      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( alice_account, XGT_SYMBOL ).amount.value == alice_balance.amount.value );
      validate_database();

      BOOST_TEST_MESSAGE( "--- Test success killing an order that will not be filled" );

      op.orderid = 2;
      op.fill_or_kill = true;
      tx.operations.clear();
      tx.signatures.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );

      BOOST_REQUIRE( limit_order_idx.find( boost::make_tuple( "alice", op.orderid ) ) == limit_order_idx.end() );
      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( alice_account, XGT_SYMBOL ).amount.value == alice_balance.amount.value );
      validate_database();

      // BOOST_TEST_MESSAGE( "--- Test having a partial match to limit order" );
      // // Alice has order for 15 XTT at a price of 2:3
      // // Fill 5 XGT for 7.5 XTT

      op.owner = "bob";
      op.orderid = 1;
      op.amount_to_sell = asset( 7500, alice_symbol );
      op.exchange_rate = price( ASSET( "0.666 TESTS" ), asset( 1000, alice_symbol ) );
      op.fill_or_kill = false;
      tx.operations.clear();
      tx.signatures.clear();
      tx.operations.push_back( op );
      sign( tx, bob_private_key );
      db->push_transaction( tx, 0 );

      bob_xtt_balance -= asset (7500, alice_symbol );
      alice_xtt_balance += asset (7500, alice_symbol );
      bob_balance += ASSET( "4.995 TESTS" );

      auto recent_ops = get_last_operations( 1 );
      auto fill_order_op = recent_ops[0].get< fill_order_operation >();

      limit_order = limit_order_idx.find( boost::make_tuple( "alice", 1 ) );
      BOOST_REQUIRE( limit_order != limit_order_idx.end() );
      BOOST_REQUIRE( limit_order->seller == "alice" );
      BOOST_REQUIRE( limit_order->orderid == op.orderid );
      BOOST_REQUIRE( limit_order->for_sale == 5005 );
      BOOST_REQUIRE( limit_order->sell_price == price( ASSET( "0.666 TESTS" ), asset( 1000, alice_symbol ) ) );
      BOOST_REQUIRE( limit_order->get_market() == std::make_pair( alice_symbol, XGT_SYMBOL ) );
      BOOST_REQUIRE( limit_order_idx.find( boost::make_tuple( "bob", op.orderid ) ) == limit_order_idx.end() );
      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( alice_account, XGT_SYMBOL ).amount.value == alice_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( bob_account, alice_symbol ).amount.value == bob_xtt_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( bob_account, XGT_SYMBOL ).amount.value == bob_balance.amount.value );
      BOOST_REQUIRE( fill_order_op.open_owner == "alice" );
      BOOST_REQUIRE( fill_order_op.open_orderid == 1 );
      BOOST_REQUIRE( fill_order_op.open_pays.amount.value == ASSET( "4.995 TESTS").amount.value );
      BOOST_REQUIRE( fill_order_op.current_owner == "bob" );
      BOOST_REQUIRE( fill_order_op.current_orderid == 1 );
      BOOST_REQUIRE( fill_order_op.current_pays.amount.value == asset(7500, alice_symbol ).amount.value );
      validate_database();

      BOOST_TEST_MESSAGE( "--- Test filling an existing order fully, but the new order partially" );

      op.amount_to_sell = asset( 15000, alice_symbol );
      op.exchange_rate = price( ASSET( "0.666 TESTS" ), asset( 1000, alice_symbol ) );
      tx.operations.clear();
      tx.signatures.clear();
      tx.operations.push_back( op );
      sign( tx, bob_private_key );
      db->push_transaction( tx, 0 );

      bob_xtt_balance -= asset( 15000, alice_symbol );
      alice_xtt_balance += asset( 7515, alice_symbol );
      bob_balance += ASSET( "5.005 TESTS" );

      limit_order = limit_order_idx.find( boost::make_tuple( "bob", 1 ) );
      BOOST_REQUIRE( limit_order != limit_order_idx.end() );
      BOOST_REQUIRE( limit_order->seller == "bob" );
      BOOST_REQUIRE( limit_order->orderid == 1 );
      BOOST_REQUIRE( limit_order->for_sale.value == 7485 );
      BOOST_REQUIRE( limit_order->sell_price == price( asset( 1000, alice_symbol ), ASSET( "0.666 TESTS" ) ) );
      BOOST_REQUIRE( limit_order->get_market() == std::make_pair( alice_symbol, XGT_SYMBOL ) );
      BOOST_REQUIRE( limit_order_idx.find( boost::make_tuple( "alice", 1 ) ) == limit_order_idx.end() );
      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( alice_account, XGT_SYMBOL ).amount.value == alice_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( bob_account, alice_symbol ).amount.value == bob_xtt_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( bob_account, XGT_SYMBOL ).amount.value == bob_balance.amount.value );
      validate_database();

      BOOST_TEST_MESSAGE( "--- Test filling an existing order and new order fully" );

      op.owner = "alice";
      op.orderid = 3;
      op.amount_to_sell = ASSET( "4.985 TESTS" );
      op.exchange_rate = price( ASSET( "0.666 TESTS" ), asset( 1000, alice_symbol ) );
      tx.operations.clear();
      tx.signatures.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );

      alice_balance -= ASSET( "4.985 TESTS" );
      alice_xtt_balance += asset( 7484, alice_symbol );
      bob_balance += ASSET( "4.985 TESTS" );
      bob_xtt_balance += asset( 1, alice_symbol );

      BOOST_REQUIRE( limit_order_idx.find( boost::make_tuple( "alice", 3 ) ) == limit_order_idx.end() );
      BOOST_REQUIRE( limit_order_idx.find( boost::make_tuple( "bob", 1 ) ) == limit_order_idx.end() );
      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( alice_account, XGT_SYMBOL ).amount.value == alice_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( bob_account, alice_symbol ).amount.value == bob_xtt_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( bob_account, XGT_SYMBOL ).amount.value == bob_balance.amount.value );
      validate_database();

      BOOST_TEST_MESSAGE( "--- Test filling limit order with better order when partial order is better." );

      op.owner = "alice";
      op.orderid = 4;
      op.amount_to_sell = ASSET( "10.000 TESTS" );
      op.exchange_rate = price( ASSET( "1.000 TESTS" ), asset( 1000, alice_symbol ) );
      tx.operations.clear();
      tx.signatures.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );

      op.owner = "bob";
      op.orderid = 4;
      op.amount_to_sell = asset( 12000, alice_symbol );
      op.exchange_rate = price( ASSET( "0.750 TESTS" ), asset( 1000, alice_symbol ) );
      tx.operations.clear();
      tx.signatures.clear();
      tx.operations.push_back( op );
      sign( tx, bob_private_key );
      db->push_transaction( tx, 0 );

      alice_balance -= ASSET( "10.000 TESTS" );
      alice_xtt_balance += asset( 10000, alice_symbol );
      bob_xtt_balance -= asset( 12000, alice_symbol );
      bob_balance += ASSET( "10.000 TESTS" );

      limit_order = limit_order_idx.find( boost::make_tuple( "bob", 4 ) );
      BOOST_REQUIRE( limit_order != limit_order_idx.end() );
      BOOST_REQUIRE( limit_order_idx.find(boost::make_tuple( "alice", 4 ) ) == limit_order_idx.end() );
      BOOST_REQUIRE( limit_order->seller == "bob" );
      BOOST_REQUIRE( limit_order->orderid == 4 );
      BOOST_REQUIRE( limit_order->for_sale.value == 2000 );
      BOOST_REQUIRE( limit_order->sell_price == ~op.exchange_rate );
      BOOST_REQUIRE( limit_order->get_market() == std::make_pair( alice_symbol, XGT_SYMBOL ) );
      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( alice_account, XGT_SYMBOL ).amount.value == alice_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( bob_account, alice_symbol ).amount.value == bob_xtt_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( bob_account, XGT_SYMBOL ).amount.value == bob_balance.amount.value );
      validate_database();

      limit_order_cancel_operation can;
      can.owner = "bob";
      can.orderid = 4;
      tx.operations.clear();
      tx.signatures.clear();
      tx.operations.push_back( can );
      sign( tx, bob_private_key );
      db->push_transaction( tx, 0 );

      BOOST_TEST_MESSAGE( "--- Test filling limit order with better order when partial order is worse." );

      //auto gpo = db->get_dynamic_global_properties();
      //auto start_sbd = gpo.current_sbd_supply;

      op.owner = "alice";
      op.orderid = 5;
      op.amount_to_sell = ASSET( "20.000 TESTS" );
      op.exchange_rate = price( ASSET( "1.000 TESTS" ), asset( 1000, alice_symbol ) );
      tx.operations.clear();
      tx.signatures.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );

      op.owner = "bob";
      op.orderid = 5;
      op.amount_to_sell = asset( 12000, alice_symbol );
      op.exchange_rate = price( ASSET( "0.950 TESTS" ), asset( 1000, alice_symbol ) );
      tx.operations.clear();
      tx.signatures.clear();
      tx.operations.push_back( op );
      sign( tx, bob_private_key );
      db->push_transaction( tx, 0 );

      alice_balance -= ASSET( "20.000 TESTS" );
      alice_xtt_balance += asset( 12000, alice_symbol );

      bob_xtt_balance -= asset( 10000, alice_symbol );
      bob_balance += ASSET( "12.000 TESTS" );

      limit_order = limit_order_idx.find( boost::make_tuple( "alice", 5 ) );
      BOOST_REQUIRE( limit_order != limit_order_idx.end() );
      BOOST_REQUIRE( limit_order_idx.find(boost::make_tuple( "bob", 5 ) ) == limit_order_idx.end() );
      BOOST_REQUIRE( limit_order->seller == "alice" );
      BOOST_REQUIRE( limit_order->orderid == 5 );
      BOOST_REQUIRE( limit_order->for_sale.value == 8000 );
      BOOST_REQUIRE( limit_order->sell_price == price( ASSET( "1.000 TESTS" ), asset( 1000, alice_symbol ) ) );
      BOOST_REQUIRE( limit_order->get_market() == std::make_pair( alice_symbol, XGT_SYMBOL ) );
      BOOST_REQUIRE( db->get_balance( alice_account, alice_symbol ).amount.value == alice_xtt_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( alice_account, XGT_SYMBOL ).amount.value == alice_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( bob_account, alice_symbol ).amount.value == bob_xtt_balance.amount.value );
      BOOST_REQUIRE( db->get_balance( bob_account, XGT_SYMBOL ).amount.value == bob_balance.amount.value );
      validate_database();
   }
   FC_LOG_AND_RETHROW()
}
<]
BOOST_AUTO_TEST_CASE( claim_reward_balance2_validate )
{
   try
   {
      claim_reward_balance2_operation op;
      op.account = "alice";

      ACTORS( (alice) )

      generate_block();

      // Create XTT(s) and continue.
      auto xtts = create_xtt_3("alice", alice_private_key);
      const auto& xtt1 = xtts[0];
      const auto& xtt2 = xtts[1];
      const auto& xtt3 = xtts[2];

      BOOST_TEST_MESSAGE( "Testing empty rewards" );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( "Testing ineffective rewards" );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      // Manually inserted.
      op.reward_tokens.push_back( ASSET( "0.000 TESTS" ) );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.reward_tokens.clear();
      op.reward_tokens.push_back( ASSET( "0.000 TBD" ) );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.reward_tokens.clear();
      op.reward_tokens.push_back( ASSET( "0.000000 VESTS" ) );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.reward_tokens.clear();
      op.reward_tokens.push_back( asset( 0, xtt1 ) );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.reward_tokens.clear();
      op.reward_tokens.push_back( asset( 0, xtt2 ) );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.reward_tokens.clear();
      op.reward_tokens.push_back( asset( 0, xtt3 ) );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.reward_tokens.clear();

      BOOST_TEST_MESSAGE( "Testing single reward claims" );
      op.reward_tokens.push_back( ASSET( "1.000 TESTS" ) );
      op.validate();
      op.reward_tokens.clear();

      op.reward_tokens.push_back( ASSET( "1.000 TBD" ) );
      op.validate();
      op.reward_tokens.clear();

      op.reward_tokens.push_back( ASSET( "1.000000 VESTS" ) );
      op.validate();
      op.reward_tokens.clear();

      op.reward_tokens.push_back( asset( 1, xtt1 ) );
      op.validate();
      op.reward_tokens.clear();

      op.reward_tokens.push_back( asset( 1, xtt2 ) );
      op.validate();
      op.reward_tokens.clear();

      op.reward_tokens.push_back( asset( 1, xtt3 ) );
      op.validate();
      op.reward_tokens.clear();

      BOOST_TEST_MESSAGE( "Testing multiple rewards" );
      op.reward_tokens.push_back( ASSET( "1.000 TBD" ) );
      op.reward_tokens.push_back( ASSET( "1.000 TESTS" ) );
      op.reward_tokens.push_back( ASSET( "1.000000 VESTS" ) );
      op.reward_tokens.push_back( asset( 1, xtt1 ) );
      op.reward_tokens.push_back( asset( 1, xtt2 ) );
      op.reward_tokens.push_back( asset( 1, xtt3 ) );
      op.validate();

      op.reward_tokens.clear();

      BOOST_TEST_MESSAGE( "Testing invalid rewards" );
      op.reward_tokens.push_back( ASSET( "-1.000 TESTS" ) );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.reward_tokens.clear();
      op.reward_tokens.push_back( ASSET( "-1.000 TBD" ) );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.reward_tokens.clear();
      op.reward_tokens.push_back( ASSET( "-1.000000 VESTS" ) );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.reward_tokens.clear();
      op.reward_tokens.push_back( asset( -1, xtt1 ) );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.reward_tokens.clear();
      op.reward_tokens.push_back( asset( -1, xtt2 ) );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.reward_tokens.clear();
      op.reward_tokens.push_back( asset( -1, xtt3 ) );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.reward_tokens.clear();

      BOOST_TEST_MESSAGE( "Testing duplicated reward tokens." );
      op.reward_tokens.push_back( asset( 1, xtt3 ) );
      op.reward_tokens.push_back( asset( 1, xtt3 ) );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.reward_tokens.clear();

      BOOST_TEST_MESSAGE( "Testing inconsistencies of manually inserted reward tokens." );
      op.reward_tokens.push_back( ASSET( "1.000 TESTS" ) );
      op.reward_tokens.push_back( ASSET( "1.000 TBD" ) );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.reward_tokens.push_back( asset( 1, xtt3 ) );
      op.reward_tokens.push_back( asset( 1, xtt1 ) );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.reward_tokens.clear();
      op.reward_tokens.push_back( asset( 1, xtt1 ) );
      op.reward_tokens.push_back( asset( -1, xtt3 ) );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( claim_reward_balance2_authorities )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: decline_voting_rights_authorities" );

      claim_reward_balance2_operation op;
      op.account = "alice";

      flat_set< wallet_name_type > auths;
      flat_set< wallet_name_type > expected;

      op.get_required_recovery_authorities( auths );
      BOOST_REQUIRE( auths == expected );

      op.get_required_money_authorities( auths );
      BOOST_REQUIRE( auths == expected );

      expected.insert( "alice" );
      op.get_required_social_authorities( auths );
      BOOST_REQUIRE( auths == expected );
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( claim_reward_balance2_apply )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: claim_reward_balance2_apply" );
      BOOST_TEST_MESSAGE( "--- Setting up test state" );

      ACTORS( (alice) )
      generate_block();

      auto xtts = create_xtt_3( "alice", alice_private_key );
      const auto& xtt1 = xtts[0];
      const auto& xtt2 = xtts[1];
      const auto& xtt3 = xtts[2];

      FUND_XTT_REWARDS( "alice", asset( 10*std::pow(10, xtt1.decimals()), xtt1 ) );
      FUND_XTT_REWARDS( "alice", asset( 10*std::pow(10, xtt2.decimals()), xtt2 ) );
      FUND_XTT_REWARDS( "alice", asset( 10*std::pow(10, xtt3.decimals()), xtt3 ) );

      db_plugin->debug_update( []( database& db )
      {
         db.modify( db.get_account( "alice" ), []( wallet_object& a )
         {
            a.reward_sbd_balance = ASSET( "10.000 TBD" );
            a.reward_xgt_balance = ASSET( "10.000 TESTS" );
            a.reward_vesting_balance = ASSET( "10.000000 VESTS" );
            a.reward_vesting_xgt = ASSET( "10.000 TESTS" );
         });

         db.modify( db.get_dynamic_global_properties(), []( dynamic_global_property_object& gpo )
         {
            gpo.current_sbd_supply += ASSET( "10.000 TBD" );
            gpo.current_supply += ASSET( "20.000 TESTS" );
            gpo.virtual_supply += ASSET( "20.000 TESTS" );
            gpo.pending_rewarded_vesting_shares += ASSET( "10.000000 VESTS" );
            gpo.pending_rewarded_vesting_xgt += ASSET( "10.000 TESTS" );
         });
      });

      generate_block();
      validate_database();

      auto alice_xgt = db->get_account( "alice" ).balance;
      auto alice_sbd = db->get_account( "alice" ).sbd_balance;
      auto alice_vests = db->get_account( "alice" ).vesting_shares;
      auto alice_xtt1 = db->get_balance( "alice", xtt1 );
      auto alice_xtt2 = db->get_balance( "alice", xtt2 );
      auto alice_xtt3 = db->get_balance( "alice", xtt3 );

      claim_reward_balance2_operation op;
      op.account = "alice";

      BOOST_TEST_MESSAGE( "--- Attempting to claim more than exists in the reward balance." );
      // Legacy symbols
      op.reward_tokens.push_back( ASSET( "0.000 TBD" ) );
      op.reward_tokens.push_back( ASSET( "20.000 TESTS" ) );
      op.reward_tokens.push_back( ASSET( "0.000000 VESTS" ) );
      FAIL_WITH_OP(op, alice_private_key, fc::assert_exception);
      op.reward_tokens.clear();
      // XTTs
      op.reward_tokens.push_back( asset( 0, xtt1 ) );
      op.reward_tokens.push_back( asset( 0, xtt2 ) );
      op.reward_tokens.push_back( asset( 20*std::pow(10, xtt3.decimals()), xtt3 ) );
      FAIL_WITH_OP(op, alice_private_key, fc::assert_exception);
      op.reward_tokens.clear();

      BOOST_TEST_MESSAGE( "--- Claiming a partial reward balance" );
      // Legacy symbols
      asset partial_vests = ASSET( "5.000000 VESTS" );
      op.reward_tokens.clear();
      op.reward_tokens.push_back( partial_vests );
      PUSH_OP(op, alice_private_key);
      BOOST_REQUIRE( db->get_account( "alice" ).balance == alice_xgt + ASSET( "0.000 TESTS" ) );
      BOOST_REQUIRE( db->get_account( "alice" ).reward_xgt_balance == ASSET( "10.000 TESTS" ) );
      BOOST_REQUIRE( db->get_account( "alice" ).sbd_balance == alice_sbd + ASSET( "0.000 TBD" ) );
      BOOST_REQUIRE( db->get_account( "alice" ).reward_sbd_balance == ASSET( "10.000 TBD" ) );
      BOOST_REQUIRE( db->get_account( "alice" ).vesting_shares == alice_vests + partial_vests );
      BOOST_REQUIRE( db->get_account( "alice" ).reward_vesting_balance == ASSET( "5.000000 VESTS" ) );
      BOOST_REQUIRE( db->get_account( "alice" ).reward_vesting_xgt == ASSET( "5.000 TESTS" ) );
      validate_database();
      alice_vests += partial_vests;
      op.reward_tokens.clear();
      // XTTs
      asset partial_xtt2 = asset( 5*std::pow(10, xtt2.decimals()), xtt2 );
      op.reward_tokens.push_back( partial_xtt2 );
      PUSH_OP(op, alice_private_key);
      BOOST_REQUIRE( db->get_balance( "alice", xtt1 ) == alice_xtt1 + asset( 0, xtt1 ) );
      BOOST_REQUIRE( db->get_balance( "alice", xtt2 ) == alice_xtt2 + partial_xtt2 );
      BOOST_REQUIRE( db->get_balance( "alice", xtt3 ) == alice_xtt3 + asset( 0, xtt3 ) );
      validate_database();
      alice_xtt2 += partial_xtt2;
      op.reward_tokens.clear();

      BOOST_TEST_MESSAGE( "--- Claiming the full reward balance" );
      // Legacy symbols
      asset full_xgt = ASSET( "10.000 TESTS" );
      asset full_sbd = ASSET( "10.000 TBD" );
      op.reward_tokens.push_back( full_sbd );
      op.reward_tokens.push_back( full_xgt );
      op.reward_tokens.push_back( partial_vests );
      PUSH_OP(op, alice_private_key);
      BOOST_REQUIRE( db->get_account( "alice" ).balance == alice_xgt + full_xgt );
      BOOST_REQUIRE( db->get_account( "alice" ).reward_xgt_balance == ASSET( "0.000 TESTS" ) );
      BOOST_REQUIRE( db->get_account( "alice" ).sbd_balance == alice_sbd + full_sbd );
      BOOST_REQUIRE( db->get_account( "alice" ).reward_sbd_balance == ASSET( "0.000 TBD" ) );
      BOOST_REQUIRE( db->get_account( "alice" ).vesting_shares == alice_vests + partial_vests );
      BOOST_REQUIRE( db->get_account( "alice" ).reward_vesting_balance == ASSET( "0.000000 VESTS" ) );
      BOOST_REQUIRE( db->get_account( "alice" ).reward_vesting_xgt == ASSET( "0.000 TESTS" ) );
      validate_database();
      op.reward_tokens.clear();
      // XTTs
      asset full_xtt1 = asset( 10*std::pow(10, xtt1.decimals()), xtt1 );
      asset full_xtt3 = asset( 10*std::pow(10, xtt3.decimals()), xtt3 );
      op.reward_tokens.push_back( full_xtt1 );
      op.reward_tokens.push_back( partial_xtt2 );
      op.reward_tokens.push_back( full_xtt3 );
      PUSH_OP(op, alice_private_key);
      BOOST_REQUIRE( db->get_balance( "alice", xtt1 ) == alice_xtt1 + full_xtt1 );
      BOOST_REQUIRE( db->get_balance( "alice", xtt2 ) == alice_xtt2 + partial_xtt2 );
      BOOST_REQUIRE( db->get_balance( "alice", xtt3 ) == alice_xtt3 + full_xtt3 );
      validate_database();
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( xtt_transfer_to_vesting_validate )
{
   BOOST_TEST_MESSAGE( "Testing: transfer_to_vesting_validate with XTT" );

   ACTORS( (alice) )
   generate_block();

   auto token = create_xtt( "alice", alice_private_key, 3 );

   FUND( "alice", asset( 100, token ) );

   transfer_to_vesting_operation op;
   op.from = "alice";
   op.amount = asset( 20, token );
   op.validate();

   BOOST_TEST_MESSAGE( " -- Fail on invalid 'from' account name" );
   op.from = "@@@@@";
   BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );
   op.from = "alice";

   BOOST_TEST_MESSAGE( " -- Fail on invalid 'to' account name" );
   op.to = "@@@@@";
   BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );
   op.to = "";

   BOOST_TEST_MESSAGE( " -- Fail on vesting symbol (instead of liquid)" );
   op.amount = asset( 20, token.get_paired_symbol() );
   BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );
   op.amount = asset( 20, token );

   BOOST_TEST_MESSAGE( " -- Fail on 0 amount" );
   op.amount = asset( 0, token );
   BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );
   op.amount = asset( 20, token );

   BOOST_TEST_MESSAGE( " -- Fail on negative amount" );
   op.amount = asset( -20, token );
   BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );
   op.amount = asset( 20, token );

   op.validate();
}

BOOST_AUTO_TEST_CASE( xtt_transfer_to_vesting_apply )
{
   BOOST_TEST_MESSAGE( "Testing: xtt_transfer_to_vesting_apply" );

   ACTORS( (alice)(bob) )
   generate_block();

   auto symbol = create_xtt( "alice", alice_private_key, 3 );

   FUND( "alice", asset( 10000, symbol ) );

   BOOST_TEST_MESSAGE( " -- Checking initial balances" );
   BOOST_REQUIRE( db->get_balance( "alice", symbol ).amount == 10000 );
   BOOST_REQUIRE( db->get_balance( "alice", symbol.get_paired_symbol() ).amount == 0 );

   const auto& token = db->get< xtt_token_object, by_symbol >( symbol );

   auto xtt_shares      = asset( token.total_vesting_shares, symbol.get_paired_symbol() );
   auto xtt_vests       = asset( token.total_vesting_fund_xtt, symbol );
   auto xtt_share_price = token.get_vesting_share_price();

   BOOST_TEST_MESSAGE( " -- Self transfer to vesting (alice)" );
   transfer_to_vesting_operation op;
   op.from = "alice";
   op.to = "";
   op.amount = asset( 7500, symbol );
   PUSH_OP( op, alice_private_key );

   auto new_vest = op.amount * xtt_share_price;
   xtt_shares += new_vest;
   xtt_vests += op.amount;
   auto alice_xtt_shares = new_vest;

   BOOST_TEST_MESSAGE( " -- Checking balances" );
   BOOST_REQUIRE( db->get_balance( "alice", symbol ) == asset( 2500, symbol ) );
   BOOST_REQUIRE( db->get_balance( "alice", symbol.get_paired_symbol() ) == alice_xtt_shares );
   BOOST_REQUIRE( token.total_vesting_fund_xtt.value == xtt_vests.amount.value );
   BOOST_REQUIRE( token.total_vesting_shares.value == xtt_shares.amount.value );

   validate_database();

   xtt_share_price = token.get_vesting_share_price();

   BOOST_TEST_MESSAGE( " -- Transfer from alice to bob ( liquid -> vests)" );
   op.to = "bob";
   op.amount = asset( 2000, symbol );
   PUSH_OP( op, alice_private_key );

   new_vest = op.amount * xtt_share_price;
   xtt_shares += new_vest;
   xtt_vests += op.amount;
   auto bob_xtt_shares = new_vest;

   BOOST_TEST_MESSAGE( " -- Checking balances" );
   BOOST_REQUIRE( db->get_balance( "alice", symbol ) == asset( 500, symbol ) );
   BOOST_REQUIRE( db->get_balance( "alice", symbol.get_paired_symbol() ) == alice_xtt_shares );
   BOOST_REQUIRE( db->get_balance( "bob", symbol ) == asset( 0, symbol ) );
   BOOST_REQUIRE( db->get_balance( "bob", symbol.get_paired_symbol() ) == bob_xtt_shares );
   BOOST_REQUIRE( token.total_vesting_fund_xtt.value == xtt_vests.amount.value );
   BOOST_REQUIRE( token.total_vesting_shares.value == xtt_shares.amount.value );

   BOOST_TEST_MESSAGE( " -- Transfer from alice to bob ( liquid -> vests ) insufficient funds" );
   op.amount = asset( 501, symbol );
   FAIL_WITH_OP( op, alice_private_key, fc::assert_exception );

   BOOST_TEST_MESSAGE( " -- Checking balances" );
   BOOST_REQUIRE( db->get_balance( "alice", symbol ) == asset( 500, symbol ) );
   BOOST_REQUIRE( db->get_balance( "alice", symbol.get_paired_symbol() ) == alice_xtt_shares );
   BOOST_REQUIRE( db->get_balance( "bob", symbol ) == asset( 0, symbol ) );
   BOOST_REQUIRE( db->get_balance( "bob", symbol.get_paired_symbol() ) == bob_xtt_shares );
   BOOST_REQUIRE( token.total_vesting_fund_xtt.value == xtt_vests.amount.value );
   BOOST_REQUIRE( token.total_vesting_shares.value == xtt_shares.amount.value );

   validate_database();
}

BOOST_AUTO_TEST_CASE( xtt_withdraw_vesting_validate )
{
   BOOST_TEST_MESSAGE( "Testing: withdraw_vesting_validate" );

   ACTORS( (alice) )
   generate_block();

   auto symbol = create_xtt( "alice", alice_private_key, 3 );

   withdraw_vesting_operation op;
   op.account = "alice";
   op.vesting_shares = asset( 10, symbol.get_paired_symbol() );
   op.validate();

   BOOST_TEST_MESSAGE( " -- Testing invalid account name" );
   op.account = "@@@@@";
   BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );
   op.account = "alice";

   BOOST_TEST_MESSAGE( " -- Testing withdrawal of non-vest symbol" );
   op.vesting_shares = asset( 10, symbol );
   BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );
   op.vesting_shares = asset( 10, symbol.get_paired_symbol() );

   op.validate();
}

BOOST_AUTO_TEST_CASE( xtt_withdraw_vesting_apply )
{
   BOOST_TEST_MESSAGE( "Testing: xtt_withdraw_vesting_apply" );

   ACTORS( (alice)(bob)(charlie) )
   generate_block();

   auto symbol = create_xtt( "charlie", charlie_private_key, 3 );

   FUND( XGT_INIT_MINER_NAME, asset( 10000, symbol ) );
   vest( XGT_INIT_MINER_NAME, "alice", asset( 10000, symbol ) );

   BOOST_TEST_MESSAGE( "--- Test failure withdrawing negative XTT VESTS" );

   withdraw_vesting_operation op;
   op.account = "alice";
   op.vesting_shares = asset( -1, symbol.get_paired_symbol() );

   signed_transaction tx;
   tx.operations.push_back( op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );

   BOOST_TEST_MESSAGE( "--- Test withdraw of existing XTT VESTS" );
   op.vesting_shares = asset( db->get_balance( "alice", symbol.get_paired_symbol() ).amount / 2, symbol.get_paired_symbol() );

   auto old_vesting_shares = db->get_balance( "alice", symbol.get_paired_symbol() );

   tx.clear();
   tx.operations.push_back( op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   db->push_transaction( tx, 0 );

   auto key = boost::make_tuple( "alice", symbol );
   const auto* balance_obj = db->find< account_regular_balance_object, by_name_liquid_symbol >( key );

   BOOST_REQUIRE( balance_obj != nullptr );
   BOOST_REQUIRE( db->get_balance( "alice", symbol.get_paired_symbol() ).amount.value == old_vesting_shares.amount.value );
   BOOST_REQUIRE( balance_obj->vesting_withdraw_rate.amount.value == ( old_vesting_shares.amount / ( XGT_VESTING_WITHDRAW_INTERVALS * 2 ) ).value + 1 );
   BOOST_REQUIRE( balance_obj->to_withdraw.value == op.vesting_shares.amount.value );
   BOOST_REQUIRE( balance_obj->next_vesting_withdrawal == db->head_block_time() + XTT_VESTING_WITHDRAW_INTERVAL_SECONDS );
   validate_database();

   BOOST_TEST_MESSAGE( "--- Test changing vesting withdrawal" );
   tx.operations.clear();
   tx.signatures.clear();

   op.vesting_shares = asset( balance_obj->vesting_shares.amount / 3, symbol.get_paired_symbol() );
   tx.operations.push_back( op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   db->push_transaction( tx, 0 );

   BOOST_REQUIRE( balance_obj->vesting_shares.amount.value == old_vesting_shares.amount.value );
   BOOST_REQUIRE( balance_obj->vesting_withdraw_rate.amount.value == ( old_vesting_shares.amount / ( XGT_VESTING_WITHDRAW_INTERVALS * 3 ) ).value + 1 );
   BOOST_REQUIRE( balance_obj->to_withdraw.value == op.vesting_shares.amount.value );
   BOOST_REQUIRE( balance_obj->next_vesting_withdrawal == db->head_block_time() + XTT_VESTING_WITHDRAW_INTERVAL_SECONDS );
   validate_database();

   BOOST_TEST_MESSAGE( "--- Test withdrawing more vests than available" );
   tx.operations.clear();
   tx.signatures.clear();

   op.vesting_shares = asset( balance_obj->vesting_shares.amount * 2, symbol.get_paired_symbol() );
   tx.operations.push_back( op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );

   BOOST_REQUIRE( balance_obj->vesting_shares.amount.value == old_vesting_shares.amount.value );
   BOOST_REQUIRE( balance_obj->vesting_withdraw_rate.amount.value == ( old_vesting_shares.amount / ( XGT_VESTING_WITHDRAW_INTERVALS * 3 ) ).value + 1 );
   BOOST_REQUIRE( balance_obj->next_vesting_withdrawal == db->head_block_time() + XTT_VESTING_WITHDRAW_INTERVAL_SECONDS );
   validate_database();

   BOOST_TEST_MESSAGE( "--- Test withdrawing 0 to reset vesting withdraw" );
   tx.operations.clear();
   tx.signatures.clear();

   op.vesting_shares = asset( 0, symbol.get_paired_symbol() );
   tx.operations.push_back( op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   db->push_transaction( tx, 0 );

   BOOST_REQUIRE( balance_obj->vesting_shares.amount.value == old_vesting_shares.amount.value );
   BOOST_REQUIRE( balance_obj->vesting_withdraw_rate.amount.value == 0 );
   BOOST_REQUIRE( balance_obj->to_withdraw.value == 0 );
   BOOST_REQUIRE( balance_obj->next_vesting_withdrawal == fc::time_point_sec::maximum() );
   validate_database();

   BOOST_TEST_MESSAGE( "--- Test withdrawing with no balance" );
   op.account = "bob";
   op.vesting_shares = db->get_balance( "bob", symbol.get_paired_symbol() );
   tx.clear();
   tx.operations.push_back( op );
   sign( tx, bob_private_key );
   XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
}
*/
/*
BOOST_AUTO_TEST_CASE( xtt_create_validate )
{
   try
   {
      ACTORS( (alice) );

      BOOST_TEST_MESSAGE( " -- A valid xtt_create_operation" );
      xtt_create_operation op;
      op.control_account = "alice";
      op.xtt_creation_fee = db->get_dynamic_global_properties().xtt_creation_fee;
      op.symbol = get_new_xtt_symbol( 3, db );
      op.precision = op.symbol.decimals();
      op.validate();

      BOOST_TEST_MESSAGE( " -- Test invalid control account name" );
      op.control_account = "@@@@@";
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.control_account = "alice";

      // Test invalid creation fees.
      BOOST_TEST_MESSAGE( " -- Invalid negative creation fee" );
      op.xtt_creation_fee.amount = -op.xtt_creation_fee.amount;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Valid maximum XTT creation fee (XGT_MAX_SHARE_SUPPLY)" );
      op.xtt_creation_fee.amount = XGT_MAX_SHARE_SUPPLY;
      op.validate();

      BOOST_TEST_MESSAGE( " -- Invalid XTT creation fee (MAX_SHARE_SUPPLY + 1)" );
      op.xtt_creation_fee.amount++;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Invalid currency for XTT creation fee (VESTS)" );
      op.xtt_creation_fee = ASSET( "1.000000 VESTS" );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.xtt_creation_fee = db->get_dynamic_global_properties().xtt_creation_fee;

      BOOST_TEST_MESSAGE( " -- Invalid XTT creation fee: differing decimals" );
      op.precision = 0;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.precision = op.symbol.decimals();

      // Test symbol
      BOOST_TEST_MESSAGE( " -- Invalid XTT creation symbol: vesting symbol used instead of liquid one" );
      op.symbol = op.symbol.get_paired_symbol();
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Invalid XTT creation symbol: XGT cannot be an XTT" );
      op.symbol = XGT_SYMBOL;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Invalid XTT creation symbol: SBD cannot be an XTT" );
      op.symbol = SBD_SYMBOL;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Invalid XTT creation symbol: VESTS cannot be an XTT" );
      op.symbol = VESTS_SYMBOL;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.symbol = get_new_xtt_symbol( 3, db );

      BOOST_TEST_MESSAGE( " -- Invalid XTT Ticker Symbols" );
      op.desired_ticker = "TeST";
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      op.desired_ticker = "TEST1";
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      op.desired_ticker = "TEST!";
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      op.desired_ticker = XGT_SYMBOL_STR;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      op.desired_ticker = VESTS_SYMBOL_STR;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      op.desired_ticker = SBD_SYMBOL_STR;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.desired_ticker = "TEST";

      // If this fails, it could indicate a test above has failed for the wrong reasons
      op.validate();
   }
   FC_LOG_AND_RETHROW()
}
*/
BOOST_AUTO_TEST_CASE( xtt_create_authorities )
{
   try
   {
      XTT_SYMBOL( alice, 3, db );

      xtt_create_operation op;
      op.control_account = "alice";
      op.symbol = alice_symbol;
      op.xtt_creation_fee = db->get_dynamic_global_properties().xtt_creation_fee;

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

BOOST_AUTO_TEST_CASE( xtt_create_duplicate )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_create_duplicate" );

      ACTORS( (alice) )
      asset_symbol_type alice_symbol = create_xtt( "alice", alice_private_key, 0 );

      // We add the NAI back to the pool to ensure the test does not fail because the NAI is not in the pool
      db->modify( db->get< nai_pool_object >(), [&] ( nai_pool_object& obj )
      {
         obj.nais[ 0 ] = alice_symbol;
      } );

      // Fail on duplicate XTT lookup
      XGT_REQUIRE_THROW( create_xtt_with_nai( "alice", alice_private_key, alice_symbol.to_nai(), alice_symbol.decimals() ), fc::assert_exception)
   }
   FC_LOG_AND_RETHROW();
}

BOOST_AUTO_TEST_CASE( xtt_create_duplicate_differing_decimals )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_create_duplicate_differing_decimals" );

      ACTORS( (alice) )
      asset_symbol_type alice_symbol = create_xtt( "alice", alice_private_key, 3 /* Decimals */ );

      // We add the NAI back to the pool to ensure the test does not fail because the NAI is not in the pool
      db->modify( db->get< nai_pool_object >(), [&] ( nai_pool_object& obj )
      {
         obj.nais[ 0 ] = asset_symbol_type::from_nai( alice_symbol.to_nai(), 0 );
      } );

      // Fail on duplicate XTT lookup
      XGT_REQUIRE_THROW( create_xtt_with_nai( "alice", alice_private_key, alice_symbol.to_nai(), 2 /* Decimals */ ), fc::assert_exception)
   }
   FC_LOG_AND_RETHROW();
}

BOOST_AUTO_TEST_CASE( xtt_create_duplicate_different_users )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_create_duplicate_different_users" );

      ACTORS( (alice)(bob) )
      asset_symbol_type alice_symbol = create_xtt( "alice", alice_private_key, 0 );

      // We add the NAI back to the pool to ensure the test does not fail because the NAI is not in the pool
      db->modify( db->get< nai_pool_object >(), [&] ( nai_pool_object& obj )
      {
         obj.nais[ 0 ] = alice_symbol;
      } );

      // Fail on duplicate XTT lookup
      XGT_REQUIRE_THROW( create_xtt_with_nai( "bob", bob_private_key, alice_symbol.to_nai(), alice_symbol.decimals() ), fc::assert_exception)
   }
   FC_LOG_AND_RETHROW();
}
/*
BOOST_AUTO_TEST_CASE( xtt_create_with_xgt_funds )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_create_with_xgt_funds" );

      // This test expects 1.000 TBD xtt_creation_fee
      db->modify( db->get_dynamic_global_properties(), [&] ( dynamic_global_property_object& dgpo )
      {
         dgpo.xtt_creation_fee = asset( 1000, SBD_SYMBOL );
      } );

      set_price_feed( price( ASSET( "1.000 TBD" ), ASSET( "1.000 TESTS" ) ) );

      ACTORS( (alice) )

      generate_block();

      FUND( "alice", ASSET( "0.999 TESTS" ) );

      xtt_create_operation op;
      op.control_account = "alice";
      op.xtt_creation_fee = ASSET( "1.000 TESTS" );
      op.symbol = get_new_xtt_symbol( 3, db );
      op.precision = op.symbol.decimals();
      op.validate();

      // Fail insufficient funds
      FAIL_WITH_OP( op, alice_private_key, fc::assert_exception );

      BOOST_REQUIRE( util::xtt::find_token( *db, op.symbol, true ) == nullptr );

      FUND( "alice", ASSET( "0.001 TESTS" ) );

      PUSH_OP( op, alice_private_key );

      BOOST_REQUIRE( util::xtt::find_token( *db, op.symbol, true ) != nullptr );
   }
   FC_LOG_AND_RETHROW();
}*/
/*
BOOST_AUTO_TEST_CASE( xtt_create_with_sbd_funds )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_create_with_sbd_funds" );

      // This test expects 1.000 TBD xtt_creation_fee
      db->modify( db->get_dynamic_global_properties(), [&] ( dynamic_global_property_object& dgpo )
      {
         dgpo.xtt_creation_fee = asset( 1000, SBD_SYMBOL );
      } );

      set_price_feed( price( ASSET( "1.000 TBD" ), ASSET( "1.000 TESTS" ) ) );

      ACTORS( (alice) )

      generate_block();

      FUND( "alice", ASSET( "0.999 TBD" ) );

      xtt_create_operation op;
      op.control_account = "alice";
      op.xtt_creation_fee = ASSET( "1.000 TBD" );
      op.symbol = get_new_xtt_symbol( 3, db );
      op.precision = op.symbol.decimals();
      op.validate();

      // Fail insufficient funds
      FAIL_WITH_OP( op, alice_private_key, fc::assert_exception );

      BOOST_REQUIRE( util::xtt::find_token( *db, op.symbol, true ) == nullptr );

      FUND( "alice", ASSET( "0.001 TBD" ) );

      PUSH_OP( op, alice_private_key );

      BOOST_REQUIRE( util::xtt::find_token( *db, op.symbol, true ) != nullptr );
   }
   FC_LOG_AND_RETHROW();
}
*/
BOOST_AUTO_TEST_CASE( xtt_create_with_invalid_nai )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_create_with_invalid_nai" );

      ACTORS( (alice) )

      uint32_t seed = 0;
      asset_symbol_type ast;
      uint32_t collisions = 0;
      do
      {
         BOOST_REQUIRE( collisions < XTT_MAX_NAI_GENERATION_TRIES );
         collisions++;

         ast = util::nai_generator::generate( seed++ );
      }
      while ( db->get< nai_pool_object >().contains( ast ) || util::xtt::find_token( *db, ast, true ) != nullptr );

      // Fail on NAI pool not containing this NAI
      XGT_REQUIRE_THROW( create_xtt_with_nai( "alice", alice_private_key, ast.to_nai(), ast.decimals() ), fc::assert_exception)
   }
   FC_LOG_AND_RETHROW();
}
/*
BOOST_AUTO_TEST_CASE( xtt_creation_fee_test )
{
   try
   {
      ACTORS( (alice) );
      generate_block();

      set_price_feed( price( ASSET( "1.000 TBD" ), ASSET( "2.000 TESTS" ) ) );

      // This ensures that our actual xtt_creation_fee is sane in production (either XGT or SBD)
      const dynamic_global_property_object& dgpo = db->get_dynamic_global_properties();
      FC_ASSERT( dgpo.xtt_creation_fee.symbol == XGT_SYMBOL || dgpo.xtt_creation_fee.symbol == SBD_SYMBOL,
                "Unexpected symbol for the XTT creation fee on the dynamic global properties object: ${s}", ("s", dgpo.xtt_creation_fee.symbol) );

      FC_ASSERT( dgpo.xtt_creation_fee.amount > 0, "Expected positive xtt_creation_fee." );

      for ( int i = 0; i < 2; i++ )
      {
         FUND( "alice", ASSET( "2.000 TESTS" ) );
         FUND( "alice", ASSET( "1.000 TBD" ) );

         // These values should be equivilant as per our price feed and all tests here should work either way
         if ( !i ) // First pass
            db->modify( dgpo, [&] ( dynamic_global_property_object& dgpo )
            {
               dgpo.xtt_creation_fee = asset( 2000, XGT_SYMBOL );
            } );
         else // Second pass
            db->modify( dgpo, [&] ( dynamic_global_property_object& dgpo )
            {
               dgpo.xtt_creation_fee = asset( 1000, SBD_SYMBOL );
            } );

         BOOST_TEST_MESSAGE( " -- Invalid creation fee, 0.001 TESTS short" );
         xtt_create_operation fail_op;
         fail_op.control_account = "alice";
         fail_op.xtt_creation_fee = ASSET( "1.999 TESTS" );
         fail_op.symbol = get_new_xtt_symbol( 3, db );
         fail_op.precision = fail_op.symbol.decimals();
         fail_op.validate();

         // Fail because we are 0.001 TESTS short of the fee
         FAIL_WITH_OP( fail_op, alice_private_key, fc::assert_exception );

         BOOST_TEST_MESSAGE( " -- Invalid creation fee, 0.001 TBD short" );
         xtt_create_operation fail_op2;
         fail_op2.control_account = "alice";
         fail_op2.xtt_creation_fee = ASSET( "0.999 TBD" );
         fail_op2.symbol = get_new_xtt_symbol( 3, db );
         fail_op2.precision = fail_op2.symbol.decimals();
         fail_op2.validate();

         // Fail because we are 0.001 TBD short of the fee
         FAIL_WITH_OP( fail_op2, alice_private_key, fc::assert_exception );

         BOOST_TEST_MESSAGE( " -- Valid creation fee, using XGT" );
         // We should be able to pay with XGT
         xtt_create_operation op;
         op.control_account = "alice";
         op.xtt_creation_fee = ASSET( "2.000 TESTS" );
         op.symbol = get_new_xtt_symbol( 3, db );
         op.precision = op.symbol.decimals();
         op.validate();

         // Succeed because we have paid the equivilant of 1 TBD or 2 TESTS
         PUSH_OP( op, alice_private_key );

         BOOST_TEST_MESSAGE( " -- Valid creation fee, using SBD" );
         // We should be able to pay with SBD
         xtt_create_operation op2;
         op2.control_account = "alice";
         op2.xtt_creation_fee = ASSET( "1.000 TBD" );
         op2.symbol = get_new_xtt_symbol( 3, db );
         op2.precision = op.symbol.decimals();
         op2.validate();

         // Succeed because we have paid the equivilant of 1 TBD or 2 TESTS
         PUSH_OP( op2, alice_private_key );
      }
   }
   FC_LOG_AND_RETHROW()
}*/
/*
BOOST_AUTO_TEST_CASE( xtt_create_reset )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing xtt_create_operation reset" );

      ACTORS( (alice) )
      generate_block();

      set_price_feed( price( ASSET( "1.000 TBD" ), ASSET( "1.000 TESTS" ) ) );
      fund( "alice", ASSET( "100.000 TESTS" ) );

       XTT_SYMBOL( alice, 3, db )

      db_plugin->debug_update( [=]( database& db )
      {
         db.create< xtt_token_object >( [&]( xtt_token_object& o )
         {
            o.control_account = "alice";
            o.liquid_symbol = alice_symbol;
         });
      });

      generate_block();

      signed_transaction tx;
      xtt_setup_emissions_operation op1;
      op1.control_account = "alice";
      op1.symbol = alice_symbol;
      op1.emissions_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] = 10;
      op1.schedule_time = db->head_block_time() + fc::days(30);
      op1.interval_seconds = XTT_EMISSION_MIN_INTERVAL_SECONDS;
      op1.emission_count = 1;
      op1.lep_abs_amount = 0;
      op1.rep_abs_amount = 0;
      op1.lep_rel_amount_numerator = 1;
      op1.rep_rel_amount_numerator = 0;

      xtt_setup_emissions_operation op2;
      op2.control_account = "alice";
      op2.symbol = alice_symbol;
      op2.emissions_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] = 10;
      op2.schedule_time = op1.schedule_time + fc::days( 365 );
      op2.interval_seconds = XTT_EMISSION_MIN_INTERVAL_SECONDS;
      op2.emission_count = 10;
      op2.lep_abs_amount = 0;
      op2.rep_abs_amount = 0;
      op2.lep_rel_amount_numerator = 1;
      op2.rep_rel_amount_numerator = 0;

      xtt_set_runtime_parameters_operation op3;
      xtt_param_windows_v1 windows;
      windows.cashout_window_seconds = 86400 * 4;
      windows.reverse_auction_window_seconds = 60 * 5;
      xtt_param_vote_regeneration_period_seconds_v1 vote_regen;
      vote_regen.vote_regeneration_period_seconds = 86400 * 6;
      vote_regen.votes_per_regeneration_period = 600;
      xtt_param_rewards_v1 rewards;
      rewards.content_constant = uint128_t( uint64_t( 1000000000000ull ) );
      rewards.percent_curation_rewards = 15 * XGT_1_PERCENT;
      rewards.author_reward_curve = curve_id::quadratic;
      rewards.curation_reward_curve = curve_id::linear;
      xtt_param_allow_downvotes downvotes;
      downvotes.value = false;
      op3.runtime_parameters.insert( windows );
      op3.runtime_parameters.insert( vote_regen );
      op3.runtime_parameters.insert( rewards );
      op3.runtime_parameters.insert( downvotes );
      op3.control_account = "alice";
      op3.symbol = alice_symbol;

      xtt_set_setup_parameters_operation op4;
      xtt_param_allow_voting voting;
      voting.value = false;
      op4.setup_parameters.insert( voting );
      op4.control_account = "alice";
      op4.symbol = alice_symbol;

      tx.operations.push_back( op1 );
      tx.operations.push_back( op2 );
      tx.operations.push_back( op3 );
      tx.operations.push_back( op4 );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );

      BOOST_TEST_MESSAGE( "--- Failure when specifying fee" );
      auto alice_prec_4 = asset_symbol_type::from_nai( alice_symbol.to_nai(), 4 );
      xtt_create_operation op;
      op.control_account = "alice";
      op.symbol = alice_prec_4;
      op.xtt_creation_fee = ASSET( "1.000 TESTS" );
      op.precision = 4;
      tx.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );

      BOOST_TEST_MESSAGE( "--- Failure resetting XTT with token emissions" );
      op.xtt_creation_fee = ASSET( "0.000 TBD" );
      tx.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );

      BOOST_TEST_MESSAGE( "--- Failure deleting token emissions in wrong order" );
      op1.remove = true;
      op2.remove = true;
      tx.clear();
      tx.operations.push_back( op1 );
      tx.operations.push_back( op2 );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );

      BOOST_TEST_MESSAGE( "--- Success deleting token emissions" );
      tx.clear();
      tx.operations.push_back( op2 );
      tx.operations.push_back( op1 );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );

      BOOST_TEST_MESSAGE( "--- Success resetting XTT" );
      op.xtt_creation_fee = ASSET( "0.000 TBD" );
      tx.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );

      auto token = db->get< xtt_token_object, by_symbol >( alice_prec_4 );

      BOOST_REQUIRE( token.liquid_symbol == op.symbol );
      BOOST_REQUIRE( token.control_account == "alice" );
      BOOST_REQUIRE( token.allow_voting == true );
      BOOST_REQUIRE( token.cashout_window_seconds == XGT_CASHOUT_WINDOW_SECONDS );
      BOOST_REQUIRE( token.reverse_auction_window_seconds == XGT_REVERSE_AUCTION_WINDOW_SECONDS_HF20 );
      BOOST_REQUIRE( token.vote_regeneration_period_seconds == XGT_VOTING_ENERGY_REGENERATION_SECONDS );
      BOOST_REQUIRE( token.votes_per_regeneration_period == XTT_DEFAULT_VOTES_PER_REGEN_PERIOD );
      BOOST_REQUIRE( token.content_constant == XGT_CONTENT_CONSTANT_HF0 );
      BOOST_REQUIRE( token.percent_curation_rewards == XTT_DEFAULT_PERCENT_CURATION_REWARDS );
      BOOST_REQUIRE( token.author_reward_curve == curve_id::linear );
      BOOST_REQUIRE( token.curation_reward_curve == curve_id::square_root );
      BOOST_REQUIRE( token.allow_downvotes == true );

      const auto& emissions_idx = db->get_index< xtt_token_emissions_index, by_id >();
      BOOST_REQUIRE( emissions_idx.begin() == emissions_idx.end() );

      generate_block();

      BOOST_TEST_MESSAGE( "--- Failure resetting a token that has completed setup" );

      db_plugin->debug_update( [=]( database& db )
      {
         db.modify( db.get< xtt_token_object, by_symbol >( alice_prec_4 ), [&]( xtt_token_object& o )
         {
            o.phase = xtt_phase::setup_completed;
         });
      });

      tx.set_expiration( db->head_block_time() + XGT_BLOCK_INTERVAL * 10 );
      tx.signatures.clear();
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
   }
   FC_LOG_AND_RETHROW()
}
*/
/*
BOOST_AUTO_TEST_CASE( xtt_nai_pool_removal )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_nai_pool_removal" );

      ACTORS( (alice) )
      asset_symbol_type alice_symbol = create_xtt( "alice", alice_private_key, 0 );

      // Ensure the NAI does not exist in the pool after being registered
      BOOST_REQUIRE( !db->get< nai_pool_object >().contains( alice_symbol ) );
   }
   FC_LOG_AND_RETHROW();
}

BOOST_AUTO_TEST_CASE( xtt_nai_pool_count )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_nai_pool_count" );
      const auto &npo = db->get< nai_pool_object >();

      // We should begin with a full NAI pool
      BOOST_REQUIRE( npo.num_available_nais == XTT_MAX_NAI_POOL_COUNT );

      ACTORS( (alice) )

      fund( "alice", 10 * 1000 * 1000 );
      this->generate_block();

      set_price_feed( price( ASSET( "1.000 TBD" ), ASSET( "1.000 TESTS" ) ) );
      convert( "alice", ASSET( "10000.000 TESTS" ) );

      // Drain the NAI pool one at a time
      for ( unsigned int i = 1; i <= XTT_MAX_NAI_POOL_COUNT; i++ )
      {
         xtt_create_operation op;
         signed_transaction tx;

         op.symbol = get_new_xtt_symbol( 0, this->db );
         op.precision = op.symbol.decimals();
         op.xtt_creation_fee = db->get_dynamic_global_properties().xtt_creation_fee;
         op.control_account = "alice";

         tx.operations.push_back( op );
         tx.set_expiration( this->db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
         tx.sign( alice_private_key, this->db->get_chain_id(), fc::ecc::bip_0062 );

         this->db->push_transaction( tx, 0 );

         BOOST_REQUIRE( npo.num_available_nais == XTT_MAX_NAI_POOL_COUNT - i );
         BOOST_REQUIRE( npo.nais[ npo.num_available_nais ] == asset_symbol_type() );
      }

      // At this point, there should be no available NAIs
      XGT_REQUIRE_THROW( get_new_xtt_symbol( 0, this->db ), fc::assert_exception );

      this->generate_block();

      // We should end with a full NAI pool after block generation
      BOOST_REQUIRE( npo.num_available_nais == XTT_MAX_NAI_POOL_COUNT );
   }
   FC_LOG_AND_RETHROW();
}*/
/*
BOOST_AUTO_TEST_CASE( xtt_setup_emissions_validate )
{
   try
   {
      ACTORS( (alice)(bob) );
      generate_block();

      asset_symbol_type alice_symbol = create_xtt( "alice", alice_private_key, 3 );

      xtt_setup_emissions_operation op;
      op.control_account = "alice";
      op.symbol = alice_symbol;
      op.emissions_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] = 10;
      op.interval_seconds = XTT_EMISSION_MIN_INTERVAL_SECONDS;
      op.emission_count = 1;

      fc::time_point_sec schedule_time = fc::time_point::now();
      fc::time_point_sec schedule_end_time = schedule_time + fc::seconds( op.interval_seconds * ( op.emission_count - 1 ) );

      op.schedule_time = schedule_time;
      op.lep_abs_amount = 0;
      op.rep_abs_amount = 0;
      op.lep_rel_amount_numerator = 1;
      op.rep_rel_amount_numerator = 0;
      op.lep_time = schedule_time;
      op.rep_time = schedule_end_time;
      op.validate();

      BOOST_TEST_MESSAGE( " -- Invalid token symbol" );
      op.symbol = XGT_SYMBOL;
      op.lep_abs_amount = 0;
      op.rep_abs_amount = 0;
      XGT_REQUIRE_THROW( op.validate(), fc::exception );
      op.symbol = alice_symbol;
      op.lep_abs_amount = 0;
      op.rep_abs_amount = 0;

      BOOST_TEST_MESSAGE( " -- No emissions" );
      op.lep_rel_amount_numerator = 0;
      XGT_REQUIRE_THROW( op.validate(), fc::exception );
      op.lep_rel_amount_numerator = 1;

      BOOST_TEST_MESSAGE( " -- Invalid control account name" );
      op.control_account = "@@@@";
      XGT_REQUIRE_THROW( op.validate(), fc::exception );
      op.control_account = "alice";

      BOOST_TEST_MESSAGE(" -- Empty emission unit" );
      op.emissions_unit.token_unit.clear();
      XGT_REQUIRE_THROW( op.validate(), fc::exception );

      BOOST_TEST_MESSAGE( " -- Invalid emission unit token unit account" );
      op.emissions_unit.token_unit[ "@@@@" ] = 10;
      XGT_REQUIRE_THROW( op.validate(), fc::exception );
      op.emissions_unit.token_unit.clear();
      op.emissions_unit.token_unit[ XTT_DESTINATION_REWARDS ] = 1;
      op.emissions_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] = 1;
      op.emissions_unit.token_unit[ XTT_DESTINATION_VESTING ] = 1;

      BOOST_TEST_MESSAGE( " -- Invalid schedule time" );
      op.schedule_time = XGT_GENESIS_TIME;
      XGT_REQUIRE_THROW( op.validate(), fc::exception );
      op.schedule_time = fc::time_point::now();

      BOOST_TEST_MESSAGE( " -- 0 emission count" );
      op.emission_count = 0;
      XGT_REQUIRE_THROW( op.validate(), fc::exception );
      op.emission_count = 1;

      BOOST_TEST_MESSAGE( " -- Interval seconds too low" );
      op.interval_seconds = XTT_EMISSION_MIN_INTERVAL_SECONDS - 1;
      XGT_REQUIRE_THROW( op.validate(), fc::exception );
      op.interval_seconds = XTT_EMISSION_MIN_INTERVAL_SECONDS;

      BOOST_TEST_MESSAGE( " -- Negative asset left endpoint" );
      op.lep_abs_amount = -1;
      XGT_REQUIRE_THROW( op.validate(), fc::exception );
      op.lep_abs_amount = 0;

      BOOST_TEST_MESSAGE( " -- Negative asset right endpoint" );
      op.rep_abs_amount = -1;
      XGT_REQUIRE_THROW( op.validate(), fc::exception );
      op.rep_abs_amount = 0;

      BOOST_TEST_MESSAGE( " -- Left endpoint time cannot be before schedule time" );
      op.lep_time -= fc::seconds( 1 );
      XGT_REQUIRE_THROW( op.validate(), fc::exception );
      op.lep_time += fc::seconds( 1 );

      BOOST_TEST_MESSAGE( " -- Right endpoint time cannot be after schedule end time" );
      op.rep_time += fc::seconds( 1 );
      XGT_REQUIRE_THROW( op.validate(), fc::exception );
      op.rep_time -= fc::seconds( 1 );

      BOOST_TEST_MESSAGE( " -- Left endpoint time and right endpoint time can be anything if they're equal" );
      fc::time_point_sec tp = fc::time_point_sec( 0 );
      op.lep_time = tp;
      op.rep_time = tp;

      op.validate();
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( xtt_setup_emissions_authorities )
{
   try
   {
      //XTT_SYMBOL( alice, 3, db );

      xtt_setup_emissions_operation op;
      op.control_account = "alice";
      fc::time_point now = fc::time_point::now();
      op.schedule_time = now;
      op.emissions_unit.token_unit[ "alice" ] = 10;
      op.lep_abs_amount = op.rep_abs_amount = 1000;

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

BOOST_AUTO_TEST_CASE( xtt_setup_emissions_apply )
{
   try
   {
      ACTORS( (alice)(bob) )

      generate_block();

      asset_symbol_type alice_symbol = create_xtt( "alice", alice_private_key, 3 );

      fc::time_point_sec emissions1_schedule_time = fc::time_point::now();

      xtt_setup_emissions_operation op;
      op.control_account = "alice";
      op.symbol = alice_symbol;
      op.emissions_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] = 10;
      op.schedule_time = emissions1_schedule_time;
      op.interval_seconds = XTT_EMISSION_MIN_INTERVAL_SECONDS;
      op.emission_count = 2;
      op.lep_abs_amount = 0;
      op.rep_abs_amount = 0;
      op.lep_rel_amount_numerator = 1;
      op.rep_rel_amount_numerator = 0;;
      op.validate();

      BOOST_TEST_MESSAGE( " -- Control account does not own XTT" );
      op.control_account = "bob";
      FAIL_WITH_OP( op, bob_private_key, fc::assert_exception );
      op.control_account = "alice";

      BOOST_TEST_MESSAGE( " -- XTT setup phase has already completed" );
      auto xtt_token = util::xtt::find_token( *db, alice_symbol );
      db->modify( *xtt_token, [&] ( xtt_token_object& obj )
      {
         obj.phase = xtt_phase::setup_completed;
      } );
      FAIL_WITH_OP( op, alice_private_key, fc::assert_exception );
      db->modify( *xtt_token, [&] ( xtt_token_object& obj )
      {
         obj.phase = xtt_phase::setup;
      } );
      PUSH_OP( op, alice_private_key );

      BOOST_TEST_MESSAGE( " -- Emissions range is overlapping" );
      xtt_setup_emissions_operation op2;
      op2.control_account = "alice";
      op2.symbol = alice_symbol;
      op2.emissions_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] = 10;
      op2.schedule_time = emissions1_schedule_time + fc::seconds( XTT_EMISSION_MIN_INTERVAL_SECONDS * 2 - 1 );
      op2.interval_seconds = XTT_EMISSION_MIN_INTERVAL_SECONDS;
      op2.emission_count = 6;
      op2.lep_abs_amount = 1200;
      op2.rep_abs_amount = 1000;
      op2.lep_rel_amount_numerator = 1;
      op2.rep_rel_amount_numerator = 2;;
      op2.validate();

      FAIL_WITH_OP( op2, alice_private_key, fc::assert_exception );

      op2.schedule_time += fc::seconds( 1 );
      PUSH_OP( op2, alice_private_key );

      BOOST_TEST_MESSAGE( " -- Emissions must be created in chronological order" );
      xtt_setup_emissions_operation op3;
      op3.control_account = "alice";
      op3.symbol = alice_symbol;
      op3.emissions_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] = 10;
      op3.schedule_time = emissions1_schedule_time - fc::seconds( ( XTT_EMISSION_MIN_INTERVAL_SECONDS * 2 ) + 1 );
      op3.interval_seconds = XTT_EMISSION_MIN_INTERVAL_SECONDS;
      op3.emission_count = XTT_EMIT_INDEFINITELY;
      op3.lep_abs_amount = 0;
      op3.rep_abs_amount = 1000;
      op3.lep_rel_amount_numerator = 0;
      op3.rep_rel_amount_numerator = 0;;
      op3.validate();
      FAIL_WITH_OP( op3, alice_private_key, fc::assert_exception );

      op3.schedule_time = op2.schedule_time + fc::seconds( uint64_t( op2.interval_seconds ) * uint64_t( op2.emission_count - 1 ) );
      FAIL_WITH_OP( op3, alice_private_key, fc::assert_exception );

      op3.schedule_time += XTT_EMISSION_MIN_INTERVAL_SECONDS;
      PUSH_OP( op3, alice_private_key );

      BOOST_TEST_MESSAGE( " -- No more emissions permitted" );
      xtt_setup_emissions_operation op4;
      op4.control_account = "alice";
      op4.symbol = alice_symbol;
      op4.emissions_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] = 10;
      op4.schedule_time = op3.schedule_time + fc::days( 365 );
      op4.interval_seconds = XTT_EMISSION_MIN_INTERVAL_SECONDS;
      op4.emission_count = 10;
      op4.lep_abs_amount = 0;
      op4.rep_abs_amount = 0;
      op4.lep_rel_amount_numerator = 1;
      op4.rep_rel_amount_numerator = 0;;
      op4.validate();

      FAIL_WITH_OP( op4, alice_private_key, fc::assert_exception );

      op4.schedule_time = fc::time_point_sec::maximum();
      FAIL_WITH_OP( op4, alice_private_key, fc::assert_exception );

      validate_database();
   }
   FC_LOG_AND_RETHROW()
}
*/
/*BOOST_AUTO_TEST_CASE( set_setup_parameters_validate )
{
   try
   {
      ACTORS( (alice) )
      generate_block();

      asset_symbol_type alice_symbol = create_xtt( "alice", alice_private_key, 3 );

      xtt_set_setup_parameters_operation op;
      op.control_account = "alice";
      op.symbol = alice_symbol;
      op.setup_parameters.emplace( xtt_param_allow_voting { .value = true } );

      op.validate();

      op.symbol = XGT_SYMBOL;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception ); // Invalid symbol
      op.symbol = VESTS_SYMBOL;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception ); // Invalid symbol
      op.symbol = SBD_SYMBOL;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception ); // Invalid symbol
      op.symbol = alice_symbol;

      op.control_account = "####";
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception ); // Invalid account name
      op.control_account = "alice";

      op.setup_parameters.clear();
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception ); // Empty setup parameters
      op.setup_parameters.emplace( xtt_param_allow_voting { .value = true } );

      op.validate();

      op.setup_parameters.clear();
      op.setup_parameters.emplace( xtt_param_allow_voting { .value = false } );
      op.validate();
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( set_setup_parameters_authorities )
{
   try
   {
      xtt_set_setup_parameters_operation op;
      op.control_account = "alice";

      flat_set<wallet_name_type> auths;
      flat_set<wallet_name_type> expected;

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

BOOST_AUTO_TEST_CASE( set_setup_parameters_apply )
{
   try
   {
      ACTORS( (alice)(bob) )

      generate_block();

      FUND( "alice", 5000000 );
      FUND( "bob", 5000000 );

      set_price_feed( price( ASSET( "1.000 TBD" ), ASSET( "1.000 TESTS" ) ) );
      convert( "alice", ASSET( "5000.000 TESTS" ) );
      convert( "bob", ASSET( "5000.000 TESTS" ) );

      auto alice_symbol = create_xtt( "alice", alice_private_key, 3 );
      auto bob_symbol = create_xtt( "bob", bob_private_key, 3 );

      auto alice_token = util::xtt::find_token( *db, alice_symbol );
      auto bob_token = util::xtt::find_token( *db, bob_symbol );

      BOOST_REQUIRE( alice_token->allow_voting == true );

      xtt_set_setup_parameters_operation op;
      op.control_account = "alice";
      op.symbol = alice_symbol;

      BOOST_TEST_MESSAGE( " -- Succeed set voting to false" );
      op.setup_parameters.emplace( xtt_param_allow_voting { .value = false } );
      PUSH_OP( op, alice_private_key );

      BOOST_REQUIRE( alice_token->allow_voting == false );

      BOOST_TEST_MESSAGE( " -- Succeed set voting to true" );
      op.setup_parameters.clear();
      op.setup_parameters.emplace( xtt_param_allow_voting { .value = true } );
      PUSH_OP( op, alice_private_key );

      BOOST_REQUIRE( alice_token->allow_voting == true );

      BOOST_TEST_MESSAGE( "--- Failure with wrong control account" );
      op.symbol = bob_symbol;

      FAIL_WITH_OP( op, alice_private_key, fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Succeed set voting to true" );
      op.control_account = "bob";
      op.setup_parameters.clear();
      op.setup_parameters.emplace( xtt_param_allow_voting { .value = true } );

      PUSH_OP( op, bob_private_key );
      BOOST_REQUIRE( bob_token->allow_voting == true );


      BOOST_TEST_MESSAGE( " -- Failure with mismatching precision" );
      op.symbol.asset_num++;
      FAIL_WITH_OP( op, bob_private_key, fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Failure with non-existent asset symbol" );
      op.symbol = this->get_new_xtt_symbol( 1, db );
      FAIL_WITH_OP( op, bob_private_key, fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Succeed set voting to false" );
      op.symbol = bob_symbol;
      op.setup_parameters.clear();
      op.setup_parameters.emplace( xtt_param_allow_voting { .value = false } );
      PUSH_OP( op, bob_private_key );
      BOOST_REQUIRE( bob_token->allow_voting == false );


      validate_database();
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( xtt_set_runtime_parameters_validate )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_set_runtime_parameters_validate" );

      xtt_set_runtime_parameters_operation op;

      auto new_symbol = get_new_xtt_symbol( 3, db );

      op.symbol = new_symbol;
      op.control_account = "alice";
      op.runtime_parameters.insert( xtt_param_allow_downvotes() );

      // If this fails, it could indicate a test above has failed for the wrong reasons
      op.validate();

      BOOST_TEST_MESSAGE( "--- Test invalid control account name" );
      op.control_account = "@@@@@";
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.control_account = "alice";

      // Test symbol
      BOOST_TEST_MESSAGE( "--- Invalid XTT creation symbol: vesting symbol used instead of liquid one" );
      op.symbol = op.symbol.get_paired_symbol();
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( "--- Invalid XTT creation symbol: XGT cannot be an XTT" );
      op.symbol = XGT_SYMBOL;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( "--- Invalid XTT creation symbol: SBD cannot be an XTT" );
      op.symbol = SBD_SYMBOL;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( "--- Invalid XTT creation symbol: VESTS cannot be an XTT" );
      op.symbol = VESTS_SYMBOL;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.symbol = new_symbol;

      BOOST_TEST_MESSAGE( "--- Failure when no parameters are set" );
      op.runtime_parameters.clear();
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      [>
       * Inequality to test:
       *
       * 0 <= reverse_auction_window_seconds + XTT_UPVOTE_LOCKOUT < cashout_window_seconds
       * <= XTT_VESTING_WITHDRAW_INTERVAL_SECONDS
       <]

      BOOST_TEST_MESSAGE( "--- Failure when cashout_window_second is equal to XTT_UPVOTE_LOCKOUT" );
      xtt_param_windows_v1 windows;
      windows.reverse_auction_window_seconds = 0;
      windows.cashout_window_seconds = XTT_UPVOTE_LOCKOUT;
      op.runtime_parameters.insert( windows );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( "--- Success when cashout_window_seconds is above XTT_UPVOTE_LOCKOUT" );
      windows.cashout_window_seconds++;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( windows );
      op.validate();

      BOOST_TEST_MESSAGE( "--- Failure when cashout_window_seconds is equal to reverse_auction_window_seconds + XTT_UPVOTE_LOCKOUT" );
      windows.reverse_auction_window_seconds++;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( windows );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( "--- Failure when cashout_window_seconds is greater than XTT_VESTING_WITHDRAW_INTERVAL_SECONDS" );
      windows.cashout_window_seconds = XTT_VESTING_WITHDRAW_INTERVAL_SECONDS + 1;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( windows );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( "--- Success when cashout_window_seconds is equal to XTT_VESTING_WITHDRAW_INTERVAL_SECONDS" );
      windows.cashout_window_seconds--;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( windows );
      op.validate();

      BOOST_TEST_MESSAGE( "--- Success when reverse_auction_window_seconds + XTT_UPVOTE_LOCKOUT is one less than cashout_window_seconds" );
      windows.reverse_auction_window_seconds = windows.cashout_window_seconds - XTT_UPVOTE_LOCKOUT - 1;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( windows );
      op.validate();

      BOOST_TEST_MESSAGE( "--- Failure when reverse_auction_window_seconds + XTT_UPVOTE_LOCKOUT is equal to cashout_window_seconds" );
      windows.cashout_window_seconds--;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( windows );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      [>
       * Conditions to test:
       *
       * 0 < vote_regeneration_seconds < XTT_VESTING_WITHDRAW_INTERVAL_SECONDS
       *
       * votes_per_regeneration_period * 86400 / vote_regeneration_period
       * <= XTT_MAX_NOMINAL_VOTES_PER_DAY
       *
       * 0 < votes_per_regeneration_period <= XTT_MAX_VOTES_PER_REGENERATION
       <]
      uint32_t practical_regen_seconds_lower_bound = 86400 / XTT_MAX_NOMINAL_VOTES_PER_DAY;

      BOOST_TEST_MESSAGE( "--- Failure when vote_regeneration_period_seconds is 0" );
      xtt_param_vote_regeneration_period_seconds_v1 vote_regen;
      vote_regen.vote_regeneration_period_seconds = 0;
      vote_regen.votes_per_regeneration_period = 1;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( vote_regen );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( "--- Success when vote_regeneration_period_seconds is greater than 0" );
      // Any value less than 86 will violate the nominal votes per day check. 86 is a practical minimum as a consequence.
      vote_regen.vote_regeneration_period_seconds = practical_regen_seconds_lower_bound + 1;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( vote_regen );
      op.validate();

      BOOST_TEST_MESSAGE( "--- Failure when vote_regeneration_period_seconds is greater XTT_VESTING_WITHDRAW_INTERVAL_SECONDS" );
      vote_regen.vote_regeneration_period_seconds = XTT_VESTING_WITHDRAW_INTERVAL_SECONDS + 1;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( vote_regen );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( "--- Success when vote_regeneration_period_seconds is equal to XTT_VESTING_WITHDRAW_INTERVAL_SECONDS" );
      vote_regen.vote_regeneration_period_seconds--;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( vote_regen );
      op.validate();

      BOOST_TEST_MESSAGE( "--- Test various \"nominal votes per day\" scenarios" );
      BOOST_TEST_MESSAGE( "--- Mid Point Checks" );
      vote_regen.vote_regeneration_period_seconds = 86400;
      vote_regen.votes_per_regeneration_period = XTT_MAX_NOMINAL_VOTES_PER_DAY;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( vote_regen );
      op.validate();

      vote_regen.vote_regeneration_period_seconds = 86399;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( vote_regen );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      vote_regen.vote_regeneration_period_seconds = 86400;
      vote_regen.votes_per_regeneration_period = XTT_MAX_NOMINAL_VOTES_PER_DAY + 1;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( vote_regen );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      vote_regen.vote_regeneration_period_seconds = 86401;
      vote_regen.votes_per_regeneration_period = XTT_MAX_NOMINAL_VOTES_PER_DAY;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( vote_regen );
      op.validate();

      vote_regen.vote_regeneration_period_seconds = 86400;
      vote_regen.votes_per_regeneration_period = XTT_MAX_NOMINAL_VOTES_PER_DAY - 1;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( vote_regen );
      op.validate();

      BOOST_TEST_MESSAGE( "--- Lower Bound Checks" );
      vote_regen.vote_regeneration_period_seconds = practical_regen_seconds_lower_bound;
      vote_regen.votes_per_regeneration_period = 1;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( vote_regen );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      vote_regen.vote_regeneration_period_seconds = practical_regen_seconds_lower_bound + 1;
      vote_regen.votes_per_regeneration_period = 2;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( vote_regen );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      vote_regen.vote_regeneration_period_seconds = practical_regen_seconds_lower_bound + 2;
      vote_regen.votes_per_regeneration_period = 1;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( vote_regen );
      op.validate();

      BOOST_TEST_MESSAGE( "--- Upper Bound Checks" );
      vote_regen.vote_regeneration_period_seconds = XTT_VESTING_WITHDRAW_INTERVAL_SECONDS;
      vote_regen.votes_per_regeneration_period = XTT_MAX_VOTES_PER_REGENERATION;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( vote_regen );
      op.validate();

      vote_regen.votes_per_regeneration_period = XTT_MAX_VOTES_PER_REGENERATION + 1;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( vote_regen );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      vote_regen.vote_regeneration_period_seconds = XTT_VESTING_WITHDRAW_INTERVAL_SECONDS - 1;
      vote_regen.votes_per_regeneration_period = XTT_MAX_VOTES_PER_REGENERATION;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( vote_regen );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      vote_regen.vote_regeneration_period_seconds = XTT_VESTING_WITHDRAW_INTERVAL_SECONDS;
      vote_regen.votes_per_regeneration_period = XTT_MAX_VOTES_PER_REGENERATION - 1;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( vote_regen );
      op.validate();

      BOOST_TEST_MESSAGE( "--- Failure when percent_curation_rewards greater than 10000" );
      xtt_param_rewards_v1 rewards;
      rewards.content_constant = XGT_CONTENT_CONSTANT_HF0;
      rewards.percent_curation_rewards = XGT_100_PERCENT + 1;
      rewards.author_reward_curve = curve_id::linear;
      rewards.curation_reward_curve = curve_id::square_root;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( rewards );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( "--- Success when percent_curation_rewards is 10000" );
      rewards.percent_curation_rewards = XGT_100_PERCENT;
      op.runtime_parameters.clear();
      op.runtime_parameters.insert( rewards );
      op.validate();

      auto valid_curves = {
         curve_id::quadratic,
         curve_id::linear,
         curve_id::square_root,
         curve_id::convergent_linear,
         curve_id::convergent_square_root,
         curve_id::bounded
      };

      rewards = {};
      for ( auto& curve : valid_curves )
      {
         BOOST_TEST_MESSAGE( std::string( "--- Success when author curve is " ) + fc::reflector< xgt::protocol::curve_id >::to_string( curve ) );
         rewards.author_reward_curve = curve;
         op.runtime_parameters.clear();
         op.runtime_parameters.insert( rewards );
         op.validate();
      }

      rewards = {};
      for ( auto& curve : valid_curves )
      {
         BOOST_TEST_MESSAGE( std::string( "--- Success when curation curve is " ) + fc::reflector< xgt::protocol::curve_id >::to_string( curve ) );
         rewards.curation_reward_curve = curve;
         op.runtime_parameters.clear();
         op.runtime_parameters.insert( rewards );
         op.validate();
      }

      // Literally nothing to test for xtt_param_allow_downvotes because it can only be true or false.
      // Inclusion success was tested in initial positive validation at the beginning of the test.
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( xtt_set_runtime_parameters_authorities )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_set_runtime_parameters_authorities" );
      xtt_set_runtime_parameters_operation op;
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

BOOST_AUTO_TEST_CASE( xtt_set_runtime_parameters_apply )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_set_runtime_parameters_evaluate" );

      ACTORS( (alice)(bob) )
      XTT_SYMBOL( alice, 3, db );
      XTT_SYMBOL( bob, 3, db );

      generate_block();

      db_plugin->debug_update( [=](database& db)
      {
         db.create< xtt_token_object >( [&]( xtt_token_object& o )
         {
            o.control_account = "alice";
            o.liquid_symbol = alice_symbol;
         });
      });

      xtt_set_runtime_parameters_operation op;
      signed_transaction tx;

      BOOST_TEST_MESSAGE( "--- Failure with wrong control account" );
      op.control_account = "bob";
      op.symbol = alice_symbol;
      op.runtime_parameters.insert( xtt_param_allow_downvotes() );
      tx.operations.push_back( op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, bob_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );

      BOOST_TEST_MESSAGE( "--- Failure with a non-existent asset symbol" );
      op.control_account = "alice";
      op.symbol = bob_symbol;
      tx.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );

      BOOST_TEST_MESSAGE( "--- Failure with wrong precision in asset symbol" );
      op.symbol = alice_symbol;
      op.symbol.asset_num++;
      tx.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );

      BOOST_TEST_MESSAGE( "--- Success updating runtime parameters" );
      op.runtime_parameters.clear();
      // These are params different than the default
      xtt_param_windows_v1 windows;
      windows.cashout_window_seconds = 86400 * 4;
      windows.reverse_auction_window_seconds = 60 * 5;
      xtt_param_vote_regeneration_period_seconds_v1 vote_regen;
      vote_regen.vote_regeneration_period_seconds = 86400 * 6;
      vote_regen.votes_per_regeneration_period = 600;
      xtt_param_rewards_v1 rewards;
      rewards.content_constant = uint128_t( uint64_t( 1000000000000ull ) );
      rewards.percent_curation_rewards = 15 * XGT_1_PERCENT;
      rewards.author_reward_curve = curve_id::quadratic;
      rewards.curation_reward_curve = curve_id::linear;
      xtt_param_allow_downvotes downvotes;
      downvotes.value = false;
      op.runtime_parameters.insert( windows );
      op.runtime_parameters.insert( vote_regen );
      op.runtime_parameters.insert( rewards );
      op.runtime_parameters.insert( downvotes );
      op.symbol = alice_symbol;
      tx.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );

      const auto& token = db->get< xtt_token_object, by_symbol >( alice_symbol );

      BOOST_REQUIRE( token.cashout_window_seconds == windows.cashout_window_seconds );
      BOOST_REQUIRE( token.reverse_auction_window_seconds == windows.reverse_auction_window_seconds );
      BOOST_REQUIRE( token.vote_regeneration_period_seconds == vote_regen.vote_regeneration_period_seconds );
      BOOST_REQUIRE( token.votes_per_regeneration_period == vote_regen.votes_per_regeneration_period );
      BOOST_REQUIRE( token.content_constant == rewards.content_constant );
      BOOST_REQUIRE( token.percent_curation_rewards == rewards.percent_curation_rewards );
      BOOST_REQUIRE( token.author_reward_curve == rewards.author_reward_curve );
      BOOST_REQUIRE( token.curation_reward_curve == rewards.curation_reward_curve );
      BOOST_REQUIRE( token.allow_downvotes == downvotes.value );
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( xtt_contribute_validate )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_contribute_validate" );

      auto new_symbol = get_new_xtt_symbol( 3, db );

      xtt_contribute_operation op;
      op.contributor = "alice";
      op.contribution = asset( 1000, XGT_SYMBOL );
      op.contribution_id = 1;
      op.symbol = new_symbol;
      op.validate();

      BOOST_TEST_MESSAGE( " -- Failure on invalid account name" );
      op.contributor = "@@@@@";
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.contributor = "alice";

      BOOST_TEST_MESSAGE( " -- Failure on negative contribution" );
      op.contribution = asset( -1, XGT_SYMBOL );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.contribution = asset( 1000, XGT_SYMBOL );

      BOOST_TEST_MESSAGE( " -- Failure on no contribution" );
      op.contribution = asset( 0, XGT_SYMBOL );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.contribution = asset( 1000, XGT_SYMBOL );

      BOOST_TEST_MESSAGE( " -- Failure on VESTS contribution" );
      op.contribution = asset( 1000, VESTS_SYMBOL );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.contribution = asset( 1000, XGT_SYMBOL );

      BOOST_TEST_MESSAGE( " -- Failure on SBD contribution" );
      op.contribution = asset( 1000, SBD_SYMBOL );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.contribution = asset( 1000, XGT_SYMBOL );

      BOOST_TEST_MESSAGE( " -- Failure on XTT contribution" );
      op.contribution = asset( 1000, new_symbol );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.contribution = asset( 1000, XGT_SYMBOL );

      BOOST_TEST_MESSAGE( " -- Failure on contribution to XGT" );
      op.symbol = XGT_SYMBOL;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.symbol = new_symbol;

      BOOST_TEST_MESSAGE( " -- Failure on contribution to VESTS" );
      op.symbol = VESTS_SYMBOL;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.symbol = new_symbol;

      BOOST_TEST_MESSAGE( " -- Failure on contribution to SBD" );
      op.symbol = SBD_SYMBOL;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.symbol = new_symbol;

      op.validate();
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( xtt_contribute_apply )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_contribute_evaluate" );

      ACTORS( (alice)(bob)(sam) );
      XTT_SYMBOL( alice, 3, db );

      generate_block();

      auto alice_asset_accumulator = asset( 0, XGT_SYMBOL );
      auto bob_asset_accumulator = asset( 0, XGT_SYMBOL );
      auto sam_asset_accumulator = asset( 0, XGT_SYMBOL );

      auto alice_contribution_counter = 0;
      auto bob_contribution_counter = 0;
      auto sam_contribution_counter = 0;

      FUND( "sam", ASSET( "1000.000 TESTS" ) );

      generate_block();

      db_plugin->debug_update( [=] ( database& db )
      {
         db.create< xtt_token_object >( [&]( xtt_token_object& o )
         {
            o.control_account = "alice";
            o.liquid_symbol = alice_symbol;
         } );

         db.create< xtt_ico_object >( [&]( xtt_ico_object& o )
         {
            o.symbol = alice_symbol;
         } );

         db.create< xtt_ico_tier_object >( [&]( xtt_ico_tier_object& o )
         {
            o.symbol = alice_symbol;
            o.xgt_units_cap = 1000;
         } );

         db.create< xtt_ico_tier_object >( [&]( xtt_ico_tier_object& o )
         {
            o.symbol = alice_symbol;
            o.xgt_units_cap = 99000;
         } );
      } );

      xtt_contribute_operation bob_op;
      bob_op.contributor = "bob";
      bob_op.contribution = asset( 1000, XGT_SYMBOL );
      bob_op.contribution_id = bob_contribution_counter;
      bob_op.symbol = alice_symbol;

      xtt_contribute_operation alice_op;
      alice_op.contributor = "alice";
      alice_op.contribution = asset( 2000, XGT_SYMBOL );
      alice_op.contribution_id = alice_contribution_counter;
      alice_op.symbol = alice_symbol;

      xtt_contribute_operation sam_op;
      sam_op.contributor = "sam";
      sam_op.contribution = asset( 3000, XGT_SYMBOL );
      sam_op.contribution_id = sam_contribution_counter;
      sam_op.symbol = alice_symbol;

      BOOST_TEST_MESSAGE( " -- Failure on XTT not in contribution phase" );
      FAIL_WITH_OP( bob_op, bob_private_key, fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Failure on XTT not in contribution phase" );
      FAIL_WITH_OP( alice_op, alice_private_key, fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Failure on XTT not in contribution phase" );
      FAIL_WITH_OP( sam_op, sam_private_key, fc::assert_exception );

      db_plugin->debug_update( [=] ( database& db )
      {
         const xtt_token_object *token = util::xtt::find_token( db, alice_symbol );
         db.modify( *token, [&]( xtt_token_object& o )
         {
            o.phase = xtt_phase::ico;
         } );
      } );

      BOOST_TEST_MESSAGE( " -- Failure on insufficient funds" );
      FAIL_WITH_OP( bob_op, bob_private_key, fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Failure on insufficient funds" );
      FAIL_WITH_OP( alice_op, alice_private_key, fc::assert_exception );

      FUND( "alice", ASSET( "1000.000 TESTS" ) );
      FUND( "bob",   ASSET( "1000.000 TESTS" ) );

      generate_block();

      BOOST_TEST_MESSAGE( " -- Succeed on sufficient funds" );
      bob_op.contribution_id = bob_contribution_counter++;
      PUSH_OP( bob_op, bob_private_key );
      bob_asset_accumulator += bob_op.contribution;

      BOOST_TEST_MESSAGE( " -- Failure on duplicate contribution ID" );
      FAIL_WITH_OP( bob_op, bob_private_key, fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Succeed with new contribution ID" );
      bob_op.contribution_id = bob_contribution_counter++;
      PUSH_OP( bob_op, bob_private_key );
      bob_asset_accumulator += bob_op.contribution;

      BOOST_TEST_MESSAGE( " -- Succeed on sufficient funds" );
      alice_op.contribution_id = alice_contribution_counter++;
      PUSH_OP( alice_op, alice_private_key );
      alice_asset_accumulator += alice_op.contribution;

      BOOST_TEST_MESSAGE( " -- Failure on duplicate contribution ID" );
      FAIL_WITH_OP( alice_op, alice_private_key, fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Succeed with new contribution ID" );
      alice_op.contribution_id = alice_contribution_counter++;
      PUSH_OP( alice_op, alice_private_key );
      alice_asset_accumulator += alice_op.contribution;

      BOOST_TEST_MESSAGE( " -- Succeed with sufficient funds" );
      sam_op.contribution_id = sam_contribution_counter++;
      PUSH_OP( sam_op, sam_private_key );
      sam_asset_accumulator += sam_op.contribution;

      validate_database();

      for ( int i = 0; i < 15; i++ )
      {
         BOOST_TEST_MESSAGE( " -- Successful contribution (alice)" );
         alice_op.contribution_id = alice_contribution_counter++;
         PUSH_OP( alice_op, alice_private_key );
         alice_asset_accumulator += alice_op.contribution;

         BOOST_TEST_MESSAGE( " -- Successful contribution (bob)" );
         bob_op.contribution_id = bob_contribution_counter++;
         PUSH_OP( bob_op, bob_private_key );
         bob_asset_accumulator += bob_op.contribution;

         BOOST_TEST_MESSAGE( " -- Successful contribution (sam)" );
         sam_op.contribution_id = sam_contribution_counter++;
         PUSH_OP( sam_op, sam_private_key );
         sam_asset_accumulator += sam_op.contribution;
      }

      BOOST_TEST_MESSAGE( " -- Fail to contribute after hard cap (alice)" );
      alice_op.contribution_id = alice_contribution_counter + 1;
      FAIL_WITH_OP( alice_op, alice_private_key, fc::exception );

      BOOST_TEST_MESSAGE( " -- Fail to contribute after hard cap (bob)" );
      bob_op.contribution_id = bob_contribution_counter + 1;
      FAIL_WITH_OP( bob_op, bob_private_key, fc::exception );

      BOOST_TEST_MESSAGE( " -- Fail to contribute after hard cap (sam)" );
      sam_op.contribution_id = sam_contribution_counter + 1;
      FAIL_WITH_OP( sam_op, sam_private_key, fc::exception );

      validate_database();

      generate_block();

      db_plugin->debug_update( [=] ( database& db )
      {
         const xtt_token_object *token = util::xtt::find_token( db, alice_symbol );
         db.modify( *token, [&]( xtt_token_object& o )
         {
            o.phase = xtt_phase::ico_completed;
         } );
      } );

      BOOST_TEST_MESSAGE( " -- Failure XTT contribution phase has ended" );
      alice_op.contribution_id = alice_contribution_counter;
      FAIL_WITH_OP( alice_op, alice_private_key, fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Failure XTT contribution phase has ended" );
      bob_op.contribution_id = bob_contribution_counter;
      FAIL_WITH_OP( bob_op, bob_private_key, fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Failure XTT contribution phase has ended" );
      sam_op.contribution_id = sam_contribution_counter;
      FAIL_WITH_OP( sam_op, sam_private_key, fc::assert_exception );

      auto alices_contributions = asset( 0, XGT_SYMBOL );
      auto bobs_contributions = asset( 0, XGT_SYMBOL );
      auto sams_contributions = asset( 0, XGT_SYMBOL );

      auto alices_num_contributions = 0;
      auto bobs_num_contributions = 0;
      auto sams_num_contributions = 0;

      const auto& idx = db->get_index< xtt_contribution_index, by_symbol_contributor >();

      auto itr = idx.lower_bound( boost::make_tuple( alice_symbol, wallet_name_type( "alice" ), 0 ) );
      while( itr != idx.end() && itr->contributor == wallet_name_type( "alice" ) )
      {
         alices_contributions += itr->contribution;
         alices_num_contributions++;
         ++itr;
      }

      itr = idx.lower_bound( boost::make_tuple( alice_symbol, wallet_name_type( "bob" ), 0 ) );
      while( itr != idx.end() && itr->contributor == wallet_name_type( "bob" ) )
      {
         bobs_contributions += itr->contribution;
         bobs_num_contributions++;
         ++itr;
      }

      itr = idx.lower_bound( boost::make_tuple( alice_symbol, wallet_name_type( "sam" ), 0 ) );
      while( itr != idx.end() && itr->contributor == wallet_name_type( "sam" ) )
      {
         sams_contributions += itr->contribution;
         sams_num_contributions++;
         ++itr;
      }

      BOOST_TEST_MESSAGE( " -- Checking account contributions" );
      BOOST_REQUIRE( alices_contributions == alice_asset_accumulator );
      BOOST_REQUIRE( bobs_contributions == bob_asset_accumulator );
      BOOST_REQUIRE( sams_contributions == sam_asset_accumulator );

      BOOST_TEST_MESSAGE( " -- Checking contribution counts" );
      BOOST_REQUIRE( alices_num_contributions == alice_contribution_counter );
      BOOST_REQUIRE( bobs_num_contributions == bob_contribution_counter );
      BOOST_REQUIRE( sams_num_contributions == sam_contribution_counter );

      BOOST_TEST_MESSAGE( " -- Checking account balances" );
      BOOST_REQUIRE( db->get_balance( "alice", XGT_SYMBOL ) == ASSET( "1000.000 TESTS" ) - alice_asset_accumulator );
      BOOST_REQUIRE( db->get_balance( "bob", XGT_SYMBOL ) == ASSET( "1000.000 TESTS" ) - bob_asset_accumulator );
      BOOST_REQUIRE( db->get_balance( "sam", XGT_SYMBOL ) == ASSET( "1000.000 TESTS" ) - sam_asset_accumulator );

      BOOST_TEST_MESSAGE( " -- Checking ICO total contributions" );
      const auto* ico_obj = db->find< xtt_ico_object, by_symbol >( alice_symbol );
      BOOST_REQUIRE( ico_obj->contributed == alice_asset_accumulator + bob_asset_accumulator + sam_asset_accumulator );

      validate_database();
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( xtt_transfer_validate )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_transfer_validate" );
      ACTORS( (alice) )
      auto symbol = create_xtt( "alice", alice_private_key, 3 );

      transfer_operation op;
      op.from = "alice";
      op.to = "bob";
      op.memo = "Memo";
      op.amount = asset( 100, symbol );
      op.validate();

      BOOST_TEST_MESSAGE( " --- Invalid from account" );
      op.from = "alice-";
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.from = "alice";

      BOOST_TEST_MESSAGE( " --- Invalid to account" );
      op.to = "bob-";
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.to = "bob";

      BOOST_TEST_MESSAGE( " --- Memo too long" );
      std::string memo;
      for ( int i = 0; i < XGT_MAX_MEMO_SIZE + 1; i++ )
         memo += "x";
      op.memo = memo;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.memo = "Memo";

      BOOST_TEST_MESSAGE( " --- Negative amount" );
      op.amount = -op.amount;
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.amount = -op.amount;

      BOOST_TEST_MESSAGE( " --- Transferring vests" );
      op.amount = asset( 100, symbol.get_paired_symbol() );
      XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.amount = asset( 100, symbol );

      op.validate();
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( xtt_transfer_apply )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_transfer_apply" );

      ACTORS( (alice)(bob) )
      generate_block();

      auto symbol = create_xtt( "alice", alice_private_key, 3 );

      fund( "alice", asset( 10000, symbol ) );

      BOOST_REQUIRE( db->get_balance( "alice", symbol ) == asset( 10000, symbol ) );
      BOOST_REQUIRE( db->get_balance( "bob", symbol ) == asset( 0, symbol ) );

      signed_transaction tx;
      transfer_operation op;

      op.from = "alice";
      op.to = "bob";
      op.amount = asset( 5000, symbol );

      BOOST_TEST_MESSAGE( "--- Test normal transaction" );
      tx.operations.push_back( op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );

      BOOST_REQUIRE( db->get_balance( "alice", symbol ) == asset( 5000, symbol ) );
      BOOST_REQUIRE( db->get_balance( "bob", symbol ) == asset( 5000, symbol ) );
      validate_database();

      BOOST_TEST_MESSAGE( "--- Generating a block" );
      generate_block();

      BOOST_REQUIRE( db->get_balance( "alice", symbol ) == asset( 5000, symbol ) );
      BOOST_REQUIRE( db->get_balance( "bob", symbol ) == asset( 5000, symbol ) );
      validate_database();

      BOOST_TEST_MESSAGE( "--- Test emptying an account" );
      tx.signatures.clear();
      tx.operations.clear();
      tx.operations.push_back( op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      db->push_transaction( tx, database::skip_transaction_dupe_check );

      BOOST_REQUIRE( db->get_balance( "alice", symbol ) == asset( 0, symbol ) );
      BOOST_REQUIRE( db->get_balance( "bob", symbol ) == asset( 10000, symbol ) );
      validate_database();

      BOOST_TEST_MESSAGE( "--- Test transferring non-existent funds" );
      tx.signatures.clear();
      tx.operations.clear();
      tx.operations.push_back( op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), fc::exception );

      BOOST_REQUIRE( db->get_balance( "alice", symbol ) == asset( 0, symbol ) );
      BOOST_REQUIRE( db->get_balance( "bob", symbol ) == asset( 10000, symbol ) );
      validate_database();
   }
   FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( xtt_setup_validate )
{
   ACTORS( (alice) )
   generate_block();

   BOOST_TEST_MESSAGE( "Testing: xtt_setup_operation validate" );

   xtt_setup_operation op;
   op.control_account = "alice";
   op.symbol = create_xtt( "alice", alice_private_key, 3 );

   op.contribution_begin_time = fc::variant( "2020-12-21T00:00:00" ).as< fc::time_point_sec >();
   op.contribution_end_time   = op.contribution_begin_time + fc::days( 365 );

   op.launch_time = op.contribution_end_time + fc::days( 1 );
   op.max_supply  = XGT_MAX_SHARE_SUPPLY;
   op.min_unit_ratio  = 50;
   op.max_unit_ratio  = 100;
   op.xgt_units_min = 0;

   op.validate();

   BOOST_TEST_MESSAGE( " -- Failure on invalid control acount name" );
   op.control_account = "@@@@";
   BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );
   op.control_account = "alice";

   BOOST_TEST_MESSAGE( " -- Failure on contribution_end_time > contribution_begin_time" );
   op.contribution_end_time = op.contribution_begin_time - fc::seconds( 1 );
   BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );
   op.contribution_end_time = op.contribution_begin_time + fc::days( 365 );

   BOOST_TEST_MESSAGE( " -- Failure on launch_time > contribution_end_time" );
   op.launch_time = op.contribution_end_time - fc::seconds( 1 );
   BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );
   op.launch_time = op.contribution_end_time + fc::days( 1 );

   BOOST_TEST_MESSAGE( " -- Failure on max_supply > XGT_MAX_SHARE_SUPPLY" );
   op.max_supply = XGT_MAX_SHARE_SUPPLY + 1;
   BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );
   op.max_supply = XGT_MAX_SHARE_SUPPLY;

   BOOST_TEST_MESSAGE( " -- Successful sanity check" );
   op.validate();
}


BOOST_AUTO_TEST_CASE( xtt_setup_authorities )
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

BOOST_AUTO_TEST_CASE( xtt_setup_apply )
{
   BOOST_TEST_MESSAGE( "Testing: xtt_setup_operation apply" );
   ACTORS( (alice)(bob)(sam)(dave) )

   generate_block();

   BOOST_TEST_MESSAGE( " -- XTT creation" );
   auto symbol = create_xtt( "alice", alice_private_key, 3 );

   BOOST_TEST_MESSAGE( " -- XTT setup" );
   signed_transaction tx;
   xtt_setup_operation setup_op;

   setup_op.control_account = "alice";
   setup_op.symbol = symbol;
   setup_op.contribution_begin_time = db->head_block_time() + XGT_BLOCK_INTERVAL;
   setup_op.contribution_end_time   = setup_op.contribution_begin_time + fc::days( 30 );
   setup_op.xgt_units_min         = 0;
   setup_op.min_unit_ratio = 50;
   setup_op.max_unit_ratio = 100;
   setup_op.max_supply = XGT_MAX_SHARE_SUPPLY;
   setup_op.launch_time = setup_op.contribution_end_time + fc::days( 1 );

   xtt_capped_generation_policy invalid_generation_policy;
   invalid_generation_policy.pre_soft_cap_unit = {
      {
         { "elaine", 2 },
         { "$!alice.vesting", 2 },
         { XTT_DESTINATION_MARKET_MAKER, 2 }
      },
      {
         { "alice", 2 },
         { "$!alice.vesting", 2 },
         { XTT_DESTINATION_MARKET_MAKER, 2 },
         { XTT_DESTINATION_REWARDS, 2 },
         { XTT_DESTINATION_FROM, 2 },
         { XTT_DESTINATION_FROM_VESTING, 2 }
      }
   };
   invalid_generation_policy.post_soft_cap_unit = {
      {
         { "alice", 1 },
         { "$!alice.vesting", 1 },
         { XTT_DESTINATION_MARKET_MAKER, 1 }
      },
      {
         { "alice", 1 },
         { "$!alice.vesting", 1 },
         { XTT_DESTINATION_MARKET_MAKER, 1 },
         { XTT_DESTINATION_REWARDS, 1 },
         { XTT_DESTINATION_FROM, 1 },
         { XTT_DESTINATION_FROM_VESTING, 1 }
      }
   };
   invalid_generation_policy.max_unit_ratio = 100;
   invalid_generation_policy.min_unit_ratio = 50;
   setup_op.initial_generation_policy = invalid_generation_policy;

   BOOST_TEST_MESSAGE( " -- Failure on non-existent account (elaine) on pre soft cap xgt unit" );
   tx.operations.push_back( setup_op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
   tx.operations.clear();
   tx.signatures.clear();

   invalid_generation_policy.pre_soft_cap_unit = {
      {
         { "alice", 2 },
         { "$!alice.vesting", 2 },
         { XTT_DESTINATION_MARKET_MAKER, 2 }
      },
      {
         { "elaine", 2 },
         { "$!alice.vesting", 2 },
         { XTT_DESTINATION_MARKET_MAKER, 2 },
         { XTT_DESTINATION_REWARDS, 2 },
         { XTT_DESTINATION_FROM, 2 },
         { XTT_DESTINATION_FROM_VESTING, 2 }
      }
   };
   invalid_generation_policy.post_soft_cap_unit = {
      {
         { "alice", 1 },
         { "$!alice.vesting", 1 },
         { XTT_DESTINATION_MARKET_MAKER, 1 }
      },
      {
         { "alice", 1 },
         { "$!alice.vesting", 1 },
         { XTT_DESTINATION_MARKET_MAKER, 1 },
         { XTT_DESTINATION_REWARDS, 1 },
         { XTT_DESTINATION_FROM, 1 },
         { XTT_DESTINATION_FROM_VESTING, 1 }
      }
   };
   invalid_generation_policy.max_unit_ratio = 100;
   invalid_generation_policy.min_unit_ratio = 50;
   setup_op.initial_generation_policy = invalid_generation_policy;

   BOOST_TEST_MESSAGE( " -- Failure on non-existent account (elaine) on pre soft cap token unit" );
   tx.operations.push_back( setup_op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
   tx.operations.clear();
   tx.signatures.clear();

   invalid_generation_policy.pre_soft_cap_unit = {
      {
         { "alice", 2 },
         { "$!alice.vesting", 2 },
         { XTT_DESTINATION_MARKET_MAKER, 2 }
      },
      {
         { "alice", 2 },
         { "$!alice.vesting", 2 },
         { XTT_DESTINATION_MARKET_MAKER, 2 },
         { XTT_DESTINATION_REWARDS, 2 },
         { XTT_DESTINATION_FROM, 2 },
         { XTT_DESTINATION_FROM_VESTING, 2 }
      }
   };
   invalid_generation_policy.post_soft_cap_unit = {
      {
         { "elaine", 1 },
         { "$!alice.vesting", 1 },
         { XTT_DESTINATION_MARKET_MAKER, 1 }
      },
      {
         { "alice", 1 },
         { "$!alice.vesting", 1 },
         { XTT_DESTINATION_MARKET_MAKER, 1 },
         { XTT_DESTINATION_REWARDS, 1 },
         { XTT_DESTINATION_FROM, 1 },
         { XTT_DESTINATION_FROM_VESTING, 1 }
      }
   };
   invalid_generation_policy.max_unit_ratio = 100;
   invalid_generation_policy.min_unit_ratio = 50;
   setup_op.initial_generation_policy = invalid_generation_policy;

   BOOST_TEST_MESSAGE( " -- Failure on non-existent account (elaine) on post soft cap xgt unit" );
   tx.operations.push_back( setup_op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
   tx.operations.clear();
   tx.signatures.clear();

   invalid_generation_policy.pre_soft_cap_unit = {
      {
         { "elaine", 2 },
         { "$!alice.vesting", 2 },
         { XTT_DESTINATION_MARKET_MAKER, 2 }
      },
      {
         { "alice", 2 },
         { "$!alice.vesting", 2 },
         { XTT_DESTINATION_MARKET_MAKER, 2 },
         { XTT_DESTINATION_REWARDS, 2 },
         { XTT_DESTINATION_FROM, 2 },
         { XTT_DESTINATION_FROM_VESTING, 2 }
      }
   };
   invalid_generation_policy.post_soft_cap_unit = {
      {
         { "alice", 1 },
         { "$!alice.vesting", 1 },
         { XTT_DESTINATION_MARKET_MAKER, 1 }
      },
      {
         { "elaine", 1 },
         { "$!alice.vesting", 1 },
         { XTT_DESTINATION_MARKET_MAKER, 1 },
         { XTT_DESTINATION_REWARDS, 1 },
         { XTT_DESTINATION_FROM, 1 },
         { XTT_DESTINATION_FROM_VESTING, 1 }
      }
   };
   invalid_generation_policy.max_unit_ratio = 100;
   invalid_generation_policy.min_unit_ratio = 50;
   setup_op.initial_generation_policy = invalid_generation_policy;

   BOOST_TEST_MESSAGE( " -- Failure on non-existent account (elaine) on post soft cap token unit" );
   tx.operations.push_back( setup_op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
   tx.operations.clear();
   tx.signatures.clear();

   invalid_generation_policy.pre_soft_cap_unit = {
      {
         { "alice", 2 },
         { "$!elaine.vesting", 2 },
         { XTT_DESTINATION_MARKET_MAKER, 2 }
      },
      {
         { "alice", 2 },
         { "$!alice.vesting", 2 },
         { XTT_DESTINATION_MARKET_MAKER, 2 },
         { XTT_DESTINATION_REWARDS, 2 },
         { XTT_DESTINATION_FROM, 2 },
         { XTT_DESTINATION_FROM_VESTING, 2 }
      }
   };
   invalid_generation_policy.post_soft_cap_unit = {
      {
         { "alice", 1 },
         { "$!alice.vesting", 1 },
         { XTT_DESTINATION_MARKET_MAKER, 1 }
      },
      {
         { "alice", 1 },
         { "$!alice.vesting", 1 },
         { XTT_DESTINATION_MARKET_MAKER, 1 },
         { XTT_DESTINATION_REWARDS, 1 },
         { XTT_DESTINATION_FROM, 1 },
         { XTT_DESTINATION_FROM_VESTING, 1 }
      }
   };
   invalid_generation_policy.max_unit_ratio = 100;
   invalid_generation_policy.min_unit_ratio = 50;
   setup_op.initial_generation_policy = invalid_generation_policy;

   BOOST_TEST_MESSAGE( " -- Failure on non-existent founder vesting account (elaine) on pre soft cap xgt unit" );
   tx.operations.push_back( setup_op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
   tx.operations.clear();
   tx.signatures.clear();

   invalid_generation_policy.pre_soft_cap_unit = {
      {
         { "alice", 2 },
         { "$!alice.vesting", 2 },
         { XTT_DESTINATION_MARKET_MAKER, 2 }
      },
      {
         { "alice", 2 },
         { "$!elaine.vesting", 2 },
         { XTT_DESTINATION_MARKET_MAKER, 2 },
         { XTT_DESTINATION_REWARDS, 2 },
         { XTT_DESTINATION_FROM, 2 },
         { XTT_DESTINATION_FROM_VESTING, 2 }
      }
   };
   invalid_generation_policy.post_soft_cap_unit = {
      {
         { "alice", 1 },
         { "$!alice.vesting", 1 },
         { XTT_DESTINATION_MARKET_MAKER, 1 }
      },
      {
         { "alice", 1 },
         { "$!alice.vesting", 1 },
         { XTT_DESTINATION_MARKET_MAKER, 1 },
         { XTT_DESTINATION_REWARDS, 1 },
         { XTT_DESTINATION_FROM, 1 },
         { XTT_DESTINATION_FROM_VESTING, 1 }
      }
   };
   invalid_generation_policy.max_unit_ratio = 100;
   invalid_generation_policy.min_unit_ratio = 50;
   setup_op.initial_generation_policy = invalid_generation_policy;

   BOOST_TEST_MESSAGE( " -- Failure on non-existent founder vesting account (elaine) on pre soft cap token unit" );
   tx.operations.push_back( setup_op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
   tx.operations.clear();
   tx.signatures.clear();

   invalid_generation_policy.pre_soft_cap_unit = {
      {
         { "alice", 2 },
         { "$!alice.vesting", 2 },
         { XTT_DESTINATION_MARKET_MAKER, 2 }
      },
      {
         { "alice", 2 },
         { "$!alice.vesting", 2 },
         { XTT_DESTINATION_MARKET_MAKER, 2 },
         { XTT_DESTINATION_REWARDS, 2 },
         { XTT_DESTINATION_FROM, 2 },
         { XTT_DESTINATION_FROM_VESTING, 2 }
      }
   };
   invalid_generation_policy.post_soft_cap_unit = {
      {
         { "alice", 1 },
         { "$!elaine.vesting", 1 },
         { XTT_DESTINATION_MARKET_MAKER, 1 }
      },
      {
         { "alice", 1 },
         { "$!alice.vesting", 1 },
         { XTT_DESTINATION_MARKET_MAKER, 1 },
         { XTT_DESTINATION_REWARDS, 1 },
         { XTT_DESTINATION_FROM, 1 },
         { XTT_DESTINATION_FROM_VESTING, 1 }
      }
   };
   invalid_generation_policy.max_unit_ratio = 100;
   invalid_generation_policy.min_unit_ratio = 50;
   setup_op.initial_generation_policy = invalid_generation_policy;

   BOOST_TEST_MESSAGE( " -- Failure on non-existent founder vesting account (elaine) on post soft cap xgt unit" );
   tx.operations.push_back( setup_op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
   tx.operations.clear();
   tx.signatures.clear();

   invalid_generation_policy.pre_soft_cap_unit = {
      {
         { "alice", 2 },
         { "$!alice.vesting", 2 },
         { XTT_DESTINATION_MARKET_MAKER, 2 }
      },
      {
         { "alice", 2 },
         { "$!alice.vesting", 2 },
         { XTT_DESTINATION_MARKET_MAKER, 2 },
         { XTT_DESTINATION_REWARDS, 2 },
         { XTT_DESTINATION_FROM, 2 },
         { XTT_DESTINATION_FROM_VESTING, 2 }
      }
   };
   invalid_generation_policy.post_soft_cap_unit = {
      {
         { "alice", 1 },
         { "$!alice.vesting", 1 },
         { XTT_DESTINATION_MARKET_MAKER, 1 }
      },
      {
         { "alice", 1 },
         { "$!elaine.vesting", 1 },
         { XTT_DESTINATION_MARKET_MAKER, 1 },
         { XTT_DESTINATION_REWARDS, 1 },
         { XTT_DESTINATION_FROM, 1 },
         { XTT_DESTINATION_FROM_VESTING, 1 }
      }
   };
   invalid_generation_policy.max_unit_ratio = 100;
   invalid_generation_policy.min_unit_ratio = 50;
   setup_op.initial_generation_policy = invalid_generation_policy;

   BOOST_TEST_MESSAGE( " -- Failure on non-existent founder vesting account (elaine) on post soft cap token unit" );
   tx.operations.push_back( setup_op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
   tx.operations.clear();
   tx.signatures.clear();

   xtt_capped_generation_policy valid_generation_policy;
   valid_generation_policy.pre_soft_cap_unit = {
      {
         { "alice", 2 },
         { "$!alice.vesting", 2 },
         { XTT_DESTINATION_MARKET_MAKER, 2 }
      },
      {
         { "alice", 2 },
         { "$!alice.vesting", 2 },
         { XTT_DESTINATION_MARKET_MAKER, 2 },
         { XTT_DESTINATION_REWARDS, 2 },
         { XTT_DESTINATION_FROM, 2 },
         { XTT_DESTINATION_FROM_VESTING, 2 }
      }
   };
   valid_generation_policy.post_soft_cap_unit = {
      {
         { "alice", 1 },
         { "$!alice.vesting", 1 },
         { XTT_DESTINATION_MARKET_MAKER, 1 }
      },
      {
         { "alice", 1 },
         { "$!alice.vesting", 1 },
         { XTT_DESTINATION_MARKET_MAKER, 1 },
         { XTT_DESTINATION_REWARDS, 1 },
         { XTT_DESTINATION_FROM, 1 },
         { XTT_DESTINATION_FROM_VESTING, 1 }
      }
   };
   valid_generation_policy.max_unit_ratio = 100;
   valid_generation_policy.min_unit_ratio = 50;
   setup_op.initial_generation_policy = valid_generation_policy;

   setup_op.control_account = "bob";

   BOOST_TEST_MESSAGE( " -- Failure to setup XTT with non controlling account" );
   tx.operations.push_back( setup_op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, bob_private_key );
   BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
   tx.operations.clear();
   tx.signatures.clear();

   setup_op.control_account = "alice";

   BOOST_TEST_MESSAGE( " -- Success on valid XTT setup" );
   tx.operations.push_back( setup_op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   db->push_transaction( tx, 0 );

   generate_block();
}

BOOST_AUTO_TEST_CASE( comment_votable_assets_validate )
{
   try
   {
      BOOST_TEST_MESSAGE( "Test Comment Votable Assets Validate" );
      ACTORS((alice));

      generate_block();

      std::array< asset_symbol_type, XTT_MAX_VOTABLE_ASSETS + 1 > xtts;
      /// Create one more than limit to test negative cases
      for( size_t i = 0; i < XTT_MAX_VOTABLE_ASSETS + 1; ++i )
      {
         xtts[i] = create_xtt( "alice", alice_private_key, 0 );
      }

      {
         comment_options_operation op;

         op.author = "alice";
         op.permlink = "test";

         BOOST_TEST_MESSAGE( "--- Testing valid configuration: no votable_assets" );
         allowed_vote_assets ava;
         op.extensions.insert( ava );
         op.validate();
      }

      {
         comment_options_operation op;

         op.author = "alice";
         op.permlink = "test";

         BOOST_TEST_MESSAGE( "--- Testing valid configuration of votable_assets" );
         allowed_vote_assets ava;
         for( size_t i = 0; i < XTT_MAX_VOTABLE_ASSETS; ++i )
         {
            ava.add_votable_asset( xtts[i], 10 + i, (i & 2) != 0 );
         }

         op.extensions.insert( ava );
         op.validate();
      }

      {
         comment_options_operation op;

         op.author = "alice";
         op.permlink = "test";

         BOOST_TEST_MESSAGE( "--- Testing invalid configuration of votable_assets - too much assets specified" );
         allowed_vote_assets ava;
         for( size_t i = 0; i < xtts.size(); ++i )
         {
            ava.add_votable_asset( xtts[i], 20 + i, (i & 2) != 0 );
         }

         op.extensions.insert( ava );
         XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      }

      {
         comment_options_operation op;

         op.author = "alice";
         op.permlink = "test";

         BOOST_TEST_MESSAGE( "--- Testing invalid configuration of votable_assets - XGT added to container" );
         allowed_vote_assets ava;
         ava.add_votable_asset( xtts.front(), 20, false);
         ava.add_votable_asset( XGT_SYMBOL, 20, true);
         op.extensions.insert( ava );
         XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
      }

      {
         comment_options_operation op;

         op.author = "alice";
         op.permlink = "test";

         BOOST_TEST_MESSAGE( "--- Testing more than 100% weight on a single route" );
         allowed_vote_assets ava;
         ava.add_votable_asset( xtts[0], 10, true );

         auto& b = ava.votable_assets[xtts[0]].beneficiaries;

         b.beneficiaries.push_back( beneficiary_route_type( wallet_name_type( "bob" ), XGT_100_PERCENT + 1 ) );
         op.extensions.insert( ava );
         XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

         BOOST_TEST_MESSAGE( "--- Testing more than 100% total weight" );
         b.beneficiaries.clear();
         b.beneficiaries.push_back( beneficiary_route_type( wallet_name_type( "bob" ), XGT_1_PERCENT * 75 ) );
         b.beneficiaries.push_back( beneficiary_route_type( wallet_name_type( "sam" ), XGT_1_PERCENT * 75 ) );
         op.extensions.clear();
         op.extensions.insert( ava );
         XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

         BOOST_TEST_MESSAGE( "--- Testing maximum number of routes" );
         b.beneficiaries.clear();
         for( size_t i = 0; i < 127; i++ )
         {
            b.beneficiaries.push_back( beneficiary_route_type( wallet_name_type( "foo" + fc::to_string( i ) ), 1 ) );
         }

         op.extensions.clear();
         std::sort( b.beneficiaries.begin(), b.beneficiaries.end() );
         op.extensions.insert( ava );
         op.validate();

         BOOST_TEST_MESSAGE( "--- Testing one too many routes" );
         b.beneficiaries.push_back( beneficiary_route_type( wallet_name_type( "bar" ), 1 ) );
         std::sort( b.beneficiaries.begin(), b.beneficiaries.end() );
         op.extensions.clear();
         op.extensions.insert( ava );
         XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );


         BOOST_TEST_MESSAGE( "--- Testing duplicate accounts" );
         b.beneficiaries.clear();
         b.beneficiaries.push_back( beneficiary_route_type( "bob", XGT_1_PERCENT * 2 ) );
         b.beneficiaries.push_back( beneficiary_route_type( "bob", XGT_1_PERCENT ) );
         op.extensions.clear();
         op.extensions.insert( ava );
         XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

         BOOST_TEST_MESSAGE( "--- Testing incorrect account sort order" );
         b.beneficiaries.clear();
         b.beneficiaries.push_back( beneficiary_route_type( "bob", XGT_1_PERCENT ) );
         b.beneficiaries.push_back( beneficiary_route_type( "alice", XGT_1_PERCENT ) );
         op.extensions.clear();
         op.extensions.insert( ava );
         XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

         BOOST_TEST_MESSAGE( "--- Testing correct account sort order" );
         b.beneficiaries.clear();
         b.beneficiaries.push_back( beneficiary_route_type( "alice", XGT_1_PERCENT ) );
         b.beneficiaries.push_back( beneficiary_route_type( "bob", XGT_1_PERCENT ) );
         op.extensions.clear();
         op.extensions.insert( ava );
         op.validate();
      }
   }
   FC_LOG_AND_RETHROW()
}*/
/*
BOOST_AUTO_TEST_CASE( comment_votable_assets_apply )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: comment votable assets apply" );
      ACTORS( (alice)(bob) );

      XTT_SYMBOL( alice, 3, db );
      generate_block();

      comment_operation comment;
      comment_options_operation op;
      allowed_vote_assets ava;
      votable_asset_options opts;
      signed_transaction tx;

      comment.author = "alice";
      comment.permlink = "test";
      comment.parent_permlink = "test";
      comment.title = "test";
      comment.body = "foobar";

      tx.operations.push_back( comment );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      db->push_transaction( tx );


      BOOST_TEST_MESSAGE( "--- Failure when XTT does not exist" );

      op.author = "alice";
      op.permlink = "test";
      ava.votable_assets[ alice_symbol ] = opts;
      op.extensions.insert( ava );
      tx.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );


      BOOST_TEST_MESSAGE( "--- Failure when XTT is not launched" );

      alice_symbol = create_xtt( "alice", alice_private_key, 3 );
      ava.votable_assets.clear();
      ava.votable_assets[ alice_symbol ] = opts;
      op.extensions.clear();
      op.extensions.insert( ava );
      tx.clear();
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );

      BOOST_TEST_MESSAGE( "--- Success" );

      generate_block();
      db_plugin->debug_update( [=]( database& db )
      {
         db.modify( db.get< xtt_token_object, by_symbol >( alice_symbol ), [=]( xtt_token_object& xtt )
         {
            xtt.phase = xtt_phase::launch_success;
         });
      });
      generate_block();

      ava.votable_assets.clear();
      ava.votable_assets[ alice_symbol ] = opts;
      op.extensions.clear();
      op.extensions.insert( ava );
      tx.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );

      {
         const auto& alice_comment = db->get_comment( op.author, op.permlink );

         BOOST_REQUIRE( alice_comment.allowed_vote_assets.find( alice_symbol ) != alice_comment.allowed_vote_assets.end() );
         const auto va_opts = alice_comment.allowed_vote_assets.find( alice_symbol );

         BOOST_REQUIRE( va_opts->second.max_accepted_payout == opts.max_accepted_payout );
         BOOST_REQUIRE( va_opts->second.allow_curation_rewards == opts.allow_curation_rewards );

         const auto beneficiaries = db->find< comment_xtt_beneficiaries_object, by_comment_symbol >( boost::make_tuple( alice_comment.id, alice_symbol ) );
         BOOST_REQUIRE( beneficiaries == nullptr );
      }


      BOOST_TEST_MESSAGE( "--- Failure with non-existent beneficiary" );

      opts.beneficiaries.beneficiaries.push_back( beneficiary_route_type{ "charlie", XGT_100_PERCENT } );
      ava.votable_assets.clear();
      ava.votable_assets[ alice_symbol ] = opts;
      op.extensions.clear();
      op.extensions.insert( ava );
      tx.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );


      BOOST_TEST_MESSAGE( "--- Success changing when rshares are 0" );

      opts.beneficiaries.beneficiaries.clear();
      opts.beneficiaries.beneficiaries.push_back( beneficiary_route_type{ "bob", XGT_100_PERCENT } );
      opts.max_accepted_payout = 100;
      opts.allow_curation_rewards = false;
      ava.votable_assets.clear();
      ava.votable_assets[ alice_symbol ] = opts;
      op.extensions.clear();
      op.extensions.insert( ava );
      tx.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );
      {
         const auto& alice_comment = db->get_comment( op.author, op.permlink );

         BOOST_REQUIRE( alice_comment.allowed_vote_assets.find( alice_symbol ) != alice_comment.allowed_vote_assets.end() );
         const auto va_opts = alice_comment.allowed_vote_assets.find( alice_symbol );
         BOOST_REQUIRE( va_opts->second.max_accepted_payout == opts.max_accepted_payout );
         BOOST_REQUIRE( va_opts->second.allow_curation_rewards == opts.allow_curation_rewards );

         const auto beneficiaries = db->find< comment_xtt_beneficiaries_object, by_comment_symbol >( boost::make_tuple( alice_comment.id, alice_symbol ) );
         BOOST_REQUIRE( beneficiaries != nullptr );
         BOOST_REQUIRE( beneficiaries->beneficiaries.size() == 1 );
      }


      BOOST_TEST_MESSAGE( "--- Failure changing when rshares are non-zero" );

      generate_block();
      db_plugin->debug_update( [=]( database& db )
      {
         db.modify( db.get_comment( op.author, op.permlink ), [=]( comment_object& c )
         {
            c.net_rshares = 1;
            c.abs_rshares = 1;
         });
      });
      generate_block();

      opts.beneficiaries.beneficiaries.clear();
      ava.votable_assets.clear();
      ava.votable_assets[ alice_symbol ] = opts;
      op.extensions.clear();
      op.extensions.insert( ava );
      tx.clear();
      tx.operations.push_back( op );
      sign( tx, alice_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );


      BOOST_TEST_MESSAGE( "--- Failure specifying non-inherited votable asset" );

      generate_block();
      auto bob_symbol = create_xtt( "bob", bob_private_key, 3 );
      generate_block();
      db_plugin->debug_update( [=]( database& db )
      {
         db.modify( db.get< xtt_token_object, by_symbol >( bob_symbol ), [=]( xtt_token_object& xtt )
         {
            xtt.phase = xtt_phase::launch_success;
         });
      });
      generate_block();

      comment.parent_author = comment.author;
      comment.parent_permlink = comment.permlink;
      comment.author = "bob";
      tx.clear();
      tx.operations.push_back( comment );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, bob_private_key );
      db->push_transaction( tx, 0 );

      op.author = comment.author;
      ava.votable_assets.clear();
      ava.votable_assets[ bob_symbol ] = opts;
      op.extensions.clear();
      op.extensions.insert( ava );
      tx.clear();
      tx.operations.push_back( op );
      sign( tx, bob_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );


      BOOST_TEST_MESSAGE( "--- Success specifying inherited votable asset" );

      ava.votable_assets.clear();
      ava.votable_assets[ alice_symbol ] = opts;
      op.extensions.clear();
      op.extensions.insert( ava );
      tx.clear();
      tx.operations.push_back( op );
      sign( tx, bob_private_key );
      db->push_transaction( tx, 0 );


      BOOST_TEST_MESSAGE( "--- Failure specifying an XTT that disallows voting" );

      generate_block();
      db_plugin->debug_update( [=]( database& db )
      {
         db.modify( db.get< xtt_token_object, by_symbol >( bob_symbol ), [=]( xtt_token_object& xtt )
         {
            xtt.allow_voting = false;
         });
      });
      generate_block();

      comment.parent_author = "";
      comment.parent_permlink = "foorbar";
      comment.permlink = "foobar";
      op.author = comment.author;
      op.permlink = comment.permlink;
      ava.votable_assets.clear();
      ava.votable_assets[ bob_symbol ] = opts;
      op.extensions.clear();
      op.extensions.insert( ava );
      tx.clear();
      tx.operations.push_back( comment );
      tx.operations.push_back( op );
      sign( tx, bob_private_key );
      XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
   }
   FC_LOG_AND_RETHROW()
}*/
/*
BOOST_AUTO_TEST_CASE( xtt_delegate_vesting_shares_validate )
{
   BOOST_TEST_MESSAGE( "Testing: xtt_delegate_vesting_shares_validate" );
   ACTORS( (alice) )
   auto symbol = create_xtt( "alice", alice_private_key, 3 );

   delegate_vesting_shares_operation op;

   op.delegator = "alice";
   op.delegatee = "bob";
   op.vesting_shares = asset( 0, symbol.get_paired_symbol() );
   op.validate();

   BOOST_TEST_MESSAGE( " --- Invalid vesting_shares amount" );
   op.vesting_shares = asset( -1, symbol.get_paired_symbol() );
   XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );

   BOOST_TEST_MESSAGE( " --- Invalid vesting_shares symbol (non-vesting)" );
   op.vesting_shares = asset( 0, symbol );
   XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
   op.vesting_shares = asset( 0, symbol.get_paired_symbol() );

   BOOST_TEST_MESSAGE( " --- Invalid delegator account" );
   op.delegator = "alice-";
   XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
   op.delegator = "alice";

   BOOST_TEST_MESSAGE( " --- Invalid to account" );
   op.delegatee = "bob-";
   XGT_REQUIRE_THROW( op.validate(), fc::assert_exception );
   op.delegatee = "bob";

   op.validate();
}*/
/*
BOOST_AUTO_TEST_CASE( xtt_delegate_vesting_shares_apply )
{
   BOOST_TEST_MESSAGE( "Testing: xtt_delegate_vesting_shares_apply" );
   signed_transaction tx;
   ACTORS( (alice)(bob)(charlie)(creator) )
   generate_block();

   auto symbol = create_xtt( "creator", creator_private_key, 3 );
   fund( XGT_INIT_MINER_NAME, asset( 10000000, symbol ) );
   vest( XGT_INIT_MINER_NAME, "alice", asset( 10000000, symbol ) );

   generate_block();

   delegate_vesting_shares_operation op;
   op.vesting_shares = asset( 1000000000000, symbol.get_paired_symbol() );
   op.delegator = "alice";
   op.delegatee = "bob";

   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   tx.operations.push_back( op );
   sign( tx, alice_private_key );
   db->push_transaction( tx, 0 );
   generate_blocks( 1 );
   const account_regular_balance_object& alice_acc = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "alice", symbol ) );
   const account_regular_balance_object& bob_acc   = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "bob", symbol ) );

   BOOST_REQUIRE( alice_acc.delegated_vesting_shares == asset( 1000000000000, symbol.get_paired_symbol() ) );
   BOOST_REQUIRE( bob_acc.received_vesting_shares == asset( 1000000000000, symbol.get_paired_symbol() ) );

   BOOST_TEST_MESSAGE( "--- Test that the delegation object is correct. " );
   auto delegation = db->find< vesting_delegation_object, by_delegation >( boost::make_tuple( op.delegator, op.delegatee, op.vesting_shares.symbol.get_paired_symbol() ) );

   BOOST_REQUIRE( delegation != nullptr );
   BOOST_REQUIRE( delegation->delegator == op.delegator);
   BOOST_REQUIRE( delegation->vesting_shares  == asset( 1000000000000, symbol.get_paired_symbol() ) );

   validate_database();
   tx.clear();
   op.vesting_shares = asset( 2000000000000, symbol.get_paired_symbol() );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   tx.operations.push_back( op );
   sign( tx, alice_private_key );
   db->push_transaction( tx, 0 );
   generate_blocks(1);

   BOOST_REQUIRE( delegation != nullptr );
   BOOST_REQUIRE( delegation->delegator == op.delegator);
   BOOST_REQUIRE( delegation->vesting_shares == asset( 2000000000000, symbol.get_paired_symbol() ) );
   BOOST_REQUIRE( alice_acc.delegated_vesting_shares == asset( 2000000000000, symbol.get_paired_symbol() ) );
   BOOST_REQUIRE( bob_acc.received_vesting_shares == asset( 2000000000000, symbol.get_paired_symbol() ) );

   BOOST_TEST_MESSAGE( "--- Test failure delegating delgated VESTS." );

   op.delegator = "bob";
   op.delegatee = "charlie";
   tx.clear();
   tx.operations.push_back( op );
   sign( tx, bob_private_key );
   BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );


   BOOST_TEST_MESSAGE( "--- Test that effective vesting shares is accurate and being applied." );
   tx.operations.clear();
   tx.signatures.clear();

   db_plugin->debug_update( [=]( database& db )
   {
      db.modify( db.get< xtt_token_object, by_symbol >( symbol ), [=]( xtt_token_object& xtt )
      {
         xtt.phase = xtt_phase::launch_success;
      } );
   } );
   generate_block();

   util::energybar old_energybar = bob_acc.voting_energybar;
   util::energybar_params params( util::get_effective_vesting_shares( bob_acc ), XGT_VOTING_ENERGY_REGENERATION_SECONDS );
   old_energybar.regenerate_energy( params, db->head_block_time() );

   comment_operation comment_op;
   comment_op.author = "alice";
   comment_op.permlink = "foo";
   comment_op.parent_permlink = "test";
   comment_op.title = "bar";
   comment_op.body = "foo bar";
   tx.operations.push_back( comment_op );

   comment_options_operation comment_opts;
   comment_opts.author = "alice";
   comment_opts.permlink = "foo";
   allowed_vote_assets ava;
   ava.votable_assets[ symbol ] = votable_asset_options();
   comment_opts.extensions.insert( ava );
   tx.operations.push_back( comment_opts );

   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, alice_private_key );
   db->push_transaction( tx, 0 );
   tx.signatures.clear();
   tx.operations.clear();
   vote2_operation vote_op;
   vote_op.voter = "bob";
   vote_op.author = "alice";
   vote_op.permlink = "foo";
   vote_op.rshares[ symbol ] = util::get_effective_vesting_shares( bob_acc ) / 50;
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   tx.operations.push_back( vote_op );
   sign( tx, bob_private_key );

   db->push_transaction( tx, 0 );
   generate_blocks(1);

   const auto& vote_idx = db->get_index< comment_vote_index >().indices().get< by_comment_voter_symbol >();

   auto& alice_comment = db->get_comment( "alice", string( "foo" ) );
   auto itr = vote_idx.find( boost::make_tuple( alice_comment.id, db->get_account("bob").id, symbol ) );
   BOOST_REQUIRE( alice_comment.xtt_rshares.find( symbol )->second.net_rshares.value == old_energybar.current_energy - bob_acc.voting_energybar.current_energy - XGT_VOTE_DUST_THRESHOLD );
   BOOST_REQUIRE( itr->rshares == old_energybar.current_energy - bob_acc.voting_energybar.current_energy - XGT_VOTE_DUST_THRESHOLD );

   generate_block();
   ACTORS( (sam)(dave) )
   generate_block();

   fund( "sam", asset( 1000000, symbol ) );
   vest( "sam", asset( 1000000, symbol ) );

   generate_block();

   auto sam_vest = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "sam", symbol ) ).vesting_shares;

   BOOST_TEST_MESSAGE( "--- Test failure when delegating 0 VESTS" );
   tx.clear();
   op.delegator = "sam";
   op.delegatee = "dave";
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   sign( tx, sam_private_key );
   XGT_REQUIRE_THROW( db->push_transaction( tx ), fc::assert_exception );


   BOOST_TEST_MESSAGE( "--- Testing failure delegating more vesting shares than account has." );
   tx.clear();
   op.vesting_shares = asset( sam_vest.amount + 1, symbol.get_paired_symbol() );
   tx.operations.push_back( op );
   sign( tx, sam_private_key );
   XGT_REQUIRE_THROW( db->push_transaction( tx ), fc::assert_exception );


   BOOST_TEST_MESSAGE( "--- Test failure delegating vesting shares that are part of a power down" );
   tx.clear();
   sam_vest = asset( sam_vest.amount / 2, symbol.get_paired_symbol() );
   withdraw_vesting_operation withdraw;
   withdraw.account = "sam";
   withdraw.vesting_shares = sam_vest;
   tx.operations.push_back( withdraw );
   sign( tx, sam_private_key );
   db->push_transaction( tx, 0 );

   tx.clear();
   op.vesting_shares = asset( sam_vest.amount + 2, symbol.get_paired_symbol() );
   tx.operations.push_back( op );
   sign( tx, sam_private_key );
   XGT_REQUIRE_THROW( db->push_transaction( tx ), fc::assert_exception );

   tx.clear();
   withdraw.vesting_shares = asset( 0, symbol.get_paired_symbol() );
   tx.operations.push_back( withdraw );
   sign( tx, sam_private_key );
   db->push_transaction( tx, 0 );


   BOOST_TEST_MESSAGE( "--- Test failure powering down vesting shares that are delegated" );
   sam_vest.amount += 1000;
   op.vesting_shares = sam_vest;
   tx.clear();
   tx.operations.push_back( op );
   sign( tx, sam_private_key );
   db->push_transaction( tx, 0 );

   tx.clear();
   withdraw.vesting_shares = asset( sam_vest.amount, symbol.get_paired_symbol() );
   tx.operations.push_back( withdraw );
   sign( tx, sam_private_key );
   XGT_REQUIRE_THROW( db->push_transaction( tx ), fc::assert_exception );


   BOOST_TEST_MESSAGE( "--- Remove a delegation and ensure it is returned after 1 week" );
   tx.clear();
   op.vesting_shares = asset( 0, symbol.get_paired_symbol() );
   tx.operations.push_back( op );
   sign( tx, sam_private_key );
   db->push_transaction( tx, 0 );

   auto exp_obj = db->get_index< vesting_delegation_expiration_index, by_id >().begin();
   auto end = db->get_index< vesting_delegation_expiration_index, by_id >().end();
   auto gpo = db->get_dynamic_global_properties();

   const auto& sam_acc = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "sam", symbol ) );
   const auto& dave_acc = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "dave", symbol ) );

   BOOST_REQUIRE( gpo.delegation_return_period == XGT_DELEGATION_RETURN_PERIOD_HF20 );

   BOOST_REQUIRE( exp_obj != end );
   BOOST_REQUIRE( exp_obj->delegator == "sam" );
   BOOST_REQUIRE( exp_obj->vesting_shares == sam_vest );
   BOOST_REQUIRE( exp_obj->expiration == db->head_block_time() + gpo.delegation_return_period );
   BOOST_REQUIRE( sam_acc.delegated_vesting_shares == sam_vest );
   BOOST_REQUIRE( dave_acc.received_vesting_shares == asset( 0, symbol.get_paired_symbol() ) );
   delegation = db->find< vesting_delegation_object, by_delegation >( boost::make_tuple( op.delegator, op.delegatee, op.vesting_shares.symbol.get_paired_symbol() ) );
   BOOST_REQUIRE( delegation == nullptr );

   generate_blocks( exp_obj->expiration + XGT_BLOCK_INTERVAL );

   exp_obj = db->get_index< vesting_delegation_expiration_index, by_id >().begin();
   end = db->get_index< vesting_delegation_expiration_index, by_id >().end();

   BOOST_REQUIRE( exp_obj == end );
   BOOST_REQUIRE( sam_acc.delegated_vesting_shares == asset( 0, symbol.get_paired_symbol() ) );
}
*/
/*BOOST_AUTO_TEST_CASE( vote2_authorities )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: vote_authorities" );

      vote2_operation op;
      op.voter = "alice";

      flat_set< wallet_name_type > auths;
      flat_set< wallet_name_type > expected;

      op.get_required_recovery_authorities( auths );
      BOOST_REQUIRE( auths == expected );

      op.get_required_money_authorities( auths );
      BOOST_REQUIRE( auths == expected );

      expected.insert( "alice" );
      op.get_required_social_authorities( auths );
      BOOST_REQUIRE( auths == expected );
   }
   FC_LOG_AND_RETHROW()
}*/
/*
BOOST_AUTO_TEST_CASE( vote2_apply )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: vote2_apply" );

      ACTORS( (alice)(bob)(sam)(dave) )
      generate_block();

      auto alice_symbol = create_xtt( "alice", alice_private_key, 3 );
      auto bob_symbol   = create_xtt( "bob", bob_private_key, 3 );

      vest( XGT_INIT_MINER_NAME, "alice", ASSET( "10.000 TESTS" ) );
      validate_database();
      vest( XGT_INIT_MINER_NAME, "bob" , ASSET( "10.000 TESTS" ) );
      vest( XGT_INIT_MINER_NAME, "sam" , ASSET( "10.000 TESTS" ) );
      vest( XGT_INIT_MINER_NAME, "dave" , ASSET( "10.000 TESTS" ) );

      db_plugin->debug_update( [=]( database& db )
      {
         auto alice_vests = db.create_vesting( db.get_account( "alice" ), asset( 100000, alice_symbol ), false );
         alice_vests += db.create_vesting( db.get_account( "bob" ),   asset( 100000, alice_symbol ), false );
         alice_vests += db.create_vesting( db.get_account( "sam" ),   asset( 100000, alice_symbol ), false );
         alice_vests += db.create_vesting( db.get_account( "dave" ),  asset( 100000, alice_symbol ), false );

         auto bob_vests = db.create_vesting( db.get_account( "alice" ), asset( 100000, bob_symbol ), false );

         db.modify( db.get< xtt_token_object, by_symbol >( alice_symbol ), [=]( xtt_token_object& xtt )
         {
            xtt.phase = xtt_phase::launch_success;
            xtt.current_supply = 40000;
            xtt.total_vesting_shares = alice_vests.amount;
            xtt.total_vesting_fund_xtt = 40000;
            xtt.votes_per_regeneration_period = 50;
            xtt.vote_regeneration_period_seconds = 5*24*60*60;
         });

         db.modify( db.get< xtt_token_object, by_symbol >( bob_symbol ), [=]( xtt_token_object& xtt )
         {
            xtt.phase = xtt_phase::launch_success;
            xtt.current_supply = 10000;
            xtt.total_vesting_shares = bob_vests.amount;
            xtt.total_vesting_fund_xtt = 10000;
         });
      });
      generate_block();

      const auto& vote_idx = db->get_index< comment_vote_index, by_comment_voter_symbol >();

      {
         const auto& alice = db->get_account( "alice" );
         const auto& alice_xtt = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "alice", alice_symbol ) );

         signed_transaction tx;
         comment_operation comment_op;
         comment_op.author = "alice";
         comment_op.permlink = "foo";
         comment_op.parent_permlink = "test";
         comment_op.title = "bar";
         comment_op.body = "foo bar";
         allowed_vote_assets ava;
         ava.votable_assets[ alice_symbol ] = votable_asset_options();
         comment_options_operation comment_opts;
         comment_opts.author = "alice";
         comment_opts.permlink = "foo";
         comment_opts.extensions.insert( ava );
         tx.operations.push_back( comment_op );
         tx.operations.push_back( comment_opts );
         tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
         sign( tx, alice_private_key );
         db->push_transaction( tx, 0 );


         BOOST_TEST_MESSAGE( "--- Testing voting on a non-existent comment" );

         tx.operations.clear();
         tx.signatures.clear();

         vote2_operation op;
         op.voter = "alice";
         op.author = "bob";
         op.permlink = "foo";
         op.rshares[ XGT_SYMBOL ] = alice.vesting_shares.amount.value / 50;
         tx.operations.push_back( op );
         sign( tx, alice_private_key );

         XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );

         validate_database();


         BOOST_TEST_MESSAGE( "--- Testing voting with 0 rshares" );

         op.rshares[ XGT_SYMBOL ] = 0;
         tx.operations.clear();
         tx.signatures.clear();
         tx.operations.push_back( op );
         sign( tx, alice_private_key );

         XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );

         validate_database();


         BOOST_TEST_MESSAGE( "--- Testing voting with a non-existent XTT" );

         XTT_SYMBOL( sam, 3, db );
         op.rshares[ XGT_SYMBOL ] = alice.vesting_shares.amount.value / 50;
         op.author = "alice";
         op.rshares[ sam_symbol ] = 1;
         tx.clear();
         tx.operations.push_back( op );
         sign( tx, alice_private_key );

         XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );

         validate_database();


         BOOST_TEST_MESSAGE( "--- Test voting with a non-votable asset" );
         op.rshares.clear();
         op.rshares[ bob_symbol ] = 1;
         tx.clear();
         tx.operations.push_back( op );
         sign( tx, alice_private_key );

         XGT_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::exception );

         validate_database();


         BOOST_TEST_MESSAGE( "--- Testing success" );

         util::energybar_params params( util::get_effective_vesting_shares( alice ), XGT_VOTING_ENERGY_REGENERATION_SECONDS );
         util::energybar old_energybar = alice.voting_energybar;
         old_energybar.regenerate_energy( params, db->head_block_time() );

         util::energybar old_xtt_energybar = alice_xtt.voting_energybar;
         params.max_energy = util::get_effective_vesting_shares( alice_xtt );
         old_xtt_energybar.regenerate_energy( params, db->head_block_time() );

         op.rshares.clear();
         op.rshares[ XGT_SYMBOL ] = alice.vesting_shares.amount.value / 50;
         op.rshares[ alice_symbol ] = alice_xtt.vesting_shares.amount.value / 50;
         tx.operations.clear();
         tx.signatures.clear();
         tx.operations.push_back( op );
         sign( tx, alice_private_key );

         db->push_transaction( tx, 0 );

         auto& alice_comment = db->get_comment( "alice", string( "foo" ) );
         auto itr = vote_idx.find( boost::make_tuple( alice_comment.id, alice.id, XGT_SYMBOL ) );

         BOOST_REQUIRE( alice.last_vote_time == db->head_block_time() );
         BOOST_REQUIRE( old_energybar.current_energy - op.rshares[ XGT_SYMBOL ] == alice.voting_energybar.current_energy );
         BOOST_REQUIRE( alice_comment.net_rshares.value == op.rshares[ XGT_SYMBOL ] - XGT_VOTE_DUST_THRESHOLD );
         BOOST_REQUIRE( alice_comment.cashout_time == alice_comment.created + XGT_CASHOUT_WINDOW_SECONDS );
         BOOST_REQUIRE( itr->rshares == op.rshares[ XGT_SYMBOL ] - XGT_VOTE_DUST_THRESHOLD );
         BOOST_REQUIRE( itr != vote_idx.end() );

         itr = vote_idx.find( boost::make_tuple( alice_comment.id, alice.id, alice_symbol ) );

         BOOST_REQUIRE( old_xtt_energybar.current_energy - op.rshares[ alice_symbol ] == alice_xtt.voting_energybar.current_energy );
         BOOST_REQUIRE( alice_comment.xtt_rshares.find( alice_symbol )->second.net_rshares.value == op.rshares[ alice_symbol ] - XGT_VOTE_DUST_THRESHOLD );
         BOOST_REQUIRE( itr != vote_idx.end() );
         BOOST_REQUIRE( itr->rshares == op.rshares[ alice_symbol ] - XGT_VOTE_DUST_THRESHOLD );

         validate_database();


         BOOST_TEST_MESSAGE( "--- Test reduced power for quick voting" );

         generate_blocks( db->head_block_time() + XGT_MIN_VOTE_INTERVAL_SEC );
         {
            const auto& alice = db->get_account( "alice" );
            params.max_energy = util::get_effective_vesting_shares( alice );
            old_energybar = alice.voting_energybar;
            old_energybar.regenerate_energy( params, db->head_block_time() );

            const auto& alice_xtt = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "alice", alice_symbol ) );
            params.max_energy = util::get_effective_vesting_shares( alice_xtt );
            old_xtt_energybar = alice_xtt.voting_energybar;
            old_xtt_energybar.regenerate_energy( params, db->head_block_time() );

            comment_op.author = "bob";
            comment_op.permlink = "foo";
            comment_op.title = "bar";
            comment_op.body = "foo bar";
            comment_opts.author = "bob";
            comment_opts.permlink = "foo";
            tx.operations.clear();
            tx.signatures.clear();
            tx.operations.push_back( comment_op );
            tx.operations.push_back( comment_opts );
            sign( tx, bob_private_key );
            db->push_transaction( tx, 0 );

            op.rshares[ XGT_SYMBOL ] = old_energybar.current_energy / 50;
            op.rshares[ alice_symbol ] = old_xtt_energybar.current_energy / 50;
            op.voter = "alice";
            op.author = "bob";
            op.permlink = "foo";

            op.rshares[ XGT_SYMBOL ] += 10;
            tx.clear();
            tx.operations.push_back( op );
            sign( tx, alice_private_key );
            BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );

            op.rshares[ XGT_SYMBOL ] -= 10;
            op.rshares[ alice_symbol ] += 10;
            tx.clear();
            tx.operations.push_back( op );
            sign( tx, alice_private_key );
            BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );

            op.rshares[ alice_symbol ] -= 10;
            tx.clear();
            tx.operations.push_back( op );
            sign( tx, alice_private_key );
            db->push_transaction( tx, 0 );

            const auto& bob_comment = db->get_comment( "bob", string( "foo" ) );
            itr = vote_idx.find( boost::make_tuple( bob_comment.id, alice.id, XGT_SYMBOL ) );

            BOOST_REQUIRE( bob_comment.net_rshares.value == op.rshares[ XGT_SYMBOL ] - XGT_VOTE_DUST_THRESHOLD );
            BOOST_REQUIRE( bob_comment.cashout_time == bob_comment.created + XGT_CASHOUT_WINDOW_SECONDS );
            BOOST_REQUIRE( alice.voting_energybar.current_energy == old_energybar.current_energy - op.rshares[ XGT_SYMBOL ] );
            BOOST_REQUIRE( itr != vote_idx.end() );

            itr = vote_idx.find( boost::make_tuple( bob_comment.id, alice.id, alice_symbol ) );
            BOOST_REQUIRE( bob_comment.xtt_rshares.find( alice_symbol )->second.net_rshares.value == op.rshares[ alice_symbol ] - XGT_VOTE_DUST_THRESHOLD );
            BOOST_REQUIRE( alice_xtt.voting_energybar.current_energy == old_xtt_energybar.current_energy - op.rshares[ alice_symbol ] );
            BOOST_REQUIRE( itr != vote_idx.end() );

            validate_database();
         }


         BOOST_TEST_MESSAGE( "--- Test payout time extension on vote" );
         {
            old_energybar = db->get_account( "bob" ).voting_energybar;
            params.max_energy = util::get_effective_vesting_shares( db->get_account( "bob" ) );
            old_energybar.regenerate_energy( params, db->head_block_time() );

            const auto& bob_xtt = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "bob", alice_symbol ) );
            params.max_energy = util::get_effective_vesting_shares( bob_xtt );
            old_xtt_energybar = bob_xtt.voting_energybar;
            old_xtt_energybar.regenerate_energy( params, db->head_block_time() );

            auto old_abs_rshares = db->get_comment( "alice", string( "foo" ) ).abs_rshares.value;
            auto old_xtt_abs_rshares = db->get_comment( "alice", string( "foo" ) ).xtt_rshares.find( alice_symbol )->second.abs_rshares.value;

            generate_blocks( db->head_block_time() + fc::seconds( ( XGT_CASHOUT_WINDOW_SECONDS / 2 ) ), true );

            const auto& new_bob = db->get_account( "bob" );
            const auto& new_bob_xtt = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "bob", alice_symbol ) );
            const auto& new_alice_comment = db->get_comment( "alice", string( "foo" ) );

            op.rshares[ XGT_SYMBOL ] = old_energybar.current_energy / 50;
            op.rshares[ alice_symbol ] = old_xtt_energybar.current_energy / 50;
            op.voter = "bob";
            op.author = "alice";
            op.permlink = "foo";
            tx.operations.clear();
            tx.signatures.clear();
            tx.operations.push_back( op );
            tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
            sign( tx, bob_private_key );
            db->push_transaction( tx, 0 );

            itr = vote_idx.find( boost::make_tuple( new_alice_comment.id, new_bob.id, XGT_SYMBOL ) );

            BOOST_REQUIRE( new_alice_comment.net_rshares.value == old_abs_rshares + ( old_energybar.current_energy - new_bob.voting_energybar.current_energy ) - XGT_VOTE_DUST_THRESHOLD );
            BOOST_REQUIRE( new_alice_comment.cashout_time == new_alice_comment.created + XGT_CASHOUT_WINDOW_SECONDS );
            BOOST_REQUIRE( itr != vote_idx.end() );

            itr = vote_idx.find( boost::make_tuple( new_alice_comment.id, new_bob.id, alice_symbol ) );
            BOOST_REQUIRE( new_alice_comment.xtt_rshares.find( alice_symbol )->second.net_rshares.value == old_xtt_abs_rshares + ( old_xtt_energybar.current_energy - new_bob_xtt.voting_energybar.current_energy ) - XGT_VOTE_DUST_THRESHOLD );
            BOOST_REQUIRE( itr != vote_idx.end() );

            validate_database();
         }


         BOOST_TEST_MESSAGE( "--- Test negative vote" );
         util::energybar old_downvote_energybar;
         util::energybar old_xtt_downvote_energybar;

         {
            const auto& new_sam = db->get_account( "sam" );
            const auto& new_sam_xtt = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "sam", alice_symbol ) );
            const auto& new_bob_comment = db->get_comment( "bob", string( "foo" ) );

            auto old_abs_rshares = new_bob_comment.abs_rshares.value;
            auto old_xtt_abs_rshares = new_bob_comment.xtt_rshares.find( alice_symbol )->second.abs_rshares.value;

            old_energybar = new_sam.voting_energybar;
            params.max_energy = util::get_effective_vesting_shares( new_sam );
            old_energybar.regenerate_energy( params, db->head_block_time() );

            const auto& sam_xtt = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "sam", alice_symbol ) );
            params.max_energy = util::get_effective_vesting_shares( sam_xtt );
            old_xtt_energybar = sam_xtt.voting_energybar;
            old_xtt_energybar.regenerate_energy( params, db->head_block_time() );

            op.rshares[ XGT_SYMBOL ] = -1 * old_energybar.current_energy / 50 / 2;
            op.rshares[ alice_symbol ] = -1 * old_xtt_energybar.current_energy / 50 / 2;
            op.voter = "sam";
            op.author = "bob";
            op.permlink = "foo";
            tx.operations.clear();
            tx.signatures.clear();
            tx.operations.push_back( op );
            sign( tx, sam_private_key );
            db->push_transaction( tx, 0 );

            itr = vote_idx.find( boost::make_tuple( new_bob_comment.id, new_sam.id, XGT_SYMBOL ) );

            params.max_energy = util::get_effective_vesting_shares( new_sam ) / 4;
            old_downvote_energybar.regenerate_energy( params, db->head_block_time() );
            int64_t sam_rshares = old_downvote_energybar.current_energy - new_sam.downvote_energybar.current_energy - XGT_VOTE_DUST_THRESHOLD;

            params.max_energy = util::get_effective_vesting_shares( new_sam_xtt ) / 4;
            old_xtt_downvote_energybar.regenerate_energy( params, db->head_block_time() );
            int64_t sam_xtt_rshares = old_xtt_downvote_energybar.current_energy - new_sam_xtt.downvote_energybar.current_energy - XGT_VOTE_DUST_THRESHOLD;

            BOOST_REQUIRE( new_bob_comment.net_rshares.value == old_abs_rshares - sam_rshares );
            BOOST_REQUIRE( new_bob_comment.abs_rshares.value == old_abs_rshares + sam_rshares );
            BOOST_REQUIRE( new_bob_comment.cashout_time == new_bob_comment.created + XGT_CASHOUT_WINDOW_SECONDS );
            BOOST_REQUIRE( itr != vote_idx.end() );

            itr = vote_idx.find( boost::make_tuple( new_bob_comment.id, new_sam.id, alice_symbol ) );

            BOOST_REQUIRE( new_bob_comment.xtt_rshares.find( alice_symbol )->second.net_rshares.value == old_xtt_abs_rshares - sam_xtt_rshares );
            BOOST_REQUIRE( new_bob_comment.xtt_rshares.find( alice_symbol )->second.abs_rshares.value == old_xtt_abs_rshares + sam_xtt_rshares );
            BOOST_REQUIRE( itr != vote_idx.end() );
            validate_database();
         }


         BOOST_TEST_MESSAGE( "--- Test nested voting on nested comments" );
         {
            old_energybar = db->get_account( "alice" ).voting_energybar;
            params.max_energy = util::get_effective_vesting_shares( db->get_account( "alice" ) );
            old_energybar.regenerate_energy( params, db->head_block_time() );

            const auto& alice_xtt = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "alice", alice_symbol ) );
            params.max_energy = util::get_effective_vesting_shares( alice_xtt );
            old_xtt_energybar = alice_xtt.voting_energybar;
            old_xtt_energybar.regenerate_energy( params, db->head_block_time() );

            comment_op.author = "sam";
            comment_op.permlink = "foo";
            comment_op.title = "bar";
            comment_op.body = "foo bar";
            comment_op.parent_author = "alice";
            comment_op.parent_permlink = "foo";
            tx.operations.clear();
            tx.signatures.clear();
            tx.operations.push_back( comment_op );
            sign( tx, sam_private_key );
            db->push_transaction( tx, 0 );

            op.rshares[ XGT_SYMBOL ] = old_energybar.current_energy / 50;
            op.rshares[ alice_symbol ] = old_xtt_energybar.current_energy / 50;
            op.voter = "alice";
            op.author = "sam";
            op.permlink = "foo";
            tx.operations.clear();
            tx.signatures.clear();
            tx.operations.push_back( op );
            sign( tx, alice_private_key );
            db->push_transaction( tx, 0 );

            BOOST_REQUIRE( db->get_comment( "alice", string( "foo" ) ).cashout_time == db->get_comment( "alice", string( "foo" ) ).created + XGT_CASHOUT_WINDOW_SECONDS );

            validate_database();
         }


         BOOST_TEST_MESSAGE( "--- Test increasing vote rshares" );
         {
            generate_blocks( db->head_block_time() + XGT_MIN_VOTE_INTERVAL_SEC );

            const auto& new_alice = db->get_account( "alice" );
            const auto& new_bob_comment = db->get_comment( "bob", string( "foo" ) );
            auto alice_bob_vote = vote_idx.find( boost::make_tuple( new_bob_comment.id, new_alice.id, XGT_SYMBOL ) );
            auto old_vote_rshares = alice_bob_vote->rshares;
            auto old_net_rshares = new_bob_comment.net_rshares.value;
            auto old_abs_rshares = new_bob_comment.abs_rshares.value;

            alice_bob_vote = vote_idx.find( boost::make_tuple( new_bob_comment.id, new_alice.id, alice_symbol ) );
            auto old_xtt_vote_rshares = alice_bob_vote->rshares;
            auto old_xtt_net_rshares = new_bob_comment.xtt_rshares.find( alice_symbol )->second.net_rshares.value;
            auto old_xtt_abs_rshares = new_bob_comment.xtt_rshares.find( alice_symbol )->second.abs_rshares.value;

            old_energybar = db->get_account( "alice" ).voting_energybar;
            params.max_energy = util::get_effective_vesting_shares( db->get_account( "alice" ) );
            old_energybar.regenerate_energy( params, db->head_block_time() );

            const auto& alice_xtt = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "sam", alice_symbol ) );
            params.max_energy = util::get_effective_vesting_shares( alice_xtt );
            old_xtt_energybar = alice_xtt.voting_energybar;
            old_xtt_energybar.regenerate_energy( params, db->head_block_time() );

            op.voter = "alice";
            op.rshares[ XGT_SYMBOL ] = old_energybar.current_energy / 50 / 2;
            op.rshares[ alice_symbol ] = old_xtt_energybar.current_energy / 50 / 2;
            op.author = "bob";
            op.permlink = "foo";
            tx.operations.clear();
            tx.signatures.clear();
            tx.operations.push_back( op );
            sign( tx, alice_private_key );
            db->push_transaction( tx, 0 );
            alice_bob_vote = vote_idx.find( boost::make_tuple( new_bob_comment.id, new_alice.id, XGT_SYMBOL ) );

            int64_t new_rshares = op.rshares[ XGT_SYMBOL ] - XGT_VOTE_DUST_THRESHOLD;
            int64_t new_xtt_rshares = op.rshares[ alice_symbol ] - XGT_VOTE_DUST_THRESHOLD;

            BOOST_REQUIRE( new_bob_comment.net_rshares == old_net_rshares - old_vote_rshares + new_rshares );
            BOOST_REQUIRE( new_bob_comment.abs_rshares == old_abs_rshares + new_rshares );
            BOOST_REQUIRE( new_bob_comment.cashout_time == new_bob_comment.created + XGT_CASHOUT_WINDOW_SECONDS );
            BOOST_REQUIRE( alice_bob_vote->rshares == new_rshares );
            BOOST_REQUIRE( alice_bob_vote->last_update == db->head_block_time() );

            alice_bob_vote = vote_idx.find( boost::make_tuple( new_bob_comment.id, new_alice.id, alice_symbol ) );

            BOOST_REQUIRE( new_bob_comment.xtt_rshares.find( alice_symbol )->second.net_rshares == old_xtt_net_rshares - old_xtt_vote_rshares + new_xtt_rshares );
            BOOST_REQUIRE( new_bob_comment.xtt_rshares.find( alice_symbol )->second.abs_rshares == old_xtt_abs_rshares + new_xtt_rshares );
            BOOST_REQUIRE( alice_bob_vote->rshares == new_xtt_rshares );
            BOOST_REQUIRE( alice_bob_vote->last_update == db->head_block_time() );

            validate_database();
         }


         BOOST_TEST_MESSAGE( "--- Test decreasing vote rshares" );
         {
            generate_blocks( db->head_block_time() + XGT_MIN_VOTE_INTERVAL_SEC );

            const auto& new_alice = db->get_account( "alice" );
            const auto& new_bob_comment = db->get_comment( "bob", string( "foo" ) );
            int64_t old_vote_rshares = op.rshares[ XGT_SYMBOL ] - XGT_VOTE_DUST_THRESHOLD;
            auto old_net_rshares = new_bob_comment.net_rshares.value;
            auto old_abs_rshares = new_bob_comment.abs_rshares.value;

            int64_t old_xtt_vote_rshares = op.rshares[ alice_symbol ] - XGT_VOTE_DUST_THRESHOLD;
            auto old_xtt_net_rshares = new_bob_comment.xtt_rshares.find( alice_symbol )->second.net_rshares.value;
            auto old_xtt_abs_rshares = new_bob_comment.xtt_rshares.find( alice_symbol )->second.abs_rshares.value;

            auto old_energybar = db->get_account( "alice" ).voting_energybar;
            params.max_energy = util::get_effective_vesting_shares( db->get_account( "alice" ) );
            old_energybar.regenerate_energy( params, db->head_block_time() );

            const auto& alice_xtt = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "alice", alice_symbol ) );
            old_xtt_energybar = alice_xtt.voting_energybar;
            params.max_energy = util::get_effective_vesting_shares( alice_xtt );
            old_xtt_energybar.regenerate_energy( params, db->head_block_time() );

            op.rshares[ XGT_SYMBOL ] = old_energybar.current_energy / 50 / 3;
            op.rshares[ alice_symbol ] = old_xtt_energybar.current_energy / 50 / 3;
            tx.operations.clear();
            tx.signatures.clear();
            tx.operations.push_back( op );
            sign( tx, alice_private_key );
            db->push_transaction( tx, 0 );
            auto alice_bob_vote = vote_idx.find( boost::make_tuple( new_bob_comment.id, new_alice.id, XGT_SYMBOL ) );

            int64_t new_rshares = op.rshares[ XGT_SYMBOL ] - XGT_VOTE_DUST_THRESHOLD;
            int64_t new_xtt_rshares = op.rshares[ alice_symbol ] - XGT_VOTE_DUST_THRESHOLD;

            BOOST_REQUIRE( new_bob_comment.net_rshares == old_net_rshares - old_vote_rshares + new_rshares );
            BOOST_REQUIRE( new_bob_comment.abs_rshares == old_abs_rshares + new_rshares );
            BOOST_REQUIRE( new_bob_comment.cashout_time == new_bob_comment.created + XGT_CASHOUT_WINDOW_SECONDS );
            BOOST_REQUIRE( alice_bob_vote->rshares == new_rshares );
            BOOST_REQUIRE( alice_bob_vote->last_update == db->head_block_time() );

            alice_bob_vote = vote_idx.find( boost::make_tuple( new_bob_comment.id, new_alice.id, alice_symbol ) );
            BOOST_REQUIRE( new_bob_comment.xtt_rshares.find( alice_symbol )->second.net_rshares == old_xtt_net_rshares - old_xtt_vote_rshares + new_xtt_rshares );
            BOOST_REQUIRE( new_bob_comment.xtt_rshares.find( alice_symbol )->second.abs_rshares == old_xtt_abs_rshares + new_xtt_rshares );
            BOOST_REQUIRE( alice_bob_vote->rshares == new_xtt_rshares );
            BOOST_REQUIRE( alice_bob_vote->last_update == db->head_block_time() );
            validate_database();
         }


         BOOST_TEST_MESSAGE( "--- Test changing a vote to 0 rshares (aka: removing a vote)" );
         {
            generate_blocks( db->head_block_time() + XGT_MIN_VOTE_INTERVAL_SEC );

            const auto& new_alice = db->get_account( "alice" );
            const auto& new_bob_comment = db->get_comment( "bob", string( "foo" ) );
            auto old_net_rshares = new_bob_comment.net_rshares.value;
            auto old_abs_rshares = new_bob_comment.abs_rshares.value;
            int64_t old_vote_rshares = op.rshares[ XGT_SYMBOL ] - XGT_VOTE_DUST_THRESHOLD;

            auto old_xtt_net_rshares = new_bob_comment.xtt_rshares.find( alice_symbol )->second.net_rshares.value;
            auto old_xtt_abs_rshares = new_bob_comment.xtt_rshares.find( alice_symbol )->second.abs_rshares.value;
            int64_t old_xtt_vote_rshares = op.rshares[ alice_symbol ] - XGT_VOTE_DUST_THRESHOLD;

            op.rshares[ XGT_SYMBOL ] = 0;
            op.rshares[ alice_symbol ] = 0;
            tx.operations.clear();
            tx.signatures.clear();
            tx.operations.push_back( op );
            sign( tx, alice_private_key );
            db->push_transaction( tx, 0 );
            auto alice_bob_vote = vote_idx.find( boost::make_tuple( new_bob_comment.id, new_alice.id, XGT_SYMBOL ) );

            BOOST_REQUIRE( new_bob_comment.net_rshares == old_net_rshares - old_vote_rshares );
            BOOST_REQUIRE( new_bob_comment.abs_rshares == old_abs_rshares );
            BOOST_REQUIRE( new_bob_comment.cashout_time == new_bob_comment.created + XGT_CASHOUT_WINDOW_SECONDS );
            BOOST_REQUIRE( alice_bob_vote->rshares == op.rshares[ XGT_SYMBOL ] );
            BOOST_REQUIRE( alice_bob_vote->last_update == db->head_block_time() );

            alice_bob_vote = vote_idx.find( boost::make_tuple( new_bob_comment.id, new_alice.id, alice_symbol ) );
            BOOST_REQUIRE( new_bob_comment.xtt_rshares.find( alice_symbol )->second.net_rshares == old_xtt_net_rshares - old_xtt_vote_rshares );
            BOOST_REQUIRE( new_bob_comment.xtt_rshares.find( alice_symbol )->second.abs_rshares == old_xtt_abs_rshares );
            BOOST_REQUIRE( alice_bob_vote->rshares == op.rshares[ alice_symbol ] );
            BOOST_REQUIRE( alice_bob_vote->last_update == db->head_block_time() );

            validate_database();
         }


         BOOST_TEST_MESSAGE( "--- Test downvote overlap when downvote energy is low" );
         {
            generate_block();

            db_plugin->debug_update( [=]( database& db )
            {
               db.modify( db.get_account( "alice" ), [&]( wallet_object& a )
               {
                  a.downvote_energybar.current_energy /= 30;
                  a.downvote_energybar.last_update_time = db.head_block_time().sec_since_epoch();
               });

               const auto& alice_xtt = db.get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "alice", alice_symbol ) );

               db.modify( alice_xtt, [&]( account_regular_balance_object& a )
               {
                  a.downvote_energybar.current_energy /= 30;
                  a.downvote_energybar.last_update_time = db.head_block_time().sec_since_epoch();
               });
            });

            const auto& new_alice = db->get_account( "alice" );
            const auto& bob_comment = db->get_comment( "bob", string( "foo" ) );
            auto old_net_rshares = bob_comment.net_rshares.value;
            auto old_abs_rshares = bob_comment.abs_rshares.value;

            auto old_xtt_net_rshares = bob_comment.xtt_rshares.find( alice_symbol )->second.net_rshares.value;
            auto old_xtt_abs_rshares = bob_comment.xtt_rshares.find( alice_symbol )->second.abs_rshares.value;

            old_energybar = db->get_account( "alice" ).voting_energybar;
            params.max_energy = util::get_effective_vesting_shares( db->get_account( "alice" ) );
            old_energybar.regenerate_energy( params, db->head_block_time() );

            old_downvote_energybar = db->get_account( "alice" ).downvote_energybar;
            params.max_energy = util::get_effective_vesting_shares( db->get_account( "alice" ) ) / 4;
            old_downvote_energybar.regenerate_energy( params, db->head_block_time() );

            const auto& alice_xtt = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "alice", alice_symbol ) );

            old_xtt_energybar = alice_xtt.voting_energybar;
            params.max_energy = util::get_effective_vesting_shares( alice_xtt );
            old_xtt_energybar.regenerate_energy( params, db->head_block_time() );

            old_xtt_downvote_energybar = alice_xtt.downvote_energybar;
            params.max_energy = util::get_effective_vesting_shares( alice_xtt ) / 4;
            old_xtt_downvote_energybar.regenerate_energy( params, db->head_block_time() );

            op.rshares[ XGT_SYMBOL ] = -1 * old_energybar.current_energy / 50;
            op.rshares[ alice_symbol ] = -1 * old_xtt_energybar.current_energy / 50;
            tx.operations.clear();
            tx.signatures.clear();
            tx.operations.push_back( op );
            sign( tx, alice_private_key );
            db->push_transaction( tx, 0 );
            auto alice_bob_vote = vote_idx.find( boost::make_tuple( bob_comment.id, new_alice.id, XGT_SYMBOL ) );

            int64_t new_rshares = op.rshares[ XGT_SYMBOL ] + XGT_VOTE_DUST_THRESHOLD;
            int64_t new_xtt_rshares = op.rshares[ alice_symbol ] + XGT_VOTE_DUST_THRESHOLD;

            BOOST_REQUIRE( bob_comment.net_rshares == old_net_rshares + new_rshares );
            BOOST_REQUIRE( bob_comment.abs_rshares == old_abs_rshares - new_rshares );
            BOOST_REQUIRE( alice_bob_vote->rshares == new_rshares );
            BOOST_REQUIRE( alice_bob_vote->last_update == db->head_block_time() );
            BOOST_REQUIRE( alice.downvote_energybar.current_energy == 0 );
            BOOST_REQUIRE( alice.voting_energybar.current_energy == old_energybar.current_energy + op.rshares[ XGT_SYMBOL ] + old_downvote_energybar.current_energy );

            alice_bob_vote = vote_idx.find( boost::make_tuple( bob_comment.id, new_alice.id, alice_symbol ) );

            BOOST_REQUIRE( bob_comment.xtt_rshares.find( alice_symbol )->second.net_rshares == old_xtt_net_rshares + new_xtt_rshares );
            BOOST_REQUIRE( bob_comment.xtt_rshares.find( alice_symbol )->second.abs_rshares == old_xtt_abs_rshares - new_xtt_rshares );
            BOOST_REQUIRE( alice_bob_vote->rshares == new_xtt_rshares );
            BOOST_REQUIRE( alice_bob_vote->last_update == db->head_block_time() );
            BOOST_REQUIRE( alice_xtt.downvote_energybar.current_energy == 0 );
            BOOST_REQUIRE( alice_xtt.voting_energybar.current_energy == old_xtt_energybar.current_energy + op.rshares[ alice_symbol ] + old_xtt_downvote_energybar.current_energy );

            validate_database();
         }


         BOOST_TEST_MESSAGE( "--- Test reduced effectiveness when increasing rshares within lockout period" );
         {
            const auto& bob_comment = db->get_comment( "bob", string( "foo" ) );
            generate_blocks( fc::time_point_sec( ( bob_comment.cashout_time - XGT_UPVOTE_LOCKOUT_HF17 ).sec_since_epoch() + XGT_BLOCK_INTERVAL ), true );

            old_energybar = db->get_account( "dave" ).voting_energybar;
            params.max_energy = util::get_effective_vesting_shares( db->get_account( "dave" ) );
            old_energybar.regenerate_energy( params, db->head_block_time() );

            const auto& dave_xtt = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "dave", alice_symbol ) );
            old_xtt_energybar = dave_xtt.voting_energybar;
            params.max_energy = util::get_effective_vesting_shares( dave_xtt );
            old_xtt_energybar.regenerate_energy( params, db->head_block_time() );

            op.voter = "dave";
            op.rshares[ XGT_SYMBOL ] = old_energybar.current_energy / 50;
            op.rshares[ alice_symbol ] = old_xtt_energybar.current_energy / 50;
            tx.operations.clear();
            tx.signatures.clear();
            tx.operations.push_back( op );
            sign( tx, dave_private_key );
            db->push_transaction( tx, 0 );

            int64_t new_rshares = op.rshares[ XGT_SYMBOL ] - XGT_VOTE_DUST_THRESHOLD;
            new_rshares = ( new_rshares * ( XGT_UPVOTE_LOCKOUT_SECONDS - XGT_BLOCK_INTERVAL ) ) / XGT_UPVOTE_LOCKOUT_SECONDS;

            int64_t new_xtt_rshares = op.rshares[ alice_symbol ] - XGT_VOTE_DUST_THRESHOLD;
            new_xtt_rshares = ( new_xtt_rshares * ( XGT_UPVOTE_LOCKOUT_SECONDS - XGT_BLOCK_INTERVAL ) ) / XGT_UPVOTE_LOCKOUT_SECONDS;

            account_id_type dave_id = db->get_account( "dave" ).id;
            comment_id_type bob_comment_id = db->get_comment( "bob", string( "foo" ) ).id;

            {
               auto dave_bob_vote = db->get< comment_vote_object, by_comment_voter_symbol >( boost::make_tuple( bob_comment_id, dave_id, XGT_SYMBOL ) );
               BOOST_REQUIRE( dave_bob_vote.rshares = new_rshares );
            }

            {
               auto dave_bob_vote = db->get< comment_vote_object, by_comment_voter_symbol >( boost::make_tuple( bob_comment_id, dave_id, alice_symbol ) );
               BOOST_REQUIRE( dave_bob_vote.rshares = new_xtt_rshares );
            }
            validate_database();
         }

         BOOST_TEST_MESSAGE( "--- Test reduced effectiveness when reducing rshares within lockout period" );
         {
            generate_block();
            old_energybar = db->get_account( "dave" ).voting_energybar;
            params.max_energy = util::get_effective_vesting_shares( db->get_account( "dave" ) );
            old_energybar.regenerate_energy( params, db->head_block_time() );

            const auto& dave_xtt = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "dave", alice_symbol ) );
            old_xtt_energybar = dave_xtt.voting_energybar;
            params.max_energy = util::get_effective_vesting_shares( dave_xtt );
            old_xtt_energybar.regenerate_energy( params, db->head_block_time() );

            op.rshares[ XGT_SYMBOL ] = -1 * old_energybar.current_energy / 50;
            op.rshares[ alice_symbol ] = -1 * old_xtt_energybar.current_energy / 50;
            tx.operations.clear();
            tx.signatures.clear();
            tx.operations.push_back( op );
            sign( tx, dave_private_key );
            db->push_transaction( tx, 0 );

            int64_t new_rshares = op.rshares[ XGT_SYMBOL ] + XGT_VOTE_DUST_THRESHOLD;
            new_rshares = ( new_rshares * ( XGT_UPVOTE_LOCKOUT_SECONDS - XGT_BLOCK_INTERVAL - XGT_BLOCK_INTERVAL ) ) / XGT_UPVOTE_LOCKOUT_SECONDS;

            int64_t new_xtt_rshares = op.rshares[ alice_symbol ] - XGT_VOTE_DUST_THRESHOLD;
            new_xtt_rshares = ( new_xtt_rshares * ( XGT_UPVOTE_LOCKOUT_SECONDS - XGT_BLOCK_INTERVAL ) ) / XGT_UPVOTE_LOCKOUT_SECONDS;

            account_id_type dave_id = db->get_account( "dave" ).id;
            comment_id_type bob_comment_id = db->get_comment( "bob", string( "foo" ) ).id;

            {
               auto dave_bob_vote = db->get< comment_vote_object, by_comment_voter_symbol >( boost::make_tuple( bob_comment_id, dave_id, XGT_SYMBOL ) );
               BOOST_REQUIRE( dave_bob_vote.rshares = new_rshares );
            }

            {
               auto dave_bob_vote = db->get< comment_vote_object, by_comment_voter_symbol >( boost::make_tuple( bob_comment_id, dave_id, alice_symbol ) );
               BOOST_REQUIRE( dave_bob_vote.rshares = new_xtt_rshares );
            }
            validate_database();
         }

         BOOST_TEST_MESSAGE( "--- Test energy charge when increasing vote" );
         {
            comment_op.author = "dave";
            comment_opts.author = "dave";
            op.voter = "sam";
            op.author = "dave";
            op.rshares[ XGT_SYMBOL ] = db->get_account( "sam" ).voting_energybar.current_energy * 3 / 50 / 4;
            op.rshares[ alice_symbol ] = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "sam", alice_symbol ) ).voting_energybar.current_energy * 3 / 50 / 4;
            tx.clear();
            tx.operations.push_back( comment_op );
            tx.operations.push_back( comment_opts );
            tx.operations.push_back( op );
            sign( tx, sam_private_key );
            sign( tx, dave_private_key );
            db->push_transaction( tx, 0 );

            generate_block();
            const auto& sam = db->get_account( "sam" );
            old_energybar = sam.voting_energybar;
            params.max_energy = util::get_effective_vesting_shares( sam );
            old_energybar.regenerate_energy( params, db->head_block_time() );

            const auto& sam_xtt = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "sam", alice_symbol ) );
            old_xtt_energybar = sam_xtt.voting_energybar;
            params.max_energy = util::get_effective_vesting_shares( sam_xtt );
            old_xtt_energybar.regenerate_energy( params, db->head_block_time() );

            auto delta = sam.voting_energybar.current_energy / 50 - op.rshares[ XGT_SYMBOL ];
            auto xtt_delta = sam_xtt.voting_energybar.current_energy / 50 - op.rshares[ alice_symbol ];

            op.rshares[ XGT_SYMBOL ] += delta;
            op.rshares[ alice_symbol ] += xtt_delta;
            tx.clear();
            tx.operations.push_back( op );
            sign( tx, sam_private_key );
            db->push_transaction( tx, 0 );

            BOOST_REQUIRE( sam.voting_energybar.current_energy == old_energybar.current_energy - delta );
            BOOST_REQUIRE( sam_xtt.voting_energybar.current_energy == old_xtt_energybar.current_energy - xtt_delta );
         }

         BOOST_TEST_MESSAGE( "--- Test energy charge when decreasing vote" );
         {
            generate_block();
            const auto& sam = db->get_account( "sam" );
            old_energybar = sam.voting_energybar;
            params.max_energy = util::get_effective_vesting_shares( sam );
            old_energybar.regenerate_energy( params, db->head_block_time() );

            const auto& sam_xtt = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "sam", alice_symbol ) );
            old_xtt_energybar = sam_xtt.voting_energybar;
            params.max_energy = util::get_effective_vesting_shares( sam_xtt );
            old_xtt_energybar.regenerate_energy( params, db->head_block_time() );

            op.rshares[ XGT_SYMBOL ] = sam.voting_energybar.current_energy / 50 / 2;
            op.rshares[ alice_symbol ] = sam_xtt.voting_energybar.current_energy / 50 / 2;

            tx.clear();
            tx.operations.push_back( op );
            sign( tx, sam_private_key );
            db->push_transaction( tx, 0 );

            BOOST_REQUIRE( sam.voting_energybar.current_energy == old_energybar.current_energy );
            BOOST_REQUIRE( sam_xtt.voting_energybar.current_energy == old_xtt_energybar.current_energy );
         }

         BOOST_TEST_MESSAGE( "--- Test energy charge when changing to downvote" );
         {
            generate_block();
            const auto& sam = db->get_account( "sam" );
            old_energybar = sam.voting_energybar;
            params.max_energy = util::get_effective_vesting_shares( sam );
            old_energybar.regenerate_energy( params, db->head_block_time() );

            old_downvote_energybar = sam.downvote_energybar;
            params.max_energy = util::get_effective_vesting_shares( sam ) / 4;
            old_downvote_energybar.regenerate_energy( params, db->head_block_time() );

            const auto& sam_xtt = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "sam", alice_symbol ) );
            old_xtt_energybar = sam_xtt.voting_energybar;
            params.max_energy = util::get_effective_vesting_shares( sam_xtt );
            old_xtt_energybar.regenerate_energy( params, db->head_block_time() );

            old_xtt_downvote_energybar = sam_xtt.downvote_energybar;
            params.max_energy = util::get_effective_vesting_shares( sam_xtt ) / 4;
            old_xtt_downvote_energybar.regenerate_energy( params, db->head_block_time() );

            op.rshares[ XGT_SYMBOL ] = -1 * sam.downvote_energybar.current_energy * 4 / 50 / 2;
            op.rshares[ alice_symbol ] = -1 * sam_xtt.downvote_energybar.current_energy * 4 / 50 / 2;
            tx.clear();
            tx.operations.push_back( op );
            sign( tx, sam_private_key );
            db->push_transaction( tx, 0 );

            BOOST_REQUIRE( sam.voting_energybar.current_energy == old_energybar.current_energy );
            BOOST_REQUIRE( sam.downvote_energybar.current_energy == old_downvote_energybar.current_energy + op.rshares[ XGT_SYMBOL ] );
            BOOST_REQUIRE( sam_xtt.voting_energybar.current_energy == old_xtt_energybar.current_energy );
            BOOST_REQUIRE( sam_xtt.downvote_energybar.current_energy == old_xtt_downvote_energybar.current_energy + op.rshares[ alice_symbol ] );
         }

         BOOST_TEST_MESSAGE( "--- Test energy charge when increasing downvote" );
         {
            generate_block();
            const auto& sam = db->get_account( "sam" );
            old_downvote_energybar = sam.downvote_energybar;
            params.max_energy = util::get_effective_vesting_shares( sam ) / 4;
            old_downvote_energybar.regenerate_energy( params, db->head_block_time() );

            const auto& sam_xtt = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "sam", alice_symbol ) );
            old_xtt_downvote_energybar = sam_xtt.downvote_energybar;
            params.max_energy = util::get_effective_vesting_shares( sam_xtt ) / 4;
            old_xtt_downvote_energybar.regenerate_energy( params, db->head_block_time() );

            auto delta = -1 * sam.downvote_energybar.current_energy * 4 / 50 - op.rshares[ XGT_SYMBOL ];
            auto xtt_delta = -1 * sam_xtt.downvote_energybar.current_energy * 4 / 50 - op.rshares[ alice_symbol ];

            op.rshares[ XGT_SYMBOL ] += delta;
            op.rshares[ alice_symbol ] += xtt_delta;
            tx.clear();
            tx.operations.push_back( op );
            sign( tx, sam_private_key );
            db->push_transaction( tx, 0 );

            BOOST_REQUIRE( sam.downvote_energybar.current_energy == old_downvote_energybar.current_energy + delta );
            BOOST_REQUIRE( sam_xtt.downvote_energybar.current_energy == old_xtt_downvote_energybar.current_energy + xtt_delta );
         }

         BOOST_TEST_MESSAGE( "--- Test energy charge when decreasing downvote" );
         {
            generate_block();
            const auto& sam = db->get_account( "sam" );
            old_downvote_energybar = sam.downvote_energybar;
            params.max_energy = util::get_effective_vesting_shares( sam ) / 4;
            old_downvote_energybar.regenerate_energy( params, db->head_block_time() );

            const auto& sam_xtt = db->get< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( "sam", alice_symbol ) );
            old_xtt_downvote_energybar = sam_xtt.downvote_energybar;
            params.max_energy = util::get_effective_vesting_shares( sam_xtt ) / 4;
            old_xtt_downvote_energybar.regenerate_energy( params, db->head_block_time() );

            op.rshares[ XGT_SYMBOL ] = sam.downvote_energybar.current_energy * 4 / 50 / 8;
            op.rshares[ alice_symbol ] = sam_xtt.downvote_energybar.current_energy * 4 / 50 / 8;

            tx.clear();
            tx.operations.push_back( op );
            sign( tx, sam_private_key );
            db->push_transaction( tx, 0 );

            BOOST_REQUIRE( sam.downvote_energybar.current_energy == old_downvote_energybar.current_energy );
            BOOST_REQUIRE( sam_xtt.downvote_energybar.current_energy == old_xtt_downvote_energybar.current_energy );
         }
      }
   }
   FC_LOG_AND_RETHROW()
}*/
/*
BOOST_AUTO_TEST_CASE( xtt_setup_ico_tier_validate )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_setup_ico_tier_operation validate" );

      ACTORS( (alice) );
      auto symbol = create_xtt( "alice", alice_private_key, 3 );

      xtt_setup_ico_tier_operation op;
      op.control_account = "alice";
      op.symbol = symbol;
      op.xgt_units_cap = 0;

      xtt_capped_generation_policy valid_capped_generation_policy;
      valid_capped_generation_policy.generation_unit.xgt_unit[ "alice" ] = 2;
      valid_capped_generation_policy.generation_unit.xgt_unit[ "$!alice.vesting" ] = 2;
      valid_capped_generation_policy.generation_unit.xgt_unit[ XTT_DESTINATION_MARKET_MAKER ] = 2;

      valid_capped_generation_policy.generation_unit.token_unit[ "alice" ] = 2;
      valid_capped_generation_policy.generation_unit.token_unit[ "$!alice.vesting" ] = 2;
      valid_capped_generation_policy.generation_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] = 2;
      valid_capped_generation_policy.generation_unit.token_unit[ XTT_DESTINATION_REWARDS ] = 2;
      valid_capped_generation_policy.generation_unit.token_unit[ XTT_DESTINATION_FROM ] = 2;
      valid_capped_generation_policy.generation_unit.token_unit[ XTT_DESTINATION_FROM_VESTING ] = 2;

      op.generation_policy = valid_capped_generation_policy;

      op.validate();

      BOOST_TEST_MESSAGE( " -- Failure on invalid control acount name" );
      op.control_account = "@@@@";
      BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );
      op.control_account = "alice";

      BOOST_TEST_MESSAGE( " -- Failure on negative xgt_units_cap" );
      op.xgt_units_cap = -1;
      BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );

      op.xgt_units_cap = 0;
      BOOST_TEST_MESSAGE( " -- Success on xgt_units_cap" );
      op.validate();

      op.xgt_units_cap = 1000;
      BOOST_TEST_MESSAGE( " -- Success on positive xgt_units_cap" );
      op.validate();

      BOOST_TEST_MESSAGE( " -- Failure on XTT_DESTINATION_REWARDS in xgt_unit" );

      xtt_capped_generation_policy invalid_capped_generation_policy = valid_capped_generation_policy;
      invalid_capped_generation_policy.generation_unit.xgt_unit[ XTT_DESTINATION_REWARDS ] = 1;

      op.generation_policy = invalid_capped_generation_policy;

      BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Failure on XTT_DESTINATION_FROM in xgt_unit" );

      invalid_capped_generation_policy = valid_capped_generation_policy;
      invalid_capped_generation_policy.generation_unit.xgt_unit[ XTT_DESTINATION_FROM ] = 1;

      op.generation_policy = invalid_capped_generation_policy;

      BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Failure on XTT_DESTINATION_FROM_VESTING in xgt_unit" );

      invalid_capped_generation_policy = valid_capped_generation_policy;
      invalid_capped_generation_policy.generation_unit.xgt_unit[ XTT_DESTINATION_FROM_VESTING ] = 1;

      op.generation_policy = invalid_capped_generation_policy;

      BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Failure on XTT_DESTINATION_VESTING in xgt_unit" );

      invalid_capped_generation_policy = valid_capped_generation_policy;
      invalid_capped_generation_policy.generation_unit.xgt_unit[ XTT_DESTINATION_VESTING ] = 1;

      op.generation_policy = invalid_capped_generation_policy;

      BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Failure on XTT_DESTINATION_VESTING in token_unit" );

      invalid_capped_generation_policy = valid_capped_generation_policy;
      invalid_capped_generation_policy.generation_unit.token_unit[ XTT_DESTINATION_VESTING ] = 1;

      op.generation_policy = invalid_capped_generation_policy;

      BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Failure on malformed special destination in xgt unit" );

      invalid_capped_generation_policy = valid_capped_generation_policy;
      invalid_capped_generation_policy.generation_unit.xgt_unit[ "$market_malformed" ] = 1;

      op.generation_policy = invalid_capped_generation_policy;

      BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Failure on malformed special destination in token unit" );

      invalid_capped_generation_policy = valid_capped_generation_policy;
      invalid_capped_generation_policy.generation_unit.token_unit[ "$market_malformed" ] = 1;

      op.generation_policy = invalid_capped_generation_policy;

      BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Success on having an empty token unit" );

      invalid_capped_generation_policy = valid_capped_generation_policy;
      invalid_capped_generation_policy.generation_unit.token_unit.clear();

      op.generation_policy = invalid_capped_generation_policy;

      op.validate();

      BOOST_TEST_MESSAGE( " -- Failure on empty xgt unit" );

      invalid_capped_generation_policy = valid_capped_generation_policy;
      invalid_capped_generation_policy.generation_unit.xgt_unit.clear();

      op.generation_policy = invalid_capped_generation_policy;

      BOOST_REQUIRE_THROW( op.validate(), fc::assert_exception );

      BOOST_TEST_MESSAGE( " -- Sanity check" );
      op.generation_policy = valid_capped_generation_policy;
      op.validate();
   }
   FC_LOG_AND_RETHROW()
}*/
/*
BOOST_AUTO_TEST_CASE( xtt_setup_ico_tier_authorities )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_setup_ico_tier_operation authorities" );

      xtt_setup_ico_tier_operation op;
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

BOOST_AUTO_TEST_CASE( xtt_setup_ico_tier_apply )
{
   try
   {
      BOOST_TEST_MESSAGE( "Testing: xtt_setup_ico_tier_operation apply" );
      ACTORS( (alice)(bob)(sam)(dave) )

      generate_block();

      BOOST_TEST_MESSAGE( " -- XTT creation" );
      auto symbol = create_xtt( "alice", alice_private_key, 3 );
      signed_transaction tx;

      generate_block();

      xtt_setup_ico_tier_operation op;
      op.control_account = "alice";
      op.symbol = symbol;
      op.xgt_units_cap = 0;

      xtt_capped_generation_policy valid_capped_generation_policy;
      valid_capped_generation_policy.generation_unit.xgt_unit[ "alice" ] = 2;
      valid_capped_generation_policy.generation_unit.xgt_unit[ "$!alice.vesting" ] = 2;
      valid_capped_generation_policy.generation_unit.xgt_unit[ XTT_DESTINATION_MARKET_MAKER ] = 2;

      valid_capped_generation_policy.generation_unit.token_unit[ "alice" ] = 2;
      valid_capped_generation_policy.generation_unit.token_unit[ "$!alice.vesting" ] = 2;
      valid_capped_generation_policy.generation_unit.token_unit[ XTT_DESTINATION_MARKET_MAKER ] = 2;
      valid_capped_generation_policy.generation_unit.token_unit[ XTT_DESTINATION_REWARDS ] = 2;
      valid_capped_generation_policy.generation_unit.token_unit[ XTT_DESTINATION_FROM ] = 2;
      valid_capped_generation_policy.generation_unit.token_unit[ XTT_DESTINATION_FROM_VESTING ] = 2;

      xtt_capped_generation_policy invalid_capped_generation_policy;
      invalid_capped_generation_policy = valid_capped_generation_policy;
      invalid_capped_generation_policy.generation_unit.xgt_unit[ "elaine" ] = 2;

      op.generation_policy = invalid_capped_generation_policy;

      BOOST_TEST_MESSAGE( " -- Failure on non-existent account (elaine) in xgt unit" );
      tx.operations.push_back( op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
      tx.operations.clear();
      tx.signatures.clear();

      invalid_capped_generation_policy = valid_capped_generation_policy;
      invalid_capped_generation_policy.generation_unit.token_unit[ "elaine" ] = 2;

      op.generation_policy = invalid_capped_generation_policy;

      BOOST_TEST_MESSAGE( " -- Failure on non-existent account (elaine) in token unit" );
      tx.operations.push_back( op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
      tx.operations.clear();
      tx.signatures.clear();

      invalid_capped_generation_policy = valid_capped_generation_policy;
      invalid_capped_generation_policy.generation_unit.xgt_unit[ "$!elaine.vesting" ] = 2;

      op.generation_policy = invalid_capped_generation_policy;


      BOOST_TEST_MESSAGE( " -- Failure on non-existent founder vesting account (elaine) in xgt unit" );
      tx.operations.push_back( op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
      tx.operations.clear();
      tx.signatures.clear();

      invalid_capped_generation_policy = valid_capped_generation_policy;
      invalid_capped_generation_policy.generation_unit.token_unit[ "$!elaine.vesting" ] = 2;

      op.generation_policy = invalid_capped_generation_policy;

      BOOST_TEST_MESSAGE( " -- Failure on non-existent founder vesting account (elaine) in token unit" );
      tx.operations.push_back( op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
      tx.operations.clear();
      tx.signatures.clear();

      op.generation_policy = valid_capped_generation_policy;
      op.control_account = "sam";

      BOOST_TEST_MESSAGE( " -- Failure to setup XTT ICO tier with non controlling account" );
      tx.operations.push_back( op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, sam_private_key );
      BOOST_REQUIRE_THROW( db->push_transaction( tx, 0 ), fc::assert_exception );
      tx.operations.clear();
      tx.signatures.clear();

      op.control_account = "alice";

      BOOST_TEST_MESSAGE( " -- Success while setting up XTT ICO tier with valid generation policy and control account" );
      tx.operations.push_back( op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );
      tx.operations.clear();
      tx.signatures.clear();

      BOOST_REQUIRE( util::xtt::ico::ico_tier_size( *db, symbol ) == 1 );

      BOOST_TEST_MESSAGE( " -- Success while removing an XTT ICO tier" );

      op.remove = true;
      tx.operations.push_back( op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );
      tx.operations.clear();
      tx.signatures.clear();

      BOOST_REQUIRE( util::xtt::ico::ico_tier_size( *db, symbol ) == 0 );

      BOOST_TEST_MESSAGE( " -- Success re-adding an XTT ICO tier" );

      op.remove = false;
      tx.operations.push_back( op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION - 1 );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );
      tx.operations.clear();
      tx.signatures.clear();

      BOOST_REQUIRE( util::xtt::ico::ico_tier_size( *db, symbol ) == 1 );

      BOOST_TEST_MESSAGE( " -- XTT setup" );
      xtt_setup_operation setup_op;

      setup_op.control_account = "alice";
      setup_op.symbol = symbol;
      setup_op.contribution_begin_time = db->head_block_time() + XGT_BLOCK_INTERVAL;
      setup_op.contribution_end_time   = setup_op.contribution_begin_time + fc::days( 30 );
      setup_op.xgt_units_min         = 0;
      setup_op.min_unit_ratio = 50;
      setup_op.max_unit_ratio = 100;
      setup_op.max_supply = XGT_MAX_SHARE_SUPPLY;
      setup_op.launch_time = setup_op.contribution_end_time + fc::days( 1 );

      setup_op.control_account = "alice";

      BOOST_TEST_MESSAGE( " -- Success on valid XTT setup" );
      tx.operations.push_back( setup_op );
      tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( tx, alice_private_key );
      db->push_transaction( tx, 0 );
      tx.operations.clear();
      tx.signatures.clear();
   }
   FC_LOG_AND_RETHROW()
}
*/
BOOST_AUTO_TEST_SUITE_END()
#endif
