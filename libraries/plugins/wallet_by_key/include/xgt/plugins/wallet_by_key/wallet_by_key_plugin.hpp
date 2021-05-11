#pragma once

#include <xgt/chain/xgt_fwd.hpp>
#include <appbase/application.hpp>

#include <xgt/plugins/chain/chain_plugin.hpp>

namespace xgt { namespace plugins { namespace wallet_by_key {

namespace detail { class wallet_by_key_plugin_impl; }

using namespace appbase;

#define XGT_WALLET_BY_KEY_PLUGIN_NAME "wallet_by_key"

class wallet_by_key_plugin : public appbase::plugin< wallet_by_key_plugin >
{
   public:
      wallet_by_key_plugin();
      virtual ~wallet_by_key_plugin();

      APPBASE_PLUGIN_REQUIRES( (xgt::plugins::chain::chain_plugin) )

      static const std::string& name() { static std::string name = XGT_WALLET_BY_KEY_PLUGIN_NAME; return name; }

      virtual void set_program_options( options_description& cli, options_description& cfg ) override;
      virtual void plugin_initialize( const variables_map& options ) override;
      virtual void plugin_startup() override;
      virtual void plugin_shutdown() override;

   private:
      std::unique_ptr< detail::wallet_by_key_plugin_impl > my;
};

} } } // xgt::plugins::wallet_by_key
