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

machine::chain_adapter make_chain_adapter()
{
  std::function< uint64_t(std::string) > get_balance = [](std::string wallet_name) -> uint64_t
  {
    return 0;
  };

  machine::chain_adapter adapter = {
    get_balance
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
      std::cerr << "step\n";
      m.step();
      // Print out any logging that was generated
      if (debug_flag)
      {
        while ( std::getline(m.get_logger(), line) )
          std::cerr << "\e[36m" << "LOG: " << line << "\e[0m" << std::endl;
      }
    }
    while ( std::getline(m.get_logger(), line) )
      std::cerr << "\e[36m" << "LOG: " << line << "\e[0m" << std::endl;
    std::cout << m.to_json() << std::endl;
  }

  return 0;
}
