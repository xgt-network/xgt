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
         (debug_invoke) )

      chain::database& _db;
};

DEFINE_API_IMPL( contract_api_impl, get_contract )
{
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

DEFINE_API_IMPL( contract_api_impl, debug_invoke )
{
   debug_invoke_return result{false};

   // TODO: Add machine here
   machine::message msg = {};
   ilog( "machine::message msg.flags ${f}", ("f",msg.flags) );

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
   (debug_invoke)
)

} } } //xgt::plugins::contract
