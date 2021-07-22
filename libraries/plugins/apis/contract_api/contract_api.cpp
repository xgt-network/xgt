#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/plugins/contract_api/contract_api_plugin.hpp>
#include <xgt/plugins/contract_api/contract_api.hpp>

#include <xgt/chain/wallet_object.hpp>
#include <xgt/chain/database.hpp>
#include <xgt/chain/index.hpp>

#include <machine.hpp>

namespace xgt { namespace plugins { namespace contract {

namespace detail {

class contract_api_impl
{
   public:
      contract_api_impl()
      : _db( appbase::app().get_plugin<xgt::plugins::chain::chain_plugin>().db() ) {}

      DECLARE_API_IMPL(
         (get_contract)
         (list_owner_contracts)
         (invoke) )

      chain::database& _db;
};

DEFINE_API_IMPL( contract_api_impl, get_contract )
{
   // TODO: Temporary
   machine::message msg = {};
   ilog( "machine::message msg.flags ${f}", ("f",msg.flags) );

   get_contract_return result;
   result.example = true;
   return result;
}

DEFINE_API_IMPL( contract_api_impl, list_owner_contracts )
{
   list_owner_contracts_return result;

   const auto& idx = _db.get_index< chain::contract_index, chain::by_owner >();
   auto itr = idx.lower_bound( args.owner );
   auto end = idx.end();
   wlog("!!!!!! LIST_OWNER_CONTRACTS");
   while( itr != end )
   {
      if (itr->owner != args.owner) break;
      wlog("!!!!!! LIST_OWNER_CONTRACTS ${w}", ("w",itr->owner));
      result.contracts.push_back(*itr);
      ++itr;
   }

   return result;
}

machine::chain_adapter make_chain_adapter()
{
  std::function< uint64_t(std::string) > get_balance = [](std::string wallet_name) -> uint64_t
  {
    return 0;
  };

  machine::chain_adapter adapter = {
    get_balance
  };

  return adapter;
}

DEFINE_API_IMPL( contract_api_impl, invoke )
{
   // TODO: Temporary
   machine::message msg = {};
   ilog( "machine::message msg.flags ${f}", ("f",msg.flags) );

   machine::context ctx = {true, 0x5c477758};
   machine::chain_adapter adapter = make_chain_adapter();
   machine::machine m(ctx, args.code, msg, adapter);
   m.print_stack();

   std::string line;
   while (m.is_running())
   {
     std::cerr << "step\n";
     m.step();
     // Print out any logging that was generated
     while ( std::getline(m.get_logger(), line) )
       std::cerr << "\e[36m" << "LOG: " << line << "\e[0m" << std::endl;
   }
   while ( std::getline(m.get_logger(), line) )
     std::cerr << "\e[36m" << "LOG: " << line << "\e[0m" << std::endl;
   std::cout << m.to_json() << std::endl;

   invoke_return result;

   return result;
}

} // detail

contract_api::contract_api(): my( new detail::contract_api_impl() )
{
   JSON_RPC_REGISTER_API( XGT_CONTRACT_API_PLUGIN_NAME );
}

contract_api::~contract_api() {}

DEFINE_LOCKLESS_APIS( contract_api,
   (get_contract)
   (list_owner_contracts)
   (invoke)
)

} } } //xgt::plugins::contract
