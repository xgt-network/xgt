#include <boost/test/unit_test.hpp>
#include <boost/program_options.hpp>

#include <xgt/utilities/tempdir.hpp>
#include <xgt/utilities/database_configuration.hpp>

#include <xgt/chain/history_object.hpp>
#include <xgt/chain/xgt_objects.hpp>

#include <xgt/plugins/wallet_history/wallet_history_plugin.hpp>
#include <xgt/plugins/chain/chain_plugin.hpp>
#include <xgt/plugins/webserver/webserver_plugin.hpp>
#include <xgt/plugins/witness/witness_plugin.hpp>

#include <xgt/chain/xtt_objects/nai_pool_object.hpp>

#include <fc/crypto/digest.hpp>
#include <fc/smart_ref_impl.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>

#include "database_fixture.hpp"

//using namespace xgt::chain::test;

uint32_t XGT_TESTING_GENESIS_TIMESTAMP = 1431700000;

using namespace xgt::plugins::webserver;
using namespace xgt::plugins::database_api;
using namespace xgt::plugins::block_api;

namespace xgt { namespace chain {

using std::cout;
using std::cerr;

clean_database_fixture::clean_database_fixture( uint16_t shared_file_size_in_mb )
{
   try {
   int argc = boost::unit_test::framework::master_test_suite().argc;
   char** argv = boost::unit_test::framework::master_test_suite().argv;
   for( int i=1; i<argc; i++ )
   {
      const std::string arg = argv[i];
      if( arg == "--record-assert-trip" )
         fc::enable_record_assert_trip = true;
      if( arg == "--show-test-names" )
         std::cout << "running test " << boost::unit_test::framework::current_test_case().p_name << std::endl;
   }

   appbase::app().register_plugin< xgt::plugins::wallet_history::wallet_history_plugin >();
   db_plugin = &appbase::app().register_plugin< xgt::plugins::debug_node::debug_node_plugin >();
   appbase::app().register_plugin< xgt::plugins::witness::witness_plugin >();

   db_plugin->logging = false;
   appbase::app().initialize<
      xgt::plugins::wallet_history::wallet_history_plugin,
      xgt::plugins::debug_node::debug_node_plugin,
      xgt::plugins::witness::witness_plugin
      >( argc, argv );

   db = &appbase::app().get_plugin< xgt::plugins::chain::chain_plugin >().db();
   BOOST_REQUIRE( db );

   init_account_pub_key = init_account_priv_key.get_public_key();

   open_database( shared_file_size_in_mb );

   generate_block();
   db->set_hardfork( XGT_BLOCKCHAIN_VERSION.minor_v() );
   generate_block();

   vest( "initminer", 10000 );

   /*
   // Fill up the rest of the required miners
   for( int i = XGT_NUM_INIT_MINERS; i < XGT_MAX_WITNESSES; i++ )
   {
      account_create( XGT_INIT_MINER_NAME + fc::to_string( i ), init_account_pub_key );
      fund( XGT_INIT_MINER_NAME + fc::to_string( i ), XGT_MIN_PRODUCER_REWARD.amount.value );
      witness_create( XGT_INIT_MINER_NAME + fc::to_string( i ), init_account_priv_key, "foo.bar", init_account_pub_key, XGT_MIN_PRODUCER_REWARD.amount );
   }
   */

   validate_database();
   } catch ( const fc::exception& e )
   {
      edump( (e.to_detail_string()) );
      throw;
   }

   return;
}

clean_database_fixture::~clean_database_fixture()
{ try {
   // If we're unwinding due to an exception, don't do any more checks.
   // This way, boost test's last checkpoint tells us approximately where the error was.
   if( !std::uncaught_exception() )
   {
      BOOST_CHECK( db->get_node_properties().skip_flags == database::skip_nothing );
   }

   if( data_dir )
      db->wipe( data_dir->path(), data_dir->path(), true );
   return;
} FC_CAPTURE_AND_LOG( () )
   exit(1);
}

void clean_database_fixture::validate_database()
{
   database_fixture::validate_database();
}

void clean_database_fixture::resize_shared_mem( uint64_t size )
{
   db->wipe( data_dir->path(), data_dir->path(), true );
   int argc = boost::unit_test::framework::master_test_suite().argc;
   char** argv = boost::unit_test::framework::master_test_suite().argv;
   for( int i=1; i<argc; i++ )
   {
      const std::string arg = argv[i];
      if( arg == "--record-assert-trip" )
         fc::enable_record_assert_trip = true;
      if( arg == "--show-test-names" )
         std::cout << "running test " << boost::unit_test::framework::current_test_case().p_name << std::endl;
   }
   init_account_pub_key = init_account_priv_key.get_public_key();

   {
      database::open_args args;
      args.data_dir = data_dir->path();
      args.shared_mem_dir = args.data_dir;
      args.initial_supply = INITIAL_TEST_SUPPLY;
      args.shared_file_size = size;
      args.database_cfg = xgt::utilities::default_database_configuration();
      db->open( args );
   }

   boost::program_options::variables_map options;


   generate_block();
   db->set_hardfork( XGT_BLOCKCHAIN_VERSION.minor_v() );
   generate_block();

   vest( "initminer", 10000 );

   /*
   // Fill up the rest of the required miners
   for( int i = XGT_NUM_INIT_MINERS; i < XGT_MAX_WITNESSES; i++ )
   {
      account_create( XGT_INIT_MINER_NAME + fc::to_string( i ), init_account_pub_key );
      fund( XGT_INIT_MINER_NAME + fc::to_string( i ), XGT_MIN_PRODUCER_REWARD.amount.value );
      witness_create( XGT_INIT_MINER_NAME + fc::to_string( i ), init_account_priv_key, "foo.bar", init_account_pub_key, XGT_MIN_PRODUCER_REWARD.amount );
   }
   */

   validate_database();
}

live_database_fixture::live_database_fixture()
{
   try
   {
      int argc = boost::unit_test::framework::master_test_suite().argc;
      char** argv = boost::unit_test::framework::master_test_suite().argv;

      ilog( "Loading saved chain" );
      _chain_dir = fc::current_path() / "test_blockchain";
      FC_ASSERT( fc::exists( _chain_dir ), "Requires blockchain to test on in ./test_blockchain" );

      appbase::app().register_plugin< xgt::plugins::wallet_history::wallet_history_plugin >();
      appbase::app().initialize<
         xgt::plugins::wallet_history::wallet_history_plugin
         >( argc, argv );

      db = &appbase::app().get_plugin< xgt::plugins::chain::chain_plugin >().db();
      BOOST_REQUIRE( db );

      {
         database::open_args args;
         args.data_dir = _chain_dir;
         args.shared_mem_dir = args.data_dir;
         args.database_cfg = xgt::utilities::default_database_configuration();
         db->open( args );
      }

      validate_database();
      generate_block();

      ilog( "Done loading saved chain" );
   }
   FC_LOG_AND_RETHROW()
}

live_database_fixture::~live_database_fixture()
{
   try
   {
      // If we're unwinding due to an exception, don't do any more checks.
      // This way, boost test's last checkpoint tells us approximately where the error was.
      if( !std::uncaught_exception() )
      {
         BOOST_CHECK( db->get_node_properties().skip_flags == database::skip_nothing );
      }

      db->pop_block();
      db->close();
      return;
   }
   FC_CAPTURE_AND_LOG( () )
   exit(1);
}

fc::ecc::private_key database_fixture::generate_private_key(string seed)
{
   static const fc::ecc::private_key committee = fc::ecc::private_key::regenerate( fc::sha256::hash( string( "init_key" ) ) );
   if( seed == "init_key" )
      return committee;
   return fc::ecc::private_key::regenerate( fc::sha256::hash( seed ) );
}

asset_symbol_type database_fixture::get_new_xtt_symbol( uint8_t token_decimal_places, chain::database* db )
{
   // The list of available nais is not dependent on XTT desired precision (token_decimal_places).
   static std::vector< asset_symbol_type >::size_type next_nai = 0;
   auto available_nais = db->get< nai_pool_object >().pool();
   FC_ASSERT( available_nais.size() > 0, "No available nai returned by get_nai_pool." );
   const asset_symbol_type& new_nai = available_nais[ next_nai++ % available_nais.size() ];
   // Note that token's precision is needed now, when creating actual symbol.
   return asset_symbol_type::from_nai( new_nai.to_nai(), token_decimal_places );
}

void database_fixture::open_database( uint16_t shared_file_size_in_mb )
{
   if( !data_dir )
   {
      data_dir = fc::temp_directory( xgt::utilities::temp_directory_path() );
      db->_log_hardforks = false;

      idump( (data_dir->path()) );

      database::open_args args;
      args.data_dir = data_dir->path();
      args.shared_mem_dir = args.data_dir;
      args.initial_supply = INITIAL_TEST_SUPPLY;
      args.shared_file_size = 1024 * 1024 * shared_file_size_in_mb; // 8MB(default) or more:  file for testing
      args.database_cfg = xgt::utilities::default_database_configuration();
      //args.benchmark_is_enabled = true;
      db->open(args);
   }
   else
   {
      idump( (data_dir->path()) );
   }
}

void database_fixture::generate_block(uint32_t skip, const fc::ecc::private_key& key, int miss_blocks)
{
   skip |= default_skip;
   db_plugin->debug_generate_blocks( xgt::utilities::key_to_wif( key ), 1, skip, miss_blocks );
}

void database_fixture::generate_blocks( uint32_t block_count )
{
   auto produced = db_plugin->debug_generate_blocks( debug_key, block_count, default_skip, 0 );
   BOOST_REQUIRE( produced == block_count );
}

void database_fixture::generate_blocks(fc::time_point_sec timestamp, bool miss_intermediate_blocks)
{
   db_plugin->debug_generate_blocks_until( debug_key, timestamp, miss_intermediate_blocks, default_skip );
   BOOST_REQUIRE( ( db->head_block_time() - timestamp ).to_seconds() < XGT_BLOCK_INTERVAL );
}

const account_object& database_fixture::account_create(
   const string& name,
   const string& creator,
   const private_key_type& creator_key,
   const share_type& fee,
   const public_key_type& key,
   const public_key_type& post_key,
   const string& json_metadata
   )
{
   try
   {
      wallet_create_operation op;
      op.new_account_name = name;
      op.creator = creator;
      op.fee = asset( 0, XGT_SYMBOL );
      op.recovery = authority( 1, key, 1 );
      op.money = authority( 1, key, 1 );
      op.social = authority( 1, post_key, 1 );
      op.memo_key = key;
      op.json_metadata = json_metadata;

      trx.operations.push_back( op );

      trx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( trx, creator_key );
      trx.validate();
      db->push_transaction( trx, 0 );
      trx.clear();

      const account_object& acct = db->get_account( name );

      return acct;
   }
   FC_CAPTURE_AND_RETHROW( (name)(creator) )
}

const account_object& database_fixture::account_create(
   const string& name,
   const public_key_type& key,
   const public_key_type& post_key
)
{
   try
   {
      return account_create(
         name,
         XGT_INIT_MINER_NAME,
         init_account_priv_key,
         share_type( 100 ),
         key,
         post_key,
         "" );
   }
   FC_CAPTURE_AND_RETHROW( (name) );
}

const account_object& database_fixture::account_create(
   const string& name,
   const public_key_type& key
)
{
   return account_create( name, key, key );
}

const witness_object& database_fixture::witness_create(
   const string& owner,
   const private_key_type& owner_key,
   const string& url,
   const public_key_type& signing_key,
   const share_type& fee )
{
   try
   {
      witness_update_operation op;
      op.owner = owner;
      op.url = url;
      op.block_signing_key = signing_key;
      op.fee = asset( 0, XGT_SYMBOL );

      trx.operations.push_back( op );
      trx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      sign( trx, owner_key );
      trx.validate();
      db->push_transaction( trx, 0 );
      trx.clear();

      return db->get_witness( owner );
   }
   FC_CAPTURE_AND_RETHROW( (owner)(url) )
}

void database_fixture::fund(
   const string& account_name,
   const share_type& amount
   )
{
}

void database_fixture::fund(
   const string& account_name,
   const asset& amount
   )
{
}

void database_fixture::convert(
   const string& account_name,
   const asset& amount )
{
}

void database_fixture::transfer(
   const string& from,
   const string& to,
   const asset& amount )
{
   try
   {
      transfer_operation op;
      op.from = from;
      op.to = to;
      op.amount = amount;

      trx.operations.push_back( op );
      trx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      trx.validate();

      if( from == XGT_INIT_MINER_NAME )
      {
         sign( trx, init_account_priv_key );
      }

      db->push_transaction( trx, ~0 );
      trx.clear();
   } FC_CAPTURE_AND_RETHROW( (from)(to)(amount) )
}

void database_fixture::vest( const string& from, const string& to, const asset& amount )
{
}

void database_fixture::vest( const string& from, const share_type& amount )
{
}

void database_fixture::vest( const string& from, const asset& amount )
{
}

void database_fixture::proxy( const string& account, const string& proxy )
{
}

void database_fixture::set_price_feed( const price& new_price )
{
}

void database_fixture::set_witness_props( const flat_map< string, vector< char > >& props )
{
   /*
   trx.clear();
   for( size_t i=0; i<XGT_MAX_WITNESSES; i++ )
   {
      witness_set_properties_operation op;
      op.owner = XGT_INIT_MINER_NAME + (i == 0 ? "" : fc::to_string( i ));
      op.props = props;
      if( props.find( "key" ) == props.end() )
         op.props["key"] = fc::raw::pack_to_vector( init_account_pub_key );

      trx.operations.push_back( op );
      trx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      db->push_transaction( trx, ~0 );
      trx.clear();
   }

   const witness_schedule_object* wso = &(db->get_witness_schedule_object());
   uint32_t old_next_shuffle = wso->next_shuffle_block_num;

   for( size_t i=0; i<2*XGT_MAX_WITNESSES+1; i++ )
   {
      generate_block();
      wso = &(db->get_witness_schedule_object());
      if( wso->next_shuffle_block_num != old_next_shuffle )
         return;
   }
   FC_ASSERT( false, "Couldn't apply properties in ${n} blocks", ("n", 2*XGT_MAX_WITNESSES+1) );
   */
}

const asset& database_fixture::get_balance( const string& account_name )const
{
  return db->get_account( account_name ).balance;
}

void database_fixture::sign(signed_transaction& trx, const fc::ecc::private_key& key)
{
   trx.sign( key, db->get_chain_id(), default_sig_canon );
}

vector< operation > database_fixture::get_last_operations( uint32_t num_ops )
{
   vector< operation > ops;
   const auto& acc_hist_idx = db->get_index< account_history_index >().indices().get< by_id >();
   auto itr = acc_hist_idx.end();

   while( itr != acc_hist_idx.begin() && ops.size() < num_ops )
   {
      itr--;
      const buffer_type& _serialized_op = db->get(itr->op).serialized_op;
      std::vector<char> serialized_op;
      serialized_op.reserve( _serialized_op.size() );
      std::copy( _serialized_op.begin(), _serialized_op.end(), std::back_inserter( serialized_op ) );
      ops.push_back( fc::raw::unpack_from_vector< xgt::chain::operation >( serialized_op ) );
   }

   return ops;
}

void database_fixture::validate_database()
{
   try
   {
      db->validate_invariants();
      db->validate_xtt_invariants();
   }
   FC_LOG_AND_RETHROW();
}

asset_symbol_type database_fixture::create_xtt_with_nai( const string& account_name, const fc::ecc::private_key& key,
   uint32_t nai, uint8_t token_decimal_places )
{
   xtt_create_operation op;
   signed_transaction tx;
   try
   {
      fund( account_name, 10 * 1000 * 1000 );
      this->generate_block();

      op.symbol = asset_symbol_type::from_nai( nai, token_decimal_places );
      op.precision = op.symbol.decimals();
      op.xtt_creation_fee = this->db->get_dynamic_global_properties().xtt_creation_fee;
      op.control_account = account_name;

      tx.operations.push_back( op );
      tx.set_expiration( this->db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      tx.sign( key, this->db->get_chain_id(), fc::ecc::bip_0062 );

      this->db->push_transaction( tx, 0 );

      this->generate_block();
   }
   FC_LOG_AND_RETHROW();

   return op.symbol;
}

asset_symbol_type database_fixture::create_xtt( const string& account_name, const fc::ecc::private_key& key,
   uint8_t token_decimal_places )
{
   asset_symbol_type symbol;
   try
   {
      auto nai_symbol = this->get_new_xtt_symbol( token_decimal_places, this->db );
      symbol = create_xtt_with_nai( account_name, key, nai_symbol.to_nai(), token_decimal_places );
   }
   FC_LOG_AND_RETHROW();

   return symbol;
}

void sub_set_create_op( xtt_create_operation* op, account_name_type control_acount, chain::database& db )
{
   op->precision = op->symbol.decimals();
   op->xtt_creation_fee = db.get_dynamic_global_properties().xtt_creation_fee;
   op->control_account = control_acount;
}

void set_create_op( xtt_create_operation* op, account_name_type control_account, uint8_t token_decimal_places, chain::database& db )
{
   op->symbol = database_fixture::get_new_xtt_symbol( token_decimal_places, &db );
   sub_set_create_op( op, control_account, db );
}

void set_create_op( xtt_create_operation* op, account_name_type control_account, uint32_t token_nai, uint8_t token_decimal_places, chain::database& db )
{
   op->symbol.from_nai(token_nai, token_decimal_places);
   sub_set_create_op( op, control_account, db );
}

std::array<asset_symbol_type, 3> database_fixture::create_xtt_3(const char* control_account_name, const fc::ecc::private_key& key)
{
   xtt_create_operation op0;
   xtt_create_operation op1;
   xtt_create_operation op2;

   try
   {
      fund( control_account_name, 10 * 1000 * 1000 );
      this->generate_block();

      set_create_op( &op0, control_account_name, 0, *this->db );
      set_create_op( &op1, control_account_name, 1, *this->db );
      set_create_op( &op2, control_account_name, 1, *this->db );

      signed_transaction tx;
      tx.operations.push_back( op0 );
      tx.operations.push_back( op1 );
      tx.operations.push_back( op2 );
      tx.set_expiration( this->db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
      tx.sign( key, this->db->get_chain_id(), fc::ecc::bip_0062 );
      this->db->push_transaction( tx, 0 );

      this->generate_block();

      std::array<asset_symbol_type, 3> retVal;
      retVal[0] = op0.symbol;
      retVal[1] = op1.symbol;
      retVal[2] = op2.symbol;
      std::sort(retVal.begin(), retVal.end(),
           [](const asset_symbol_type & a, const asset_symbol_type & b) -> bool
      {
         return a.to_nai() < b.to_nai();
      });
      return retVal;
   }
   FC_LOG_AND_RETHROW();
}

void push_invalid_operation(const operation& invalid_op, const fc::ecc::private_key& key, database* db)
{
   signed_transaction tx;
   tx.operations.push_back( invalid_op );
   tx.set_expiration( db->head_block_time() + XGT_MAX_TIME_UNTIL_EXPIRATION );
   tx.sign( key, db->get_chain_id(), fc::ecc::bip_0062 );
   XGT_REQUIRE_THROW( db->push_transaction( tx, database::skip_transaction_dupe_check ), fc::assert_exception );
}

void database_fixture::create_invalid_xtt( const char* control_account_name, const fc::ecc::private_key& key )
{
   // Fail due to precision too big.
   xtt_create_operation op_precision;
   XGT_REQUIRE_THROW( set_create_op( &op_precision, control_account_name, XGT_ASSET_MAX_DECIMALS + 1, *this->db ), fc::assert_exception );
}

void database_fixture::create_conflicting_xtt( const asset_symbol_type existing_xtt, const char* control_account_name,
   const fc::ecc::private_key& key )
{
   // Fail due to the same nai & precision.
   xtt_create_operation op_same;
   set_create_op( &op_same, control_account_name, existing_xtt.to_nai(), existing_xtt.decimals(), *this->db );
   push_invalid_operation( op_same, key, this->db );
   // Fail due to the same nai (though different precision).
   xtt_create_operation op_same_nai;
   set_create_op( &op_same_nai, control_account_name, existing_xtt.to_nai(), existing_xtt.decimals() == 0 ? 1 : 0, *this->db );
   push_invalid_operation (op_same_nai, key, this->db );
}

json_rpc_database_fixture::json_rpc_database_fixture()
{
   try {
   int argc = boost::unit_test::framework::master_test_suite().argc;
   char** argv = boost::unit_test::framework::master_test_suite().argv;
   for( int i=1; i<argc; i++ )
   {
      const std::string arg = argv[i];
      if( arg == "--record-assert-trip" )
         fc::enable_record_assert_trip = true;
      if( arg == "--show-test-names" )
         std::cout << "running test " << boost::unit_test::framework::current_test_case().p_name << std::endl;
   }

   appbase::app().register_plugin< xgt::plugins::wallet_history::wallet_history_plugin >();
   db_plugin = &appbase::app().register_plugin< xgt::plugins::debug_node::debug_node_plugin >();
   appbase::app().register_plugin< xgt::plugins::witness::witness_plugin >();
   rpc_plugin = &appbase::app().register_plugin< xgt::plugins::json_rpc::json_rpc_plugin >();
   appbase::app().register_plugin< xgt::plugins::block_api::block_api_plugin >();
   appbase::app().register_plugin< xgt::plugins::database_api::database_api_plugin >();

   db_plugin->logging = false;
   appbase::app().initialize<
      xgt::plugins::wallet_history::wallet_history_plugin,
      xgt::plugins::debug_node::debug_node_plugin,
      xgt::plugins::json_rpc::json_rpc_plugin,
      xgt::plugins::block_api::block_api_plugin,
      xgt::plugins::database_api::database_api_plugin
      >( argc, argv );

   db = &appbase::app().get_plugin< xgt::plugins::chain::chain_plugin >().db();
   BOOST_REQUIRE( db );

   init_account_pub_key = init_account_priv_key.get_public_key();

   open_database();

   generate_block();
   db->set_hardfork( XGT_BLOCKCHAIN_VERSION.minor_v() );
   generate_block();

   vest( "initminer", 10000 );

   /*
   // Fill up the rest of the required miners
   for( int i = XGT_NUM_INIT_MINERS; i < XGT_MAX_WITNESSES; i++ )
   {
      account_create( XGT_INIT_MINER_NAME + fc::to_string( i ), init_account_pub_key );
      fund( XGT_INIT_MINER_NAME + fc::to_string( i ), XGT_MIN_PRODUCER_REWARD.amount.value );
      witness_create( XGT_INIT_MINER_NAME + fc::to_string( i ), init_account_priv_key, "foo.bar", init_account_pub_key, XGT_MIN_PRODUCER_REWARD.amount );
   }
   */

   validate_database();
   } catch ( const fc::exception& e )
   {
      edump( (e.to_detail_string()) );
      throw;
   }

   return;
}

json_rpc_database_fixture::~json_rpc_database_fixture()
{
   // If we're unwinding due to an exception, don't do any more checks.
   // This way, boost test's last checkpoint tells us approximately where the error was.
   if( !std::uncaught_exception() )
   {
      BOOST_CHECK( db->get_node_properties().skip_flags == database::skip_nothing );
   }

   if( data_dir )
      db->wipe( data_dir->path(), data_dir->path(), true );
   return;
}

fc::variant json_rpc_database_fixture::get_answer( std::string& request )
{
   return fc::json::from_string( rpc_plugin->call( request ) );
}

void check_id_equal( const fc::variant& id_a, const fc::variant& id_b )
{
   BOOST_REQUIRE( id_a.get_type() == id_b.get_type() );

   switch( id_a.get_type() )
   {
      case fc::variant::int64_type:
         BOOST_REQUIRE( id_a.as_int64() == id_b.as_int64() );
         break;
      case fc::variant::uint64_type:
         BOOST_REQUIRE( id_a.as_uint64() == id_b.as_uint64() );
         break;
      case fc::variant::string_type:
         BOOST_REQUIRE( id_a.as_string() == id_b.as_string() );
         break;
      case fc::variant::null_type:
         break;
      default:
         BOOST_REQUIRE( false );
   }
}

void json_rpc_database_fixture::review_answer( fc::variant& answer, int64_t code, bool is_warning, bool is_fail, fc::optional< fc::variant > id )
{
   fc::variant_object error;
   int64_t answer_code;

   if( is_fail )
   {
      if( id.valid() && code != JSON_RPC_INVALID_REQUEST )
      {
         BOOST_REQUIRE( answer.get_object().contains( "id" ) );
         check_id_equal( answer[ "id" ], *id );
      }

      BOOST_REQUIRE( answer.get_object().contains( "error" ) );
      BOOST_REQUIRE( answer["error"].is_object() );
      error = answer["error"].get_object();
      BOOST_REQUIRE( error.contains( "code" ) );
      BOOST_REQUIRE( error["code"].is_int64() );
      answer_code = error["code"].as_int64();
      BOOST_REQUIRE( answer_code == code );
      if( is_warning )
         BOOST_TEST_MESSAGE( error["message"].as_string() );
   }
   else
   {
      BOOST_REQUIRE( answer.get_object().contains( "result" ) );
      BOOST_REQUIRE( answer.get_object().contains( "id" ) );
      if( id.valid() )
         check_id_equal( answer[ "id" ], *id );
   }
}

void json_rpc_database_fixture::make_array_request( std::string& request, int64_t code, bool is_warning, bool is_fail )
{
   fc::variant answer = get_answer( request );
   BOOST_REQUIRE( answer.is_array() );

   fc::variants request_array = fc::json::from_string( request ).get_array();
   fc::variants array = answer.get_array();

   BOOST_REQUIRE( array.size() == request_array.size() );
   for( size_t i = 0; i < array.size(); ++i )
   {
      fc::optional< fc::variant > id;

      try
      {
         id = request_array[i][ "id" ];
      }
      catch( ... ) {}

      review_answer( array[i], code, is_warning, is_fail, id );
   }
}

fc::variant json_rpc_database_fixture::make_request( std::string& request, int64_t code, bool is_warning, bool is_fail )
{
   fc::variant answer = get_answer( request );
   BOOST_REQUIRE( answer.is_object() );
   fc::optional< fc::variant > id;

   try
   {
      id = fc::json::from_string( request ).get_object()[ "id" ];
   }
   catch( ... ) {}

   review_answer( answer, code, is_warning, is_fail, id );

   return answer;
}

void json_rpc_database_fixture::make_positive_request( std::string& request )
{
   make_request( request, 0/*code*/, false/*is_warning*/, false/*is_fail*/);
}

namespace test {

bool _push_block( database& db, const signed_block& b, uint32_t skip_flags /* = 0 */ )
{
   return db.push_block( b, skip_flags);
}

void _push_transaction( database& db, const signed_transaction& tx, uint32_t skip_flags /* = 0 */ )
{ try {
   db.push_transaction( tx, skip_flags );
} FC_CAPTURE_AND_RETHROW((tx)) }

} // xgt::chain::test

} } // xgt::chain
