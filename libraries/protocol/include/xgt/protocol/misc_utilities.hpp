#pragma once

namespace xgt { namespace protocol {

enum curve_id
{
   quadratic,
   bounded,
   linear,
   square_root,
   convergent_linear,
   convergent_square_root
};

} } // xgt::utilities


FC_REFLECT_ENUM(
   xgt::protocol::curve_id,
   (quadratic)
   (bounded)
   (linear)
   (square_root)
   (convergent_linear)
   (convergent_square_root)
)
