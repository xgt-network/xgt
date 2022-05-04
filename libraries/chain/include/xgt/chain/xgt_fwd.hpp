#pragma once

// This header forward-declares pack/unpack and to/from variant functions for Xgt types.
// These declarations need to be as early as possible to prevent compiler errors.

#include <chainbase/util/object_id.hpp>

#include <mira/multi_index_container_fwd.hpp>

namespace xgt { namespace chain { using mira::multi_index_container; } }

namespace fc {

namespace raw {

template<typename Stream, typename T>
inline void pack( Stream& s, const chainbase::oid<T>& id );
template<typename Stream, typename T>
inline void unpack( Stream& s, chainbase::oid<T>& id, uint32_t depth = 0 );

template<typename Stream, typename E, typename A>
void pack( Stream& s, const boost::container::deque< E, A >& value );
template<typename Stream, typename E, typename A>
void unpack( Stream& s, boost::container::deque< E, A >& value, uint32_t depth = 0  );

template<typename Stream, typename K, typename V, typename C, typename A>
void pack( Stream& s, const boost::container::flat_map< K, V, C, A >& value );
template<typename Stream, typename K, typename V, typename C, typename A>
void unpack( Stream& s, boost::container::flat_map< K, V, C, A >& value, uint32_t depth = 0  );

} }
