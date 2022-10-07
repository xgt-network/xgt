#include "include/rlpvalue.h"
#include "lib/rlpvalue.cpp"
#include "lib/rlpvalue_get.cpp"
#include "lib/rlpvalue_write.cpp"
#include "test/utilstrencodings.cpp"
#include <iostream>

int main() {
  std::string dogIns = "dog";
  std::string dogOuts = "83646f67";

  // Parse input string
  RLPValue dogV = RLPValue(dogIns);

  // Parse expected output, print corresponding input -- note that 0x has been stripped from hex output
  std::vector<unsigned char> outb = ParseHex(dogOuts);
  std::string outbStr(outb.begin(), outb.end());

  std::cout << "83646f67 to c_str:" << std::endl;
  std::cout << outbStr << std::endl;

  // Generate output rlp-encoded hex
  std::string dogGenOutput = dogV.write();
  std::string dogGenHex = HexStr( dogGenOutput.begin(), dogGenOutput.end() );

  // Inspect type of RLP value -- buffer/array
  std::cout << "\nRLPValue type: " << std::endl;
  std::cout << uvTypeName( dogV.type() ) << "\n\n" <<  std::endl;



  std::string catIns = "cat";
  std::string catOuts = "83636174";

  RLPValue catV = RLPValue(catIns);

  // Generate output rlp-encoded hex
  std::string catGenOutput = catV.write();
  std::string catGenHex = HexStr( catGenOutput.begin(), catGenOutput.end() );

  std::cout << "Cat to RLP:" << std::endl;
  std::cout << catGenHex.c_str() << std::endl;

  std::cout << "\nRLPValue type: " << std::endl;
  std::cout << uvTypeName( dogV.type() ) <<  std::endl;

  return 0;
}
