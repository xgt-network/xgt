#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/plugins/machine_api/machine_api_plugin.hpp>
#include <xgt/plugins/machine_api/machine_api.hpp>

#include <xgt/chain/database.hpp>
#include <xgt/plugins/chain/chain_plugin.hpp>
// #include <xgt/plugins/machine/machine_objects.hpp>

// TODO: Remove this later
#include "iomanip"

namespace xgt { namespace plugins { namespace machine {

namespace detail {

class machine_api_impl
{
   public:
      machine_api_impl() : _db( appbase::app().get_plugin< xgt::plugins::chain::chain_plugin >().db() ) {}

      adjure_return adjure( const adjure_args& args )const;

      chain::database& _db;
};

adjure_return machine_api_impl::adjure( const adjure_args& args )const
{
   adjure_return final_result;

   // std::cout << "=====";
   // std::cout << std::endl;
   // std::cout << "args.bytecode.size(): ";
   // std::cout << args.bytecode.size();
   // std::cout << std::endl;
   // std::stringstream stream;
   // for (size_t i = 0; i < args.bytecode.size(); ++i)
   // {
   //    uint8_t byte = args.bytecode[i];
   //    std::cout << unsigned(byte) << std::endl;
   // }
   // std::cout << std::endl;

   ctx = {}
   msg = {}

   machine m(ctx, args.bytecode, msg);
   // m.register_listener(std::move(std::unique_ptr<listener>(new stub_listener())));
   m.register_instruction(noop_opcode, std::move(std::unique_ptr<instruction>(new noop_instruction())));
   m.register_instruction(push_opcode, std::move(std::unique_ptr<instruction>(new push_instruction())));
   m.register_instruction(add_opcode, std::move(std::unique_ptr<instruction>(new add_instruction())));
   m.register_instruction(display_opcode, std::move(std::unique_ptr<instruction>(new display_instruction())));
   while (m.is_running())
      m.step();

   final_result.return_code = 0;
   return final_result;
}

} // detail

machine_api::machine_api(): my( new detail::machine_api_impl() )
{
   JSON_RPC_REGISTER_API( XGT_MACHINE_API_PLUGIN_NAME );
}

machine_api::~machine_api() {}

DEFINE_READ_APIS( machine_api, (adjure) )

} } } // xgt::plugins::machine
