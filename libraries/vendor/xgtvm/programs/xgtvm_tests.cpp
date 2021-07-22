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
