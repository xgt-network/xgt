#pragma once

#include <mira/multi_index_container.hpp>
#include <mira/ordered_index.hpp>
#include <mira/tag.hpp>
#include <mira/member.hpp>
#include <mira/indexed_by.hpp>
#include <mira/composite_key.hpp>
#include <mira/mem_fun.hpp>

#include <type_traits>
#include <typeinfo>

namespace xgt { namespace chain {

using mira::multi_index::indexed_by;
using mira::multi_index::ordered_unique;
using mira::multi_index::tag;
using mira::multi_index::member;
using mira::multi_index::composite_key;
using mira::multi_index::composite_key_compare;
using mira::multi_index::const_mem_fun;

template< class Iterator >
inline Iterator make_reverse_iterator( Iterator iterator )
{
   return iterator.reverse();
}

} } // xgt::chain
