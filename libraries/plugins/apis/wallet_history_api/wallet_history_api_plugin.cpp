#include <xgt/plugins/wallet_history_api/wallet_history_api_plugin.hpp>
#include <xgt/plugins/wallet_history_api/wallet_history_api.hpp>

namespace xgt { namespace plugins { namespace wallet_history {

wallet_history_api_plugin::wallet_history_api_plugin() {}
wallet_history_api_plugin::~wallet_history_api_plugin() {}

void wallet_history_api_plugin::set_program_options( options_description& cli, options_description& cfg ) {}

void wallet_history_api_plugin::plugin_initialize( const variables_map& options )
{
   api = std::make_shared< wallet_history_api >();
}

void wallet_history_api_plugin::plugin_startup() {}
void wallet_history_api_plugin::plugin_shutdown() {}

} } } // xgt::plugins::wallet_history
