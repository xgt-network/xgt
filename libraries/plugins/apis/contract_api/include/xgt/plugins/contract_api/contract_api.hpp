#pragma once

#include <xgt/plugins/json_rpc/utility.hpp>

#include <xgt/protocol/types.hpp>

#include <xgt/chain/wallet_object.hpp>
#include <xgt/chain/contract_objects.hpp>

#include <fc/optional.hpp>

namespace xgt { namespace plugins { namespace contract {

namespace detail { class contract_api_impl; }

struct api_contract_object
{
  chain::contract_id_type id;
  chain::wallet_name_type owner;
  chain::wallet_name_type wallet;
  chain::contract_hash_type contract_hash;
  std::string en_address;
  vector<char> code;
};


struct get_contract_args
{
   chain::contract_hash_type contract_hash;
};


struct get_contract_return
{
   api_contract_object contract;
};


struct list_owner_contracts_args
{
   protocol::wallet_name_type owner;
};


struct list_owner_contracts_return
{
   std::vector< api_contract_object > contracts;
};


class contract_api
{
   public:
      contract_api();
      ~contract_api();

      DECLARE_API(
         (get_contract)
         (list_owner_contracts) )
   private:
      std::unique_ptr< detail::contract_api_impl > my;
};

} } } // xgt::plugins::contract

FC_REFLECT( xgt::plugins::contract::api_contract_object,
            (id)
            (owner)
            (wallet)
            (contract_hash)
            (code)
            (en_address) )

FC_REFLECT( xgt::plugins::contract::get_contract_args, (contract_hash) )
FC_REFLECT( xgt::plugins::contract::get_contract_return, (contract) )
FC_REFLECT( xgt::plugins::contract::list_owner_contracts_args, (owner) )
FC_REFLECT( xgt::plugins::contract::list_owner_contracts_return, (contracts) )
