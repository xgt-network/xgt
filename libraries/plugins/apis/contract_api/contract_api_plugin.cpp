#include <xgt/plugins/contract_api/contract_api_plugin.hpp>
#include <xgt/plugins/contract_api/contract_api.hpp>


namespace xgt { namespace plugins { namespace contract {

contract_api_plugin::contract_api_plugin() {}
contract_api_plugin::~contract_api_plugin() {}

void contract_api_plugin::set_program_options( options_description& cli, options_description& cfg ) {}

void contract_api_plugin::plugin_initialize( const variables_map& options )
{
   api = std::make_shared< contract_api >();
}

void contract_api_plugin::plugin_startup() {}
void contract_api_plugin::plugin_shutdown() {}

} } } // xgt::plugins::contract
