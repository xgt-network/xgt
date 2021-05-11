#pragma once

#include <xgt/protocol/block_header.hpp>
#include <xgt/protocol/transaction.hpp>

namespace xgt { namespace protocol {

   struct signed_block : public signed_block_header
   {
      checksum_type calculate_merkle_root()const;
      vector<signed_transaction> transactions;
   };

} } // xgt::protocol

FC_REFLECT_DERIVED( xgt::protocol::signed_block, (xgt::protocol::signed_block_header), (transactions) )
