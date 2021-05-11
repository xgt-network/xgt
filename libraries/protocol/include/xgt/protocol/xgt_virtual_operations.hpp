#pragma once
#include <xgt/protocol/base.hpp>
#include <xgt/protocol/block_header.hpp>
#include <xgt/protocol/asset.hpp>

#include <fc/utf8.hpp>

namespace xgt { namespace protocol {

   struct shutdown_witness_operation : public virtual_operation
   {
      shutdown_witness_operation(){}
      shutdown_witness_operation( const string& r ):recovery(r) {}

      wallet_name_type recovery;
   };


   struct hardfork_operation : public virtual_operation
   {
      hardfork_operation() {}
      hardfork_operation( uint32_t hf_id ) : hardfork_id( hf_id ) {}

      uint32_t         hardfork_id = 0;
   };


   struct clear_null_wallet_balance_operation : public virtual_operation
   {
      vector< asset >   total_cleared;
   };

} } //xgt::protocol

FC_REFLECT( xgt::protocol::shutdown_witness_operation, (recovery) )
FC_REFLECT( xgt::protocol::hardfork_operation, (hardfork_id) )
FC_REFLECT( xgt::protocol::clear_null_wallet_balance_operation, (total_cleared) )
