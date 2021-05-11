#pragma once

#include <xgt/chain/xgt_fwd.hpp>
#include <xgt/plugins/wallet_by_key/wallet_by_key_plugin.hpp>
#include <xgt/plugins/json_rpc/json_rpc_plugin.hpp>

#include <appbase/application.hpp>

#define XGT_WALLET_BY_KEY_API_PLUGIN_NAME "wallet_by_key_api"


namespace xgt { namespace plugins { namespace wallet_by_key {

using namespace appbase;

class wallet_by_key_api_plugin : public appbase::plugin< wallet_by_key_api_plugin >
{
public:
   APPBASE_PLUGIN_REQUIRES(
      (xgt::plugins::wallet_by_key::wallet_by_key_plugin)
      (xgt::plugins::json_rpc::json_rpc_plugin)
   )

   wallet_by_key_api_plugin();
   virtual ~wallet_by_key_api_plugin();

   static const std::string& name() { static std::string name = XGT_WALLET_BY_KEY_API_PLUGIN_NAME; return name; }

   virtual void set_program_options( options_description& cli, options_description& cfg ) override;

   virtual void plugin_initialize( const variables_map& options ) override;
   virtual void plugin_startup() override;
   virtual void plugin_shutdown() override;

   std::shared_ptr< class wallet_by_key_api > api;
};

} } } // xgt::plugins::wallet_by_key
