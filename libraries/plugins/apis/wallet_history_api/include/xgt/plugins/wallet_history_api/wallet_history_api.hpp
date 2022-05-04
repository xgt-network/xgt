#pragma once

#include <xgt/plugins/json_rpc/utility.hpp>

#include <xgt/chain/history_object.hpp>

#include <xgt/protocol/types.hpp>
#include <xgt/protocol/transaction.hpp>

#include <fc/optional.hpp>
#include <fc/variant.hpp>

namespace xgt { namespace plugins { namespace wallet_history {


namespace detail { class abstract_wallet_history_api_impl; }

struct api_operation_object
{
   api_operation_object() {}

   template< typename T >
   api_operation_object( const T& op_obj ) :
      trx_id( op_obj.trx_id ),
      block( op_obj.block ),
      trx_in_block( op_obj.trx_in_block ),
      virtual_op( op_obj.virtual_op ),
      timestamp( op_obj.timestamp )
   {
      op = fc::raw::unpack_from_buffer< xgt::protocol::operation >( op_obj.serialized_op );
   }

   xgt::protocol::transaction_id_type trx_id;
   uint32_t                               block = 0;
   uint32_t                               trx_in_block = 0;
   uint32_t                               op_in_trx = 0;
   uint32_t                               virtual_op = 0;
   fc::time_point_sec                     timestamp;
   xgt::protocol::operation             op;

   bool operator<( const api_operation_object& obj ) const
   {
      return std::tie( block, trx_in_block, op_in_trx, virtual_op ) < std::tie( obj.block, obj.trx_in_block, obj.op_in_trx, obj.virtual_op );
   }
};


struct get_ops_in_block_args
{
   uint32_t block_num;
   bool     only_virtual;
};

struct get_ops_in_block_return
{
   std::multiset< api_operation_object > ops;
};


struct get_transaction_args
{
   xgt::protocol::transaction_id_type id;
};

typedef xgt::protocol::annotated_signed_transaction get_transaction_return;


struct get_wallet_history_args
{
   xgt::protocol::wallet_name_type     wallet;
   uint64_t                            start = -1;
   uint32_t                            limit = 1000;
};

struct get_wallet_history_return
{
   std::map< uint32_t, api_operation_object > history;
};

/** Allows to specify range of blocks to retrieve virtual operations for.
 *  \param block_range_begin - starting block number (inclusive) to search for virtual operations
 *  \param block_range_end   - last block number (exclusive) to search for virtual operations
 */
struct enum_virtual_ops_args
{
   uint32_t block_range_begin = 1;
   uint32_t block_range_end = 2;
};

struct enum_virtual_ops_return
{
   vector<api_operation_object> ops;
   uint32_t                     next_block_range_begin = 0;
};


class wallet_history_api
{
   public:
      wallet_history_api();
      ~wallet_history_api();

      DECLARE_API(
         (get_ops_in_block)
         (get_transaction)
         (get_wallet_history)
         (enum_virtual_ops)
      )

   private:
      std::unique_ptr< detail::abstract_wallet_history_api_impl > my;
};

} } } // xgt::plugins::wallet_history

FC_REFLECT( xgt::plugins::wallet_history::api_operation_object,
   (trx_id)(block)(trx_in_block)(op_in_trx)(virtual_op)(timestamp)(op) )

FC_REFLECT( xgt::plugins::wallet_history::get_ops_in_block_args,
   (block_num)(only_virtual) )

FC_REFLECT( xgt::plugins::wallet_history::get_ops_in_block_return,
   (ops) )

FC_REFLECT( xgt::plugins::wallet_history::get_transaction_args,
   (id) )

FC_REFLECT( xgt::plugins::wallet_history::get_wallet_history_args,
   (wallet)(start)(limit) )

FC_REFLECT( xgt::plugins::wallet_history::get_wallet_history_return,
   (history) )

FC_REFLECT( xgt::plugins::wallet_history::enum_virtual_ops_args,
   (block_range_begin)(block_range_end) )

FC_REFLECT( xgt::plugins::wallet_history::enum_virtual_ops_return,
   (ops)(next_block_range_begin) )
