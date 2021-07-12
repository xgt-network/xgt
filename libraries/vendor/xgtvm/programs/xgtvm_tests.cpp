#include <cassert>
#include "machine.hpp"

#define test_that(message) \
  std::cerr << "\e[34m" << ( message ) << "\e[0m" << std::endl;
#define assert_message(message, assertion) \
  { \
    bool result = ( assertion ); \
    std::cerr << "  " << ( result ? "\e[32m" : "\e[31m" ) << ( message ) << "\e[0m" << std::endl; \
  }

int main(int argc, char** argv)
{
  test_that("machine runs and halts")
  {
    std::vector<machine::word> input = {0x00};
    machine::context ctx = {true, 0x00};
    machine::message msg = {};
    machine::machine m(ctx, input, msg);
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
    machine::machine m(ctx, input, msg);
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
    machine::machine m(ctx, input, msg);
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
    machine::machine m(ctx, input, msg);
    while (m.is_running())
      m.step();
    assert_message( "machine should stop when done executing", m.get_state() == machine::machine_state::stopped );
    assert_message( "stack has correct length", m.stack_length() == 1 );
    assert_message( "top of stack has correct value", m.peek_word() == 5 );
  }

  return 0;
}
