#pragma once
#include <xgt/chain/xgt_fwd.hpp>
// #include <xgt/plugins/machine/machine_plugin.hpp>
#include <xgt/plugins/json_rpc/json_rpc_plugin.hpp>

#include <appbase/application.hpp>

#define XGT_MACHINE_API_PLUGIN_NAME "machine_api"


namespace xgt { namespace plugins { namespace machine {

using namespace appbase;

class machine_api_plugin : public appbase::plugin< machine_api_plugin >
{
public:
   APPBASE_PLUGIN_REQUIRES(
      // (xgt::plugins::machine::machine_plugin)
      (xgt::plugins::json_rpc::json_rpc_plugin)
   )

   machine_api_plugin();
   virtual ~machine_api_plugin();

   static const std::string& name() { static std::string name = XGT_MACHINE_API_PLUGIN_NAME; return name; }

   virtual void set_program_options( options_description& cli, options_description& cfg ) override;

   virtual void plugin_initialize( const variables_map& options ) override;
   virtual void plugin_startup() override;
   virtual void plugin_shutdown() override;

   std::shared_ptr< class machine_api > api;
};

} } } // xgt::plugins::machine
