#pragma once

#include <xgt/plugins/block_api/block_api_objects.hpp>

#include <xgt/protocol/types.hpp>
#include <xgt/protocol/transaction.hpp>
#include <xgt/protocol/block_header.hpp>

#include <xgt/plugins/json_rpc/utility.hpp>

namespace xgt { namespace plugins { namespace block_api {

/* get_block_header */

struct get_block_header_args
{
   uint32_t block_num;
};

struct get_block_header_return
{
   optional< block_header > header;
};

/* get_block */
struct get_block_args
{
   uint32_t block_num;
};

struct get_block_return
{
   optional< api_signed_block_object > block;
};

} } } // xgt::block_api

FC_REFLECT( xgt::plugins::block_api::get_block_header_args,
   (block_num) )

FC_REFLECT( xgt::plugins::block_api::get_block_header_return,
   (header) )

FC_REFLECT( xgt::plugins::block_api::get_block_args,
   (block_num) )

FC_REFLECT( xgt::plugins::block_api::get_block_return,
   (block) )

