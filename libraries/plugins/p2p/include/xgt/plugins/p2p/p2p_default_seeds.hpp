#pragma once

#include <string>
#include <vector>

namespace xgt{ namespace plugins { namespace p2p {

#ifdef IS_TEST_NET
const std::vector< std::string > default_seeds;
#else
const std::vector< std::string > default_seeds = {
    "xgt.rag.pub",
    "xgt2.rag.pub",
    "68.129.31.2",
    "95.216.69.92",
    "95.216.71.199"
    "98.33.76.100",
    "116.202.114.157",
    "195.201.167.19",
};
#endif

} } } // xgt::plugins::p2p
