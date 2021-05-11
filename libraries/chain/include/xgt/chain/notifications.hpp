#pragma once

#include <xgt/protocol/block.hpp>

namespace xgt { namespace chain {

struct block_notification
{
   block_notification( const xgt::protocol::signed_block& b ) : block(b)
   {
      block_id = b.id();
      block_num = xgt::protocol::block_header::num_from_id( block_id );
   }

   xgt::protocol::block_id_type          block_id;
   uint32_t                                block_num = 0;
   const xgt::protocol::signed_block&    block;
};

struct transaction_notification
{
   transaction_notification( const xgt::protocol::signed_transaction& tx ) : transaction(tx)
   {
      transaction_id = tx.id();
   }

   xgt::protocol::transaction_id_type          transaction_id;
   const xgt::protocol::signed_transaction&    transaction;
};

struct operation_notification
{
   operation_notification( const xgt::protocol::operation& o ) : op(o) {}

   transaction_id_type trx_id;
   uint32_t            block = 0;
   uint32_t            trx_in_block = 0;
   uint32_t            op_in_trx = 0;
   uint32_t            virtual_op = 0;
   const xgt::protocol::operation&    op;
};

struct required_action_notification
{
   required_action_notification( const xgt::protocol::required_automated_action& a ) : action(a) {}

   const xgt::protocol::required_automated_action& action;
};

struct optional_action_notification
{
   optional_action_notification( const xgt::protocol::optional_automated_action& a ) : action(a) {}

   const xgt::protocol::optional_automated_action& action;
};

} }
