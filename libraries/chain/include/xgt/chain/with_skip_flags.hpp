#pragma once

#include "xgt/protocol/operations.hpp"
#include <xgt/chain/database.hpp>

/*
 * This file provides with() functions which modify the database
 * temporarily, then restore it.  These functions are mostly internal
 * implementation detail of the database.
 *
 * Essentially, we want to be able to use "finally" to restore the
 * database regardless of whether an exception is thrown or not, but there
 * is no "finally" in C++.  Instead, C++ requires us to create a struct
 * and put the finally block in a destructor.  Aagh!
 */

namespace xgt { namespace chain { namespace detail {
/**
 * Class used to help the with_skip_flags implementation.
 * It must be defined in this header because it must be
 * available to the with_skip_flags implementation,
 * which is a template and therefore must also be defined
 * in this header.
 */
struct skip_flags_restorer
{
   skip_flags_restorer( node_property_object& npo, uint32_t old_skip_flags )
      : _npo( npo ), _old_skip_flags( old_skip_flags )
   {}

   ~skip_flags_restorer() { _npo.skip_flags = _old_skip_flags; }

   node_property_object& _npo;
   uint32_t _old_skip_flags;      // initialized in ctor
};

/**
 * Set the skip_flags to the given value, call callback,
 * then reset skip_flags to their previous value after
 * callback is done.
 */
template< typename Lambda >
void with_skip_flags(
   database& db,
   uint32_t skip_flags,
   Lambda callback )
{
   node_property_object& npo = db.node_properties();
   skip_flags_restorer restorer( npo, npo.skip_flags );
   npo.skip_flags = skip_flags;
   callback();
   return;
}

} } } // xgt::chain::detail
