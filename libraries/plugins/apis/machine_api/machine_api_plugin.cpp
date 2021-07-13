#include <xgt/plugins/machine_api/machine_api_plugin.hpp>
#include <xgt/plugins/machine_api/machine_api.hpp>


namespace xgt { namespace plugins { namespace machine {

machine_api_plugin::machine_api_plugin() {}
machine_api_plugin::~machine_api_plugin() {}

void machine_api_plugin::set_program_options( options_description& cli, options_description& cfg ) {}

void machine_api_plugin::plugin_initialize( const variables_map& options )
{
   api = std::make_shared< machine_api >();
}

void machine_api_plugin::plugin_startup() {}
void machine_api_plugin::plugin_shutdown() {}

} } } // xgt::plugins::machine
