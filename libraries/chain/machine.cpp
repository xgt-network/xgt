#include "xgt/chain/machine.hpp"

namespace xgt { namespace chain {
   namespace detail {
      class machine_impl {
         public:
            machine_impl( machine& self, machine_context ctx, std::vector<machine_word> code )
            : _self(self), ctx(ctx), code(code), pc(0), state(machine_state::running)
            {

            }

            void step()
            {
            }

            machine& _self;
            machine_context ctx;
            std::vector<machine_word> code;
            machine_address pc;
            std::deque<machine_big_word> stack;
            machine_state state;
      };
   }

   machine::machine( machine_context ctx, std::vector<machine_word> code )
   : my ( new detail::machine_impl(*this, ctx, code) )
   {
   
   }

   machine::~machine()
   {
   }

   void machine::step()
   {
      my->step();
   }
} } // xgt::chain
