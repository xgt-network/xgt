#include <appbase/application.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#include <xgt/manifest/plugins.hpp>


#include <xgt/plugins/wallet_by_key/wallet_by_key_plugin.hpp>

#include <xgt/plugins/wallet_history_rocksdb/wallet_history_rocksdb_plugin.hpp>

#include <xgt/plugins/transaction_status/transaction_status_plugin.hpp>

#include <xgt/plugins/chain/chain_plugin.hpp>

#include <xgt/plugins/webserver/webserver_plugin.hpp>

#include <xgt/plugins/chain_api/chain_api_plugin.hpp>

#include <xgt/plugins/transaction_api/transaction_api_plugin.hpp>

#include <xgt/plugins/wallet_by_key_api/wallet_by_key_api_plugin.hpp>

#include <xgt/plugins/contract_api/contract_api_plugin.hpp>

#include <xgt/plugins/wallet_history_api/wallet_history_api_plugin.hpp>

#include <xgt/plugins/database_api/database_api_plugin.hpp>

#include <xgt/plugins/block_api/block_api_plugin.hpp>

#include <xgt/plugins/debug_node/debug_node_plugin.hpp>

#include <xgt/plugins/p2p/p2p_plugin.hpp>

#include <xgt/plugins/wallet_history/wallet_history_plugin.hpp>

#include <xgt/plugins/witness/witness_plugin.hpp>


namespace xgt { namespace plugins {

void register_plugins()
{
   
   appbase::app().register_plugin< xgt::plugins::wallet_by_key::wallet_by_key_plugin >();
   
   appbase::app().register_plugin< xgt::plugins::wallet_history_rocksdb::wallet_history_rocksdb_plugin >();
   
   appbase::app().register_plugin< xgt::plugins::transaction_status::transaction_status_plugin >();
   
   appbase::app().register_plugin< xgt::plugins::chain::chain_plugin >();
   
   appbase::app().register_plugin< xgt::plugins::webserver::webserver_plugin >();
   
   appbase::app().register_plugin< xgt::plugins::chain::chain_api_plugin >();
   
   appbase::app().register_plugin< xgt::plugins::transaction_api::transaction_api_plugin >();
   
   appbase::app().register_plugin< xgt::plugins::wallet_by_key::wallet_by_key_api_plugin >();
   
   appbase::app().register_plugin< xgt::plugins::contract::contract_api_plugin >();
   
   appbase::app().register_plugin< xgt::plugins::wallet_history::wallet_history_api_plugin >();
   
   appbase::app().register_plugin< xgt::plugins::database_api::database_api_plugin >();
   
   appbase::app().register_plugin< xgt::plugins::block_api::block_api_plugin >();
   
   appbase::app().register_plugin< xgt::plugins::debug_node::debug_node_plugin >();
   
   appbase::app().register_plugin< xgt::plugins::p2p::p2p_plugin >();
   
   appbase::app().register_plugin< xgt::plugins::wallet_history::wallet_history_plugin >();
   
   appbase::app().register_plugin< xgt::plugins::witness::witness_plugin >();
   
}

} }