#pragma once

#include <string>
#include <vector>

namespace xgt{ namespace plugins { namespace p2p {

#ifdef IS_TEST_NET
const std::vector< std::string > default_seeds;
#else
const std::vector< std::string > default_seeds = {
    "98.33.76.100",
    "xgt.rag.pub",
    "xgt2.rag.pub",
    "45.138.27.42",
    "68.129.31.2",
    "116.202.114.157",
    "195.201.167.19"
};
#endif

} } } // xgt::plugins::p2p
