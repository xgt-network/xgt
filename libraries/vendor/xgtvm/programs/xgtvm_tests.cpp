#include <cassert>
#include "machine.hpp"

#define test_that(message) \
  std::cerr << "\e[34m" << ( message ) << "\e[0m" << std::endl;
#define assert_message(message, assertion) \
  { \
    bool result = ( assertion ); \
    std::cerr << "  " << ( result ? "\e[32m" : "\e[31m" ) << ( message ) << "\e[0m" << std::endl; \
  }

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

  std::function< machine::big_word(std::string) > access_storage = [](std::string key) -> machine::big_word
  {
    return {};
  };

  std::function< bool(std::string, machine::big_word) > set_storage = [](std::string key, machine::big_word value) -> bool
  {
    return {};
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
    access_storage,
    set_storage,
    contract_return,
    self_destruct,
    get_input_data
  };

  return adapter;
}

int main(int argc, char** argv)
{
  test_that("machine runs and halts")
  {
    std::vector<machine::word> input = {0x00};
    machine::context ctx = {true, 0x00};
    machine::message msg = {};
    machine::chain_adapter adapter = make_chain_adapter();
    machine::machine m(ctx, input, msg, adapter);
    assert_message( "machine should start running", m.get_state() == machine::machine_state::running );
    while (m.is_running())
      m.step();
    assert_message( "machine should stop when done executing", m.get_state() == machine::machine_state::stopped );
    assert_message( "stack has correct length", m.stack_length() == 0 );
  }

  test_that("machine halts when nothing else to do")
  {
    std::vector<machine::word> input = {};
    machine::context ctx = {true, 0x00};
    machine::message msg = {};
    machine::chain_adapter adapter = make_chain_adapter();
    machine::machine m(ctx, input, msg, adapter);
    while (m.is_running())
      m.step();
    assert_message( "machine should stop when done executing", m.get_state() == machine::machine_state::stopped );
    assert_message( "stack has correct length", m.stack_length() == 0 );
  }

  test_that("machine returns a timestamp")
  {
    std::vector<machine::word> input = {0x42, 0x00};
    machine::context ctx = {true, 0x5c477758};
    machine::message msg = {};
    machine::chain_adapter adapter = make_chain_adapter();
    machine::machine m(ctx, input, msg, adapter);
    while (m.is_running())
      m.step();
    assert_message( "machine should stop when done executing", m.get_state() == machine::machine_state::stopped );
    assert_message( "stack has correct length", m.stack_length() == 1 );
    assert_message( "top of stack has correct value", m.peek_word() == 0x5c477758 );
  }

  test_that("machine adds unsigned 8-bit values")
  {
    std::vector<machine::word> input = {0x60, 0x02, 0x60, 0x03, 0x01, 0x00};
    machine::context ctx = {true, 0x00};
    machine::message msg = {};
    machine::chain_adapter adapter = make_chain_adapter();
    machine::machine m(ctx, input, msg, adapter);
    while (m.is_running())
      m.step();
    assert_message( "machine should stop when done executing", m.get_state() == machine::machine_state::stopped );
    assert_message( "stack has correct length", m.stack_length() == 1 );
    assert_message( "top of stack has correct value", m.peek_word() == 5 );
  }

  test_that("machine can get a wallet's balance via the chain adapter")
  {
    std::vector<machine::word> input = {0x31, 0x00};
    machine::context ctx = {true, 0x00};
    machine::message msg = {};
    machine::chain_adapter adapter = make_chain_adapter();
    adapter.get_balance = [](std::string wallet_name) -> uint64_t
    {
      return 2;
    };
    machine::machine m(ctx, input, msg, adapter);
    m.push_string("alice");
    while (m.is_running())
      m.step();
    assert_message( "machine should stop when done executing", m.get_state() == machine::machine_state::stopped );
    assert_message( "stack has correct length", m.stack_length() == 1 );
    assert_message( "top of stack has correct value", m.peek_word() == 2 );
  }

  return 0;
}
