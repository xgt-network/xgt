#pragma once

#include <string>
#include <vector>

namespace xgt{ namespace plugins { namespace p2p {

#ifdef IS_TEST_NET
const std::vector< std::string > default_seeds;
#else
const std::vector< std::string > default_seeds = {
};
#endif

} } } // xgt::plugins::p2p
