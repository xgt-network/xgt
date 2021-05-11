#pragma once

#include <vector>

namespace xgt{ namespace plugins { namespace p2p {

#ifdef IS_TEST_NET
const std::vector< std::string > default_seeds;
#else
// TODO: Replace these
const std::vector< std::string > default_seeds = {
};
#endif

} } } // xgt::plugins::p2p
