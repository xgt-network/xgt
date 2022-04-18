#include <appbase/application.hpp>
#include <xgt/manifest/plugins.hpp>

#include <xgt/protocol/types.hpp>
#include <xgt/protocol/version.hpp>

#include <xgt/utilities/logging_config.hpp>
#include <xgt/utilities/key_conversion.hpp>
#include <xgt/utilities/git_revision.hpp>

// API plugins
#include <xgt/plugins/block_api/block_api_plugin.hpp>
#include <xgt/plugins/chain_api/chain_api_plugin.hpp>
#include <xgt/plugins/database_api/database_api_plugin.hpp>
#include <xgt/plugins/transaction_api/transaction_api_plugin.hpp>
#include <xgt/plugins/wallet_by_key_api/wallet_by_key_api_plugin.hpp>
#include <xgt/plugins/wallet_history_api/wallet_history_api_plugin.hpp>

// Core plugins
#include <xgt/plugins/chain/chain_plugin.hpp>
#include <xgt/plugins/p2p/p2p_plugin.hpp>
#include <xgt/plugins/webserver/webserver_plugin.hpp>
#include <xgt/plugins/witness/witness_plugin.hpp>
#include <xgt/plugins/wallet_by_key/wallet_by_key_plugin.hpp>
#include <xgt/plugins/wallet_history_rocksdb/wallet_history_rocksdb_plugin.hpp>

#include <fc/exception/exception.hpp>
#include <fc/thread/thread.hpp>
#include <fc/stacktrace.hpp>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <csignal>
#include <vector>

namespace bpo = boost::program_options;
using xgt::protocol::version;
using std::string;
using std::vector;

string& version_string()
{
   static string v_str =
      "xgt_version: " + fc::string( xgt::utilities::git_revision_description ) + "\n" +
      "xgt_blockchain_version: " + fc::string( XGT_BLOCKCHAIN_VERSION ) + "\n" +
      "xgt_git_revision:       " + fc::string( xgt::utilities::git_revision_sha ) + "\n";
   return v_str;
}

void info()
{
   std::cerr << "------------------------------------------------------\n\n";
   std::cerr << "            STARTING XGT NETWORK\n\n";
   std::cerr << "------------------------------------------------------\n";
   std::cerr << "network type: " << XGT_NETWORK_TYPE << "\n";
   std::cerr << "address prefix: " << XGT_ADDRESS_PREFIX << "\n";
   std::cerr << "genesis wallet name: " << XGT_INIT_MINER_NAME << "\n";
   std::cerr << "genesis public key: " << XGT_INIT_PUBLIC_KEY_STR << "\n";
   std::cerr << "chain id: " << std::string( XGT_CHAIN_ID ) << "\n";
   std::cerr << "blockchain version: " << fc::string( XGT_BLOCKCHAIN_VERSION ) << "\n";
   std::cerr << "genesis_time: " << fc::string( XGT_GENESIS_TIME ) << "\n";
   std::cerr << "now: " << fc::string( (fc::time_point_sec(fc::time_point::now())) ) << "\n";
   std::cerr << "xgt_max_witnesses: " << XGT_MAX_WITNESSES << "\n";
   std::cerr << "xgt_max_voted_witnesses: " << XGT_MAX_VOTED_WITNESSES << "\n";
   std::cerr << "xgt_max_miner_witnesses: " << XGT_MAX_MINER_WITNESSES << "\n";
   std::cerr << "------------------------------------------------------\n";
}

int main( int argc, char** argv )
{
   try
   {
      // Setup logging config
      bpo::options_description options;

      xgt::utilities::set_logging_program_options( options );
      options.add_options()
         ("backtrace", bpo::value< string >()->default_value( "yes" ), "Whether to print backtrace on SIGSEGV" );

      appbase::app().add_program_options( bpo::options_description(), options );

      xgt::plugins::register_plugins();

      appbase::app().set_version_string( version_string() );
      appbase::app().set_app_name( "xgtd" );

      // These plugins are included in the default config
      appbase::app().set_default_plugins<
         xgt::plugins::witness::witness_plugin,
         xgt::plugins::wallet_by_key::wallet_by_key_plugin,
         xgt::plugins::wallet_history_rocksdb::wallet_history_rocksdb_plugin,

         // APIs:
         xgt::plugins::block_api::block_api_plugin,
         xgt::plugins::chain::chain_api_plugin,
         xgt::plugins::database_api::database_api_plugin,
         xgt::plugins::transaction_api::transaction_api_plugin,
         xgt::plugins::wallet_by_key::wallet_by_key_api_plugin,
         xgt::plugins::wallet_history::wallet_history_api_plugin
      >();

      // These plugins are loaded regardless of the config
      bool initialized = appbase::app().initialize<
            xgt::plugins::chain::chain_plugin,
            xgt::plugins::p2p::p2p_plugin,
            xgt::plugins::webserver::webserver_plugin
      >( argc, argv );

      info();

      if( !initialized )
         return 0;

      auto& args = appbase::app().get_args();

      try
      {
         fc::optional< fc::logging_config > logging_config = xgt::utilities::load_logging_config( args, appbase::app().data_dir() );
         if( logging_config )
            fc::configure_logging( *logging_config );
      }
      catch( const fc::exception& e )
      {
         wlog( "Error parsing logging config. ${e}", ("e", e.to_string()) );
      }

      if( args.at( "backtrace" ).as< string >() == "yes" )
      {
         fc::print_stacktrace_on_segfault();
         ilog( "Backtrace on segfault is enabled." );
      }

      appbase::app().startup();
      appbase::app().exec();
      std::cout << "exited cleanly\n";
      return 0;
   }
   catch ( const boost::exception& e )
   {
      std::cerr << boost::diagnostic_information(e) << "\n";
   }
   catch ( const fc::exception& e )
   {
      std::cerr << e.to_detail_string() << "\n";
   }
   catch ( const std::exception& e )
   {
      std::cerr << e.what() << "\n";
   }
   catch ( ... )
   {
      std::cerr << "unknown exception\n";
   }

   return -1;
}
