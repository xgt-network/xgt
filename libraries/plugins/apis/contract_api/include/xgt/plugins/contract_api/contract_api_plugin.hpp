#pragma once

#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/plugins/chain/chain_plugin.hpp>
#include <xgt/plugins/json_rpc/json_rpc_plugin.hpp>

#include <appbase/application.hpp>

#define XGT_CONTRACT_API_PLUGIN_NAME "contract_api"


namespace xgt { namespace plugins { namespace contract {

using namespace appbase;

class contract_api_plugin : public plugin< contract_api_plugin >
{
public:
   APPBASE_PLUGIN_REQUIRES(
      (xgt::plugins::chain::chain_plugin)
      (xgt::plugins::json_rpc::json_rpc_plugin)
   )

   contract_api_plugin();
   virtual ~contract_api_plugin();

   static const std::string& name()
      { static std::string name = XGT_CONTRACT_API_PLUGIN_NAME; return name; }

   virtual void set_program_options( options_description& cli, options_description& cfg ) override;

   void plugin_initialize( const variables_map& options ) override;
   void plugin_startup() override;
   void plugin_shutdown() override;

public:
   std::shared_ptr< class contract_api > api;
};

} } } // xgt::plugins::contract
