#pragma once

#include <mira/multi_index_container.hpp>

namespace mira {

template< typename Value, typename IndexSpecifierList >
struct multi_index_adapter
{
   typedef multi_index_container< Value, IndexSpecifierList > mira_container;
};

}