#pragma once

#include <xgt/plugins/json_rpc/utility.hpp>

#include <xgt/protocol/types.hpp>

#include <xgt/chain/contract_objects.hpp>

#include <fc/optional.hpp>

namespace xgt { namespace plugins { namespace contract {

namespace detail { class contract_api_impl; }

struct get_contract_args
{
   bool example = true;
};


struct get_contract_return
{
   bool example = true;
};


struct list_owner_contracts_args
{
   protocol::wallet_name_type owner;
};


struct list_owner_contracts_return
{
   std::vector< chain::contract_object > contracts;
};


<<<<<<< HEAD
struct invoke_args
{
   protocol::wallet_name_type owner;
   vector< uint8_t > code;
};


struct invoke_return
{
   protocol::wallet_name_type owner;
}; 

class contract_api
{
   public:
      contract_api();
      ~contract_api();

      DECLARE_API(
         (get_contract)
         (list_owner_contracts)
<<<<<<< HEAD
         (invoke) )
=======
         (debug_invoke) )
>>>>>>> 6dbfd9467f8bb750a69d143ac0cec769b2d5647b
   private:
      std::unique_ptr< detail::contract_api_impl > my;
};

} } } // xgt::plugins::contract

FC_REFLECT( xgt::plugins::contract::get_contract_args, (example) )
FC_REFLECT( xgt::plugins::contract::get_contract_return, (example) )
FC_REFLECT( xgt::plugins::contract::list_owner_contracts_args, (owner) )
FC_REFLECT( xgt::plugins::contract::list_owner_contracts_return, (contracts) )
<<<<<<< HEAD
FC_REFLECT( xgt::plugins::contract::invoke_args, (owner) (code) )
FC_REFLECT( xgt::plugins::contract::invoke_return, (owner) )
=======
FC_REFLECT( xgt::plugins::contract::debug_invoke_args, (example) )
FC_REFLECT( xgt::plugins::contract::debug_invoke_return, (example) )
>>>>>>> 6dbfd9467f8bb750a69d143ac0cec769b2d5647b
