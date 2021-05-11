#include <xgt/plugins/transaction_api/transaction_api_plugin.hpp>
#include <xgt/plugins/transaction_api/transaction_api.hpp>

namespace xgt { namespace plugins { namespace transaction_api {

transaction_api_plugin::transaction_api_plugin() {}
transaction_api_plugin::~transaction_api_plugin() {}

void transaction_api_plugin::set_program_options( options_description& cli, options_description& cfg ) {}

void transaction_api_plugin::plugin_initialize( const variables_map& options )
{
   api = std::make_shared< transaction_api >();
}

void transaction_api_plugin::plugin_startup() {
   api->api_startup();
}

void transaction_api_plugin::plugin_shutdown() {}

} } } // xgt::plugins::transaction_api
