#pragma once

#include <xgt/plugins/database_api/database_api.hpp>
#include <xgt/plugins/p2p/p2p_plugin.hpp>
#include <xgt/plugins/chain/chain_plugin.hpp>
#include <xgt/plugins/json_rpc/utility.hpp>
#include <xgt/plugins/transaction_status/transaction_status_objects.hpp>

#include <fc/optional.hpp>
#include <fc/variant.hpp>
#include <fc/vector.hpp>

#include <boost/thread/mutex.hpp>


namespace xgt { namespace plugins { namespace transaction_api {

using std::vector;
using fc::variant;
using fc::optional;
using xgt::plugins::json_rpc::void_type;

using xgt::protocol::signed_transaction;
using xgt::protocol::transaction_id_type;
using xgt::protocol::signed_block;

using namespace chain;

typedef vector< variant > get_transaction_hex_args;

typedef string get_transaction_hex_return;

typedef vector< variant > broadcast_transaction_args;

struct broadcast_transaction_return
{
   broadcast_transaction_return() {}
   broadcast_transaction_return( transaction_id_type txid )
   : id(txid) {}

   transaction_id_type   id;
};

struct broadcast_block_args
{
   signed_block   block;
};

typedef void_type broadcast_block_return;

struct find_transaction_args
{
   chain::transaction_id_type transaction_id;
   fc::optional< fc::time_point_sec > expiration;
};

struct find_transaction_return
{
   transaction_status::transaction_status status;
   fc::optional< uint32_t > block_num;
};

namespace detail{ class transaction_api_impl; }

class transaction_api
{
   public:
      transaction_api();
      ~transaction_api();

      DECLARE_API(
         (get_transaction_hex)
         (broadcast_transaction)
         (broadcast_block)
         (find_transaction)
      )

   private:
      friend class transaction_api_plugin;
      void api_startup();

      std::unique_ptr< detail::transaction_api_impl > my;
};

} } } // xgt::plugins::transaction_api

FC_REFLECT( xgt::plugins::transaction_api::broadcast_transaction_return,
   (id) )

FC_REFLECT( xgt::plugins::transaction_api::broadcast_block_args,
   (block) )

FC_REFLECT( xgt::plugins::transaction_api::find_transaction_args,
   (transaction_id)
   (expiration) )

FC_REFLECT( xgt::plugins::transaction_api::find_transaction_return,
   (status)
   (block_num) )
