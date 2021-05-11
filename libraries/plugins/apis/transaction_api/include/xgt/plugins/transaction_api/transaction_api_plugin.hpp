#pragma once

#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/plugins/json_rpc/json_rpc_plugin.hpp>
#include <xgt/plugins/chain/chain_plugin.hpp>
#include <xgt/plugins/database_api/database_api_plugin.hpp>
#include <xgt/plugins/p2p/p2p_plugin.hpp>
#include <xgt/plugins/transaction_status/transaction_status_plugin.hpp>

#include <appbase/application.hpp>

#define XGT_TRANSACTION_API_PLUGIN_NAME "transaction_api"

namespace xgt { namespace plugins { namespace transaction_api {

using namespace appbase;

class transaction_api_plugin : public appbase::plugin< transaction_api_plugin >
{
public:
   APPBASE_PLUGIN_REQUIRES(
      (xgt::plugins::json_rpc::json_rpc_plugin)
      (xgt::plugins::database_api::database_api_plugin)
      (xgt::plugins::chain::chain_plugin)
      (xgt::plugins::p2p::p2p_plugin)
      (xgt::plugins::transaction_status::transaction_status_plugin)
   )

   transaction_api_plugin();
   virtual ~transaction_api_plugin();

   static const std::string& name() { static std::string name = XGT_TRANSACTION_API_PLUGIN_NAME; return name; }

   virtual void set_program_options( options_description& cli, options_description& cfg ) override;
   virtual void plugin_initialize( const variables_map& options ) override;
   virtual void plugin_startup() override;
   virtual void plugin_shutdown() override;

   std::shared_ptr< class transaction_api > api;
};

} } } // xgt::plugins::transaction_api
