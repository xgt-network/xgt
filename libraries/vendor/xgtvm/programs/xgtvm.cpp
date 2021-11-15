#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string>
#include <algorithm>
#include <iterator>
#include "machine.hpp"
#include "unistd.h"

std::string process_stdin()
{
  std::cin >> std::noskipws;
  std::istream_iterator<char> it(std::cin);
  std::istream_iterator<char> end;
  std::string results(it, end);
  return results;
}

machine::word to_hex(std::string str)
{
  return std::stoi(str, 0, 16);
}

std::vector<machine::word> process_eval(const std::string& str)
{
  char delim = ' ';
  std::size_t current, previous = 0;
  std::string token;
  std::vector<machine::word> tokens;
  current = str.find(delim);
  while (current != std::string::npos) {
    tokens.push_back( to_hex( str.substr(previous, current - previous) ) );
    previous = current + 1;
    current = str.find(delim, previous);
  }
  token = str.substr(previous, current - previous);
  tokens.push_back( to_hex(token) );
  return tokens;
}

// Set by --debug or -d
static int debug_flag;
// Set by --eval or -e
static int eval_flag;
static char* eval_cstr;
// Set by --help or -h
static int help_flag;
static struct option long_options[] = {
  {"eval", required_argument, 0, 'e'},
  {"debug", no_argument, &debug_flag, 'd'},
  {"help", no_argument, &help_flag, 1},
  {0, 0, 0, 0}
};

// chain_adapter is state.host equivalent in evmone
machine::chain_adapter make_chain_adapter()
{
  std::function< std::string(std::vector<machine::word>) > sha3 = [](std::vector<machine::word> memory) -> std::string
  {
    return 0;
  };

  std::function< uint64_t(std::string) > get_balance = [](std::string address) -> uint64_t
  {
    return 0;
  };

  std::function< std::string(std::string) > get_code_hash = [](std::string address) -> std::string
  {
    return "";
  };

  std::function< machine::big_word(uint64_t) > get_block_hash = [](uint64_t block_num) -> machine::big_word
  {
    return {};
  };

  std::function< std::vector<machine::word>(std::string) > get_code_at_addr = [](std::string address) -> std::vector<machine::word>
  {
    return {};
  };

  std::function< std::string(std::vector<machine::word>, machine::big_word) > contract_create = [](std::vector<machine::word> memory, machine::big_word value) -> std::string
  {
    return {};
  };

  std::function< std::vector<machine::word>(std::string, uint64_t, machine::big_word, std::vector<machine::word>) > contract_call = [](std::string address, uint64_t energy, machine::big_word value, std::vector<machine::word> args) -> std::vector<machine::word>
  {
    return {};
  };

  std::function< std::vector<machine::word>(std::string, uint64_t, machine::big_word, std::vector<machine::word>) > contract_callcode = [](std::string address, uint64_t energy, machine::big_word value, std::vector<machine::word> args) -> std::vector<machine::word>
  {
    return {};
  };

  std::function< std::vector<machine::word>(std::string, uint64_t, std::vector<machine::word>) > contract_delegatecall = [](std::string address, uint64_t energy, std::vector<machine::word> args) -> std::vector<machine::word>
  {
    return {};
  };

  std::function< std::vector<machine::word>(std::string, uint64_t, std::vector<machine::word>) > contract_staticcall = [](std::string address, uint64_t energy, std::vector<machine::word> args) -> std::vector<machine::word>
  {
    return {};
  };

  std::function< std::string(std::vector<machine::word>, machine::big_word, std::string) > contract_create2 = [](std::vector<machine::word> memory, machine::big_word value, std::string salt) -> std::string
  {
    return {};
  };

  std::function< bool(std::vector<machine::word>) > revert = [](std::vector<machine::word> memory) -> bool
  {
    return {};
  };

  std::function< machine::big_word(machine::big_word) > get_storage = [](machine::big_word) -> machine::big_word
  {
    return 0;
  };

  std::function< void(machine::big_word, machine::big_word) > set_storage = [](machine::big_word, machine::big_word value) -> void
  {
  };

  std::function< bool(std::vector<machine::word>) > contract_return = [](std::vector<machine::word> memory) -> bool
  {
    return {};
  };

  std::function< bool(std::string) > self_destruct = [](std::string address) -> bool
  {
    return {};
  };

  std::function< std::vector<machine::word>(std::string) > get_input_data = [](std::string address) -> std::vector<machine::word>
  {
    return {};
  };

  machine::chain_adapter adapter = {
    sha3,
    get_balance,
    get_code_hash,
    get_block_hash,
    get_code_at_addr,
    contract_create,
    contract_call,
    contract_callcode,
    contract_delegatecall,
    contract_staticcall,
    contract_create2,
    revert,
    get_storage,
    set_storage,
    contract_return,
    self_destruct,
    get_input_data
  };

  return adapter;
}

int main(int argc, char** argv)
{
  int c;
  for (;;)
  {
    int option_index = 0;
    c = getopt_long(argc, argv, "de:h", long_options, &option_index);
    if (c == -1)
      break;
    switch(c)
    {
      case 'd':
        debug_flag = 1;
        break;
      case 'e':
        eval_flag = 1;
        eval_cstr = optarg;
        break;
      case 'h':
        help_flag = 1;
        break;
    }
  }

  // TODO: Add a help message
  if (help_flag)
  {}

  std::string input = process_stdin();
  if(eval_flag)
    input = std::string(eval_cstr);
  if (input.size() > 0)
  {
    machine::context ctx = {true, 0x5c477758};
    machine::message msg = {};
    std::vector<machine::word> code = process_eval(input);
    machine::chain_adapter adapter = make_chain_adapter();
    machine::machine m(ctx, code, msg, adapter);
    std::string line;
    while (m.is_running())
    {
      if (debug_flag)
        std::cerr << "step\n";
      m.step();
      // Print out any logging that was generated
      if (debug_flag)
        while ( std::getline(m.get_logger(), line) )
          std::cerr << "\e[36m" << "LOG: " << line << "\e[0m" << std::endl;
    }
    if (debug_flag)
      while ( std::getline(m.get_logger(), line) )
        std::cerr << "\e[36m" << "LOG: " << line << "\e[0m" << std::endl;
    std::cout << m.to_json() << std::endl;
  }

  return 0;
}
