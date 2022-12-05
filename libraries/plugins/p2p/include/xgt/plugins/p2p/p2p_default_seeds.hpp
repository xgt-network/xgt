#pragma once

#include <string>
#include <vector>

namespace xgt{ namespace plugins { namespace p2p {

#ifdef IS_TEST_NET
const std::vector< std::string > default_seeds;
#else
const std::vector< std::string > default_seeds = {
  "seed-node-1.xgt.co.in:2001",
  "mainnet.xgtcrypto.com:2001",
  "73.202.78.72:2001",
  "45.56.83.92:2001",
  "68.129.31.2:2001",
  "95.216.69.92:2001",
  "95.216.71.199:2001",
  "116.202.114.157:2001"
};
#endif

} } } // xgt::plugins::p2p
