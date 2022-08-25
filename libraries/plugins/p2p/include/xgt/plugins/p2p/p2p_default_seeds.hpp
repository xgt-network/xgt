#pragma once

#include <string>
#include <vector>

namespace xgt{ namespace plugins { namespace p2p {

#ifdef IS_TEST_NET
const std::vector< std::string > default_seeds;
#else
const std::vector< std::string > default_seeds = {
  "http://seed-node-1.xgt.co.in:8751",
  "http://mainnet.xgtcrypto.com:8751",
  "http://73.202.78.72:8751",
  "http://45.56.83.92:8751",
  "http://68.129.31.2:8751",
  "http://95.216.69.92:8751",
  "http://95.216.71.199:8751",
  "http://116.202.114.157:8751"
};
#endif

} } } // xgt::plugins::p2p
