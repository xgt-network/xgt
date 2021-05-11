#include <xgt/plugins/wallet_by_key_api/wallet_by_key_api_plugin.hpp>
#include <xgt/plugins/wallet_by_key_api/wallet_by_key_api.hpp>


namespace xgt { namespace plugins { namespace wallet_by_key {

wallet_by_key_api_plugin::wallet_by_key_api_plugin() {}
wallet_by_key_api_plugin::~wallet_by_key_api_plugin() {}

void wallet_by_key_api_plugin::set_program_options( options_description& cli, options_description& cfg ) {}

void wallet_by_key_api_plugin::plugin_initialize( const variables_map& options )
{
   api = std::make_shared< wallet_by_key_api >();
}

void wallet_by_key_api_plugin::plugin_startup() {}
void wallet_by_key_api_plugin::plugin_shutdown() {}

} } } // xgt::plugins::wallet_by_key
