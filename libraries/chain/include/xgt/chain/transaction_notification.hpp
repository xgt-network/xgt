#pragma once

#include <xgt/protocol/block.hpp>

namespace xgt { namespace chain {

struct transaction_notification
{
   transaction_notification( const xgt::protocol::signed_transaction& tx ) : transaction(tx)
   {
      transaction_id = tx.id();
   }

   xgt::protocol::transaction_id_type          transaction_id;
   const xgt::protocol::signed_transaction&    transaction;
};

} }
