#pragma once

#include <xgt/plugins/json_rpc/utility.hpp>

#include <xgt/protocol/types.hpp>

#include <fc/optional.hpp>
#include <fc/variant.hpp>
#include <fc/vector.hpp>

namespace xgt { namespace plugins { namespace wallet_by_key {

namespace detail
{
   class wallet_by_key_api_impl;
}

struct get_key_references_args
{
   std::vector< xgt::protocol::public_key_type > keys;
};

struct get_key_references_return
{
   std::vector< std::vector< xgt::protocol::wallet_name_type > > wallets;
};

struct generate_wallet_name_args
{
   std::vector< xgt::protocol::public_key_type > recovery_keys;
};

struct generate_wallet_name_return
{
   xgt::protocol::wallet_name_type wallet_name;
};

class wallet_by_key_api
{
   public:
      wallet_by_key_api();
      ~wallet_by_key_api();

      DECLARE_API(
         (get_key_references)
         (generate_wallet_name)
      )

   private:
      std::unique_ptr< detail::wallet_by_key_api_impl > my;
};

} } } // xgt::plugins::wallet_by_key

FC_REFLECT( xgt::plugins::wallet_by_key::get_key_references_args,
   (keys) )

FC_REFLECT( xgt::plugins::wallet_by_key::get_key_references_return,
   (wallets) )

FC_REFLECT( xgt::plugins::wallet_by_key::generate_wallet_name_args,
   (recovery_keys) )

FC_REFLECT( xgt::plugins::wallet_by_key::generate_wallet_name_return,
   (wallet_name) )
