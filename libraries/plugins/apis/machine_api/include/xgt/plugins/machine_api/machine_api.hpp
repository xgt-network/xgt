#pragma once
#include <xgt/plugins/json_rpc/utility.hpp>

#include <xgt/protocol/types.hpp>

#include <fc/optional.hpp>
#include <fc/variant.hpp>
#include <fc/vector.hpp>

namespace xgt { namespace plugins { namespace machine {

namespace detail
{
   class machine_api_impl;
}

struct adjure_args
{
   std::vector<uint8_t> bytecode;
};

struct adjure_return
{
   int return_code;
};

class machine_api
{
   public:
      machine_api();
      ~machine_api();

      DECLARE_API( (adjure) )

   private:
      std::unique_ptr< detail::machine_api_impl > my;
};

} } } // xgt::plugins::machine

FC_REFLECT( xgt::plugins::machine::adjure_args,
   (bytecode) )

FC_REFLECT( xgt::plugins::machine::adjure_return,
   (return_code) )
