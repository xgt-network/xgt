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


struct debug_invoke_args
{
   bool example; // TODO: Change me
};


struct debug_invoke_return
{
   bool example; // TODO: Change me
};


class contract_api
{
   public:
      contract_api();
      ~contract_api();

      DECLARE_API(
         (get_contract)
         (list_owner_contracts)
         (debug_invoke) )
   private:
      std::unique_ptr< detail::contract_api_impl > my;
};

} } } // xgt::plugins::contract

FC_REFLECT( xgt::plugins::contract::get_contract_args, (example) )
FC_REFLECT( xgt::plugins::contract::get_contract_return, (example) )
FC_REFLECT( xgt::plugins::contract::list_owner_contracts_args, (owner) )
FC_REFLECT( xgt::plugins::contract::list_owner_contracts_return, (contracts) )
FC_REFLECT( xgt::plugins::contract::debug_invoke_args, (example) )
FC_REFLECT( xgt::plugins::contract::debug_invoke_return, (example) )
