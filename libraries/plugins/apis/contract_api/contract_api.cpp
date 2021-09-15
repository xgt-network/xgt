#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/plugins/contract_api/contract_api_plugin.hpp>
#include <xgt/plugins/contract_api/contract_api.hpp>

#include <xgt/chain/wallet_object.hpp>
#include <xgt/chain/database.hpp>
#include <xgt/chain/index.hpp>

#include "keccak256.h"

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
         (list_owner_contracts) )

      chain::database& _db;
};

DEFINE_API_IMPL( contract_api_impl, get_contract )
{
   // TODO: Temporary
   machine::message msg = {};
   ilog( "machine::message msg.flags ${f}", ("f",msg.flags) );

   // TODO: Temporary
   std::string message = "testing";
   unsigned char output[32];
   SHA3_CTX ctx;
   keccak_init(&ctx);
   keccak_update(&ctx, (unsigned char*)message.c_str(), message.size());
   keccak_final(&ctx, output);

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
      auto& c = *itr;
      if (c.owner != args.owner) break;
      wlog("!!!!!! LIST_OWNER_CONTRACTS ${w}", ("w",c.owner));

      api_contract_object ac;
      ac.id = c.id;
      ac.owner = c.owner;
      ac.wallet = c.wallet;
      ac.contract_hash = c.contract_hash;
      ac.code = c.code;
      result.contracts.push_back(ac);
      ++itr;
   }

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
)

} } } //xgt::plugins::contract
