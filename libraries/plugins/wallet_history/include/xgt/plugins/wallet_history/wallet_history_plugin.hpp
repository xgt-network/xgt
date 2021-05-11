#pragma once

#include <xgt/chain/xgt_fwd.hpp>
#include <xgt/plugins/chain/chain_plugin.hpp>

#define XGT_WALLET_HISTORY_PLUGIN_NAME "wallet_history"

namespace xgt { namespace plugins { namespace wallet_history {

namespace detail { class wallet_history_plugin_impl; }

using namespace appbase;
using xgt::protocol::wallet_name_type;

//
// Plugins should #define their SPACE_ID's so plugins with
// conflicting SPACE_ID assignments can be compiled into the
// same binary (by simply re-assigning some of the conflicting #defined
// SPACE_ID's in a build script).
//
// Assignment of SPACE_ID's cannot be done at run-time because
// various template automagic depends on them being known at compile
// time.
//
#ifndef XGT_WALLET_HISTORY_SPACE_ID
#define XGT_WALLET_HISTORY_SPACE_ID 5
#endif

/**
 *  This plugin is designed to track a range of operations by wallet so that one node
 *  doesn't need to hold the full operation history in memory.
 */
class wallet_history_plugin : public plugin< wallet_history_plugin >
{
   public:
      wallet_history_plugin();
      virtual ~wallet_history_plugin();

      APPBASE_PLUGIN_REQUIRES( (xgt::plugins::chain::chain_plugin) )

      static const std::string& name() { static std::string name = XGT_WALLET_HISTORY_PLUGIN_NAME; return name; }

      virtual void set_program_options(
         options_description& cli,
         options_description& cfg ) override;
      virtual void plugin_initialize( const variables_map& options ) override;
      virtual void plugin_startup() override;
      virtual void plugin_shutdown() override;

      flat_map< wallet_name_type, wallet_name_type > tracked_wallets()const; /// map start_range to end_range

   private:
      std::unique_ptr< detail::wallet_history_plugin_impl > my;
};

} } } //xgt::plugins::wallet_history

