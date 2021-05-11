#pragma once

#include <xgt/chain/xgt_fwd.hpp>
#include <xgt/plugins/chain/chain_plugin.hpp>
#include <xgt/plugins/json_rpc/json_rpc_plugin.hpp>

#include <appbase/application.hpp>

namespace xgt { namespace plugins { namespace database_api {

using namespace appbase;

#define XGT_DATABASE_API_PLUGIN_NAME "database_api"

class database_api_plugin : public plugin< database_api_plugin >
{
   public:
      database_api_plugin();
      virtual ~database_api_plugin();

      APPBASE_PLUGIN_REQUIRES(
         (xgt::plugins::json_rpc::json_rpc_plugin)
         (xgt::plugins::chain::chain_plugin)
      )

      static const std::string& name() { static std::string name = XGT_DATABASE_API_PLUGIN_NAME; return name; }

      virtual void set_program_options(
         options_description& cli,
         options_description& cfg ) override;
      virtual void plugin_initialize( const variables_map& options ) override;
      virtual void plugin_startup() override;
      virtual void plugin_shutdown() override;

      std::shared_ptr< class database_api > api;
};

} } } // xgt::plugins::database_api
