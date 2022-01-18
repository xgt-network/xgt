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
    std::cout << "chain_adapter::sha3" << std::endl;
    return 0;
  };

  std::function< machine::big_word(machine::big_word) > get_balance = [](machine::big_word address_ripemd160) -> machine::big_word
  {
    std::cout << "chain_adapter::get_balance" << std::endl;
    return 0;
  };

  std::function< std::string(std::string) > get_code_hash = [](std::string address) -> std::string
  {
    std::cout << "chain_adapter::get_code_hash" << std::endl;
    return "";
  };

  std::function< machine::big_word(uint64_t) > get_block_hash = [](uint64_t block_num) -> machine::big_word
  {
    std::cout << "chain_adapter::get_block_hash" << std::endl;
    return {};
  };

  std::function< std::vector<machine::word>(machine::big_word) > get_code_at_addr = [](machine::big_word address) -> std::vector<machine::word>
  {
    std::cout << "chain_adapter::get_code_at_addr" << std::endl;
    return {};
  };

  std::function< machine::big_word(std::vector<machine::word>, machine::big_word) > contract_create = [](std::vector<machine::word> memory, machine::big_word value) -> machine::big_word
  {
    std::cout << "chain_adapter::contract_create" << std::endl;
    return {};
  };

  std::function< std::pair< machine::word, std::vector<machine::word> >(machine::big_word, uint64_t, machine::big_word, std::vector<machine::word>) > contract_call = [](machine::big_word address, uint64_t energy, machine::big_word value, std::vector<machine::word> args) -> std::pair< machine::word, std::vector<machine::word> >
  {
    std::cout << "chain_adapter::contract_call" << std::endl;
    return {};
  };

  std::function< std::vector<machine::word>(std::string, uint64_t, machine::big_word, std::vector<machine::word>) > contract_callcode = [](std::string address, uint64_t energy, machine::big_word value, std::vector<machine::word> args) -> std::vector<machine::word>
  {
    std::cout << "chain_adapter::contract_callcode" << std::endl;
    return {};
  };

  std::function< std::pair< machine::word, std::vector<machine::word> >(machine::big_word, uint64_t, std::vector<machine::word>) > contract_delegatecall = [](machine::big_word address, uint64_t energy, std::vector<machine::word> args) -> std::pair< machine::word, std::vector<machine::word> >
  {
    std::cout << "chain_adapter::contract_delegatecall" << std::endl;
    return {};
  };

  std::function< std::pair< machine::word, std::vector<machine::word> >(machine::big_word, uint64_t, std::vector<machine::word>) > contract_staticcall = [](machine::big_word address, uint64_t energy, std::vector<machine::word> args) -> std::pair< machine::word, std::vector<machine::word> >
  {
    std::cout << "chain_adapter::contract_staticcall" << std::endl;
    return {};
  };

  std::function< machine::big_word(std::vector<machine::word>, machine::big_word, std::string) > contract_create2 = [](std::vector<machine::word> memory, machine::big_word value, std::string salt) -> machine::big_word
  {
    std::cout << "chain_adapter::contract_create2" << std::endl;
    return {};
  };

  std::function< bool(std::vector<machine::word>) > revert = [](std::vector<machine::word> memory) -> bool
  {
    std::cout << "chain_adapter::revert" << std::endl;
    return {};
  };

  std::function< machine::big_word(machine::big_word) > get_storage = [](machine::big_word) -> machine::big_word
  {
    std::cout << "chain_adapter::get_storage" << std::endl;
    return 0;
  };

  std::function< bool(machine::big_word, machine::big_word) > set_storage = [](machine::big_word, machine::big_word value) -> bool
  {
    std::cout << "chain_adapter::set_storage" << std::endl;
    return false;
  };

  std::function< std::vector<machine::word>(std::vector<machine::word>) > contract_return = [](std::vector<machine::word> memory) -> std::vector<machine::word>
  {
    std::cout << "chain_adapter::contract_return" << std::endl;
    return {};
  };

  std::function< bool(std::string) > self_destruct = [](std::string address) -> bool
  {
    std::cout << "chain_adapter::self_destruct" << std::endl;
    return {};
  };

  std::function< std::vector<machine::word>(std::string) > get_input_data = [](std::string address) -> std::vector<machine::word>
  {
    std::cout << "chain_adapter::get_input_data" << std::endl;
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
  //test_that("machine runs and halts")
  //{
  //  std::vector<machine::word> input = {0x00};
  //  machine::context ctx = {true, 0x00};
  //  machine::message msg = {};
  //  machine::chain_adapter adapter = make_chain_adapter();
  //  machine::machine m(ctx, input, msg, adapter);
  //  assert_message( "machine should start running", m.get_state() == machine::machine_state::running );
  //  while (m.is_running())
  //    m.step();
  //  assert_message( "machine should stop when done executing", m.get_state() == machine::machine_state::stopped );
  //  assert_message( "stack has correct length", m.stack_length() == 0 );
  //}

  //test_that("machine halts when nothing else to do")
  //{
  //  std::vector<machine::word> input = {};
  //  machine::context ctx = {true, 0x00};
  //  machine::message msg = {};
  //  machine::chain_adapter adapter = make_chain_adapter();
  //  machine::machine m(ctx, input, msg, adapter);
  //  while (m.is_running())
  //    m.step();
  //  assert_message( "machine should stop when done executing", m.get_state() == machine::machine_state::stopped );
  //  assert_message( "stack has correct length", m.stack_length() == 0 );
  //}

  //test_that("machine returns a timestamp")
  //{
  //  std::vector<machine::word> input = {0x42, 0x00};
  //  machine::context ctx = {true, 0x5c477758};
  //  machine::message msg = {};
  //  machine::chain_adapter adapter = make_chain_adapter();
  //  machine::machine m(ctx, input, msg, adapter);
  //  while (m.is_running())
  //    m.step();
  //  assert_message( "machine should stop when done executing", m.get_state() == machine::machine_state::stopped );
  //  assert_message( "stack has correct length", m.stack_length() == 1 );
  //  assert_message( "top of stack has correct value", m.peek_word() == 0x5c477758 );
  //}

  //test_that("machine adds unsigned 8-bit values")
  //{
  //  std::vector<machine::word> input = {0x60, 0x02, 0x60, 0x03, 0x01, 0x00};
  //  machine::context ctx = {true, 0x00};
  //  machine::message msg = {};
  //  machine::chain_adapter adapter = make_chain_adapter();
  //  machine::machine m(ctx, input, msg, adapter);
  //  while (m.is_running())
  //    m.step();
  //  assert_message( "machine should stop when done executing", m.get_state() == machine::machine_state::stopped );
  //  assert_message( "stack has correct length", m.stack_length() == 1 );
  //  assert_message( "top of stack has correct value", m.peek_word() == 5 );
  //}

  //test_that("machine can store and load a value to/from memory")
  //{
  //  std::vector<machine::word> input = {0x60, 0x01, 0x60, 0x08, 0x52, 0x60, 0x08, 0x51, 0x00};
  //  machine::context ctx = {true, 0x00};
  //  machine::message msg = {};
  //  machine::chain_adapter adapter = make_chain_adapter();
  //  machine::machine m(ctx, input, msg, adapter);
  //  while (m.is_running())
  //    m.step();
  //  assert_message( "machine should stop when done executing", m.get_state() == machine::machine_state::stopped );
  //  assert_message( "stack has correct length", m.stack_length() == 1 );
  //  assert_message( "top of stack has correct value", m.peek_word() == 1 );
  //}

  //test_that("machine can get a wallet's balance via the chain adapter")
  //{
  //  std::vector<machine::word> input = {0x31, 0x00};
  //  machine::context ctx = {true, 0x00};
  //  machine::message msg = {};
  //  machine::chain_adapter adapter = make_chain_adapter();
  //  adapter.get_balance = [](std::string wallet_name) -> uint64_t
  //  {
  //    return 2;
  //  };
  //  machine::machine m(ctx, input, msg, adapter);
  //  m.push_string("alice");
  //  while (m.is_running())
  //    m.step();
  //  assert_message( "machine should stop when done executing", m.get_state() == machine::machine_state::stopped );
  //  assert_message( "stack has correct length", m.stack_length() == 1 );
  //  assert_message( "top of stack has correct value", m.peek_word() == 2 );
  //}

  // test_that("do nothing contract evaluates")
  // {
  //   std::vector<machine::word> input = {0x60, 0x80, 0x60, 0x40, 0x52, 0x34, 0x80, 0x15, 0x60, 0x0f, 0x57, 0x60, 0x00, 0x80, 0xfd, 0x5b, 0x50, 0x60, 0x6d, 0x80, 0x60, 0x1d, 0x60, 0x00, 0x39, 0x60, 0x00, 0xf3, 0xfe, 0x60, 0x80, 0x60, 0x40, 0x52, 0x34, 0x80, 0x15, 0x60, 0x0f, 0x57, 0x60, 0x00, 0x80, 0xfd, 0x5b, 0x50, 0x60, 0x04, 0x36, 0x10, 0x60, 0x28, 0x57, 0x60, 0x00, 0x35, 0x60, 0xe0, 0x1c, 0x80, 0x63, 0x2f, 0x57, 0x6f, 0x20, 0x14, 0x60, 0x2d, 0x57, 0x5b, 0x60, 0x00, 0x80, 0xfd, 0x5b, 0x60, 0x33, 0x60, 0x35, 0x56, 0x5b, 0x00, 0x5b, 0x56, 0xfe, 0xa2, 0x64, 0x69, 0x70, 0x66, 0x73, 0x58, 0x22, 0x12, 0x20, 0x97, 0x5b, 0xdb, 0x69, 0x49, 0x44, 0x25, 0x38, 0xb9, 0x0c, 0x14, 0xc4, 0x20, 0xb0, 0xa2, 0xf5, 0x47, 0xf0, 0x43, 0x7e, 0x85, 0xa7, 0x3d, 0xa1, 0x2f, 0xaf, 0x4e, 0x68, 0x34, 0xf0, 0x81, 0x72, 0x64, 0x73, 0x6f, 0x6c, 0x63, 0x43, 0x00, 0x08, 0x07, 0x00, 0x33};
  //   machine::context ctx = {true, 0x00};
  //   machine::message msg = {};
  //   machine::chain_adapter adapter = make_chain_adapter();
  //   machine::machine m(ctx, input, msg, adapter);
  //   assert_message( "machine should start running", m.get_state() == machine::machine_state::running );

  //   auto& logger = m.get_logger();
  //   std::string line;
  //   std::cerr << "\e[36m" << "LOG: " << m.to_json() << "\e[0m" << std::endl;
  //   while (m.is_running())
  //   {
  //     m.step();
  //     //// TODO: Figure out why logs not emitting
  //     //while ( std::getline(logger, line) )
  //     //  std::cerr << "\e[36m" << "LOG: " << line << "\e[0m" << std::endl;
  //     std::cerr << "\e[36m" << "LOG: " << m.to_json() << "\e[0m" << std::endl;
  //   }
  //   //while ( std::getline(logger, line) )
  //   //  std::cerr << "\e[36m" << "LOG: " << line << "\e[0m" << std::endl;
  //   std::cerr << "\e[36m" << "LOG: " << m.to_json() << "\e[0m" << std::endl;

  //   assert_message( "machine should stop when done executing", m.get_state() == machine::machine_state::stopped );
  //   assert_message( "stack has correct length", m.stack_length() == 0 );
  // }

  // test_that("storage contract evaluates")
  // {
  //   std::vector<machine::word> input = {0x60, 0x80, 0x60, 0x40, 0x52, 0x34, 0x80, 0x15, 0x61, 0x00, 0x10, 0x57, 0x60, 0x00, 0x80, 0xfd, 0x5b, 0x50, 0x61, 0x01, 0x50, 0x80, 0x61, 0x00, 0x20, 0x60, 0x00, 0x39, 0x60, 0x00, 0xf3, 0xfe, 0x60, 0x80, 0x60, 0x40, 0x52, 0x34, 0x80, 0x15, 0x61, 0x00, 0x10, 0x57, 0x60, 0x00, 0x80, 0xfd, 0x5b, 0x50, 0x60, 0x04, 0x36, 0x10, 0x61, 0x00, 0x36, 0x57, 0x60, 0x00, 0x35, 0x60, 0xe0, 0x1c, 0x80, 0x63, 0x2e, 0x64, 0xce, 0xc1, 0x14, 0x61, 0x00, 0x3b, 0x57, 0x80, 0x63, 0x60, 0x57, 0x36, 0x1d, 0x14, 0x61, 0x00, 0x59, 0x57, 0x5b, 0x60, 0x00, 0x80, 0xfd, 0x5b, 0x61, 0x00, 0x43, 0x61, 0x00, 0x75, 0x56, 0x5b, 0x60, 0x40, 0x51, 0x61, 0x00, 0x50, 0x91, 0x90, 0x61, 0x00, 0xd9, 0x56, 0x5b, 0x60, 0x40, 0x51, 0x80, 0x91, 0x03, 0x90, 0xf3, 0x5b, 0x61, 0x00, 0x73, 0x60, 0x04, 0x80, 0x36, 0x03, 0x81, 0x01, 0x90, 0x61, 0x00, 0x6e, 0x91, 0x90, 0x61, 0x00, 0x9d, 0x56, 0x5b, 0x61, 0x00, 0x7e, 0x56, 0x5b, 0x00, 0x5b, 0x60, 0x00, 0x80, 0x54, 0x90, 0x50, 0x90, 0x56, 0x5b, 0x80, 0x60, 0x00, 0x81, 0x90, 0x55, 0x50, 0x50, 0x56, 0x5b, 0x60, 0x00, 0x81, 0x35, 0x90, 0x50, 0x61, 0x00, 0x97, 0x81, 0x61, 0x01, 0x03, 0x56, 0x5b, 0x92, 0x91, 0x50, 0x50, 0x56, 0x5b, 0x60, 0x00, 0x60, 0x20, 0x82, 0x84, 0x03, 0x12, 0x15, 0x61, 0x00, 0xb3, 0x57, 0x61, 0x00, 0xb2, 0x61, 0x00, 0xfe, 0x56, 0x5b, 0x5b, 0x60, 0x00, 0x61, 0x00, 0xc1, 0x84, 0x82, 0x85, 0x01, 0x61, 0x00, 0x88, 0x56, 0x5b, 0x91, 0x50, 0x50, 0x92, 0x91, 0x50, 0x50, 0x56, 0x5b, 0x61, 0x00, 0xd3, 0x81, 0x61, 0x00, 0xf4, 0x56, 0x5b, 0x82, 0x52, 0x50, 0x50, 0x56, 0x5b, 0x60, 0x00, 0x60, 0x20, 0x82, 0x01, 0x90, 0x50, 0x61, 0x00, 0xee, 0x60, 0x00, 0x83, 0x01, 0x84, 0x61, 0x00, 0xca, 0x56, 0x5b, 0x92, 0x91, 0x50, 0x50, 0x56, 0x5b, 0x60, 0x00, 0x81, 0x90, 0x50, 0x91, 0x90, 0x50, 0x56, 0x5b, 0x60, 0x00, 0x80, 0xfd, 0x5b, 0x61, 0x01, 0x0c, 0x81, 0x61, 0x00, 0xf4, 0x56, 0x5b, 0x81, 0x14, 0x61, 0x01, 0x17, 0x57, 0x60, 0x00, 0x80, 0xfd, 0x5b, 0x50, 0x56, 0xfe, 0xa2, 0x64, 0x69, 0x70, 0x66, 0x73, 0x58, 0x22, 0x12, 0x20, 0x40, 0x4e, 0x37, 0xf4, 0x87, 0xa8, 0x9a, 0x93, 0x2d, 0xca, 0x5e, 0x77, 0xfa, 0xaf, 0x6c, 0xa2, 0xde, 0x3b, 0x99, 0x1f, 0x93, 0xd2, 0x30, 0x60, 0x4b, 0x1b, 0x8d, 0xaa, 0xef, 0x64, 0x76, 0x62, 0x64, 0x73, 0x6f, 0x6c, 0x63, 0x43, 0x00, 0x08, 0x07, 0x00, 0x33};
  //   machine::context ctx = {true, 0x00};
  //   machine::message msg = {};
  //   machine::chain_adapter adapter = make_chain_adapter();
  //   machine::machine m(ctx, input, msg, adapter);
  //   assert_message( "machine should start running", m.get_state() == machine::machine_state::running );

  //   auto& logger = m.get_logger();
  //   std::string line;
  //   std::cerr << "\e[36m" << "LOG: " << m.to_json() << "\e[0m" << std::endl;
  //   while (m.is_running())
  //   {
  //     m.step();
  //     //// TODO: Figure out why logs not emitting
  //     //while ( std::getline(logger, line) )
  //     //  std::cerr << "\e[36m" << "LOG: " << line << "\e[0m" << std::endl;
  //     std::cerr << "\e[36m" << "LOG: " << m.to_json() << "\e[0m" << std::endl;
  //   }
  //   //while ( std::getline(logger, line) )
  //   //  std::cerr << "\e[36m" << "LOG: " << line << "\e[0m" << std::endl;
  //   std::cerr << "\e[36m" << "LOG: " << m.to_json() << "\e[0m" << std::endl;

  //   assert_message( "machine should stop when done executing", m.get_state() == machine::machine_state::stopped );
  //   assert_message( "stack has correct length", m.stack_length() == 0 );
  // }

  //test_that("first lottery contract evaluates")
  //{
  //  std::vector<machine::word> input = {0x60, 0x80, 0x60, 0x40, 0x52, 0x34, 0x80, 0x15, 0x61, 0x00, 0x10, 0x57, 0x60, 0x00, 0x80, 0xfd, 0x5b, 0x50, 0x33, 0x60, 0x00, 0x80, 0x61, 0x01, 0x00, 0x0a, 0x81, 0x54, 0x81, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x02, 0x19, 0x16, 0x90, 0x83, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x02, 0x17, 0x90, 0x55, 0x50, 0x61, 0x06, 0x80, 0x80, 0x61, 0x00, 0x60, 0x60, 0x00, 0x39, 0x60, 0x00, 0xf3, 0x00, 0x60, 0x80, 0x60, 0x40, 0x52, 0x60, 0x04, 0x36, 0x10, 0x61, 0x00, 0x6d, 0x57, 0x60, 0x00, 0x35, 0x7c, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x04, 0x63, 0xff, 0xff, 0xff, 0xff, 0x16, 0x80, 0x63, 0x48, 0x1c, 0x6a, 0x75, 0x14, 0x61, 0x00, 0x72, 0x57, 0x80, 0x63, 0x5d, 0x49, 0x5a, 0xea, 0x14, 0x61, 0x00, 0xc9, 0x57, 0x80, 0x63, 0x8b, 0x5b, 0x9c, 0xcc, 0x14, 0x61, 0x00, 0xe0, 0x57, 0x80, 0x63, 0xe9, 0x7d, 0xcb, 0x62, 0x14, 0x61, 0x01, 0x4c, 0x57, 0x80, 0x63, 0xf7, 0x1d, 0x96, 0xcb, 0x14, 0x61, 0x01, 0x56, 0x57, 0x5b, 0x60, 0x00, 0x80, 0xfd, 0x5b, 0x34, 0x80, 0x15, 0x61, 0x00, 0x7e, 0x57, 0x60, 0x00, 0x80, 0xfd, 0x5b, 0x50, 0x61, 0x00, 0x87, 0x61, 0x01, 0xc3, 0x56, 0x5b, 0x60, 0x40, 0x51, 0x80, 0x82, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x81, 0x52, 0x60, 0x20, 0x01, 0x91, 0x50, 0x50, 0x60, 0x40, 0x51, 0x80, 0x91, 0x03, 0x90, 0xf3, 0x5b, 0x34, 0x80, 0x15, 0x61, 0x00, 0xd5, 0x57, 0x60, 0x00, 0x80, 0xfd, 0x5b, 0x50, 0x61, 0x00, 0xde, 0x61, 0x01, 0xe8, 0x56, 0x5b, 0x00, 0x5b, 0x34, 0x80, 0x15, 0x61, 0x00, 0xec, 0x57, 0x60, 0x00, 0x80, 0xfd, 0x5b, 0x50, 0x61, 0x00, 0xf5, 0x61, 0x03, 0x40, 0x56, 0x5b, 0x60, 0x40, 0x51, 0x80, 0x80, 0x60, 0x20, 0x01, 0x82, 0x81, 0x03, 0x82, 0x52, 0x83, 0x81, 0x81, 0x51, 0x81, 0x52, 0x60, 0x20, 0x01, 0x91, 0x50, 0x80, 0x51, 0x90, 0x60, 0x20, 0x01, 0x90, 0x60, 0x20, 0x02, 0x80, 0x83, 0x83, 0x60, 0x00, 0x5b, 0x83, 0x81, 0x10, 0x15, 0x61, 0x01, 0x38, 0x57, 0x80, 0x82, 0x01, 0x51, 0x81, 0x84, 0x01, 0x52, 0x60, 0x20, 0x81, 0x01, 0x90, 0x50, 0x61, 0x01, 0x1d, 0x56, 0x5b, 0x50, 0x50, 0x50, 0x50, 0x90, 0x50, 0x01, 0x92, 0x50, 0x50, 0x50, 0x60, 0x40, 0x51, 0x80, 0x91, 0x03, 0x90, 0xf3, 0x5b, 0x61, 0x01, 0x54, 0x61, 0x03, 0xce, 0x56, 0x5b, 0x00, 0x5b, 0x34, 0x80, 0x15, 0x61, 0x01, 0x62, 0x57, 0x60, 0x00, 0x80, 0xfd, 0x5b, 0x50, 0x61, 0x01, 0x81, 0x60, 0x04, 0x80, 0x36, 0x03, 0x81, 0x01, 0x90, 0x80, 0x80, 0x35, 0x90, 0x60, 0x20, 0x01, 0x90, 0x92, 0x91, 0x90, 0x50, 0x50, 0x50, 0x61, 0x04, 0x4b, 0x56, 0x5b, 0x60, 0x40, 0x51, 0x80, 0x82, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x81, 0x52, 0x60, 0x20, 0x01, 0x91, 0x50, 0x50, 0x60, 0x40, 0x51, 0x80, 0x91, 0x03, 0x90, 0xf3, 0x5b, 0x60, 0x00, 0x80, 0x90, 0x54, 0x90, 0x61, 0x01, 0x00, 0x0a, 0x90, 0x04, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x81, 0x56, 0x5b, 0x60, 0x00, 0x80, 0x60, 0x00, 0x90, 0x54, 0x90, 0x61, 0x01, 0x00, 0x0a, 0x90, 0x04, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x33, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x14, 0x15, 0x15, 0x61, 0x02, 0x45, 0x57, 0x60, 0x00, 0x80, 0xfd, 0x5b, 0x60, 0x01, 0x80, 0x54, 0x90, 0x50, 0x61, 0x02, 0x53, 0x61, 0x04, 0x89, 0x56, 0x5b, 0x81, 0x15, 0x15, 0x61, 0x02, 0x5c, 0x57, 0xfe, 0x5b, 0x06, 0x90, 0x50, 0x60, 0x01, 0x81, 0x81, 0x54, 0x81, 0x10, 0x15, 0x15, 0x61, 0x02, 0x6e, 0x57, 0xfe, 0x5b, 0x90, 0x60, 0x00, 0x52, 0x60, 0x20, 0x60, 0x00, 0x20, 0x01, 0x60, 0x00, 0x90, 0x54, 0x90, 0x61, 0x01, 0x00, 0x0a, 0x90, 0x04, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x61, 0x08, 0xfc, 0x30, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x31, 0x90, 0x81, 0x15, 0x02, 0x90, 0x60, 0x40, 0x51, 0x60, 0x00, 0x60, 0x40, 0x51, 0x80, 0x83, 0x03, 0x81, 0x85, 0x88, 0x88, 0xf1, 0x93, 0x50, 0x50, 0x50, 0x50, 0x15, 0x80, 0x15, 0x61, 0x02, 0xf5, 0x57, 0x3d, 0x60, 0x00, 0x80, 0x3e, 0x3d, 0x60, 0x00, 0xfd, 0x5b, 0x50, 0x60, 0x00, 0x60, 0x40, 0x51, 0x90, 0x80, 0x82, 0x52, 0x80, 0x60, 0x20, 0x02, 0x60, 0x20, 0x01, 0x82, 0x01, 0x60, 0x40, 0x52, 0x80, 0x15, 0x61, 0x03, 0x26, 0x57, 0x81, 0x60, 0x20, 0x01, 0x60, 0x20, 0x82, 0x02, 0x80, 0x38, 0x83, 0x39, 0x80, 0x82, 0x01, 0x91, 0x50, 0x50, 0x90, 0x50, 0x5b, 0x50, 0x60, 0x01, 0x90, 0x80, 0x51, 0x90, 0x60, 0x20, 0x01, 0x90, 0x61, 0x03, 0x3c, 0x92, 0x91, 0x90, 0x61, 0x05, 0x87, 0x56, 0x5b, 0x50, 0x50, 0x56, 0x5b, 0x60, 0x60, 0x60, 0x01, 0x80, 0x54, 0x80, 0x60, 0x20, 0x02, 0x60, 0x20, 0x01, 0x60, 0x40, 0x51, 0x90, 0x81, 0x01, 0x60, 0x40, 0x52, 0x80, 0x92, 0x91, 0x90, 0x81, 0x81, 0x52, 0x60, 0x20, 0x01, 0x82, 0x80, 0x54, 0x80, 0x15, 0x61, 0x03, 0xc4, 0x57, 0x60, 0x20, 0x02, 0x82, 0x01, 0x91, 0x90, 0x60, 0x00, 0x52, 0x60, 0x20, 0x60, 0x00, 0x20, 0x90, 0x5b, 0x81, 0x60, 0x00, 0x90, 0x54, 0x90, 0x61, 0x01, 0x00, 0x0a, 0x90, 0x04, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x81, 0x52, 0x60, 0x20, 0x01, 0x90, 0x60, 0x01, 0x01, 0x90, 0x80, 0x83, 0x11, 0x61, 0x03, 0x7a, 0x57, 0x5b, 0x50, 0x50, 0x50, 0x50, 0x50, 0x90, 0x50, 0x90, 0x56, 0x5b, 0x66, 0x23, 0x86, 0xf2, 0x6f, 0xc1, 0x00, 0x00, 0x34, 0x11, 0x15, 0x15, 0x61, 0x03, 0xe3, 0x57, 0x60, 0x00, 0x80, 0xfd, 0x5b, 0x60, 0x01, 0x33, 0x90, 0x80, 0x60, 0x01, 0x81, 0x54, 0x01, 0x80, 0x82, 0x55, 0x80, 0x91, 0x50, 0x50, 0x90, 0x60, 0x01, 0x82, 0x03, 0x90, 0x60, 0x00, 0x52, 0x60, 0x20, 0x60, 0x00, 0x20, 0x01, 0x60, 0x00, 0x90, 0x91, 0x92, 0x90, 0x91, 0x90, 0x91, 0x61, 0x01, 0x00, 0x0a, 0x81, 0x54, 0x81, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x02, 0x19, 0x16, 0x90, 0x83, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x02, 0x17, 0x90, 0x55, 0x50, 0x50, 0x56, 0x5b, 0x60, 0x01, 0x81, 0x81, 0x54, 0x81, 0x10, 0x15, 0x15, 0x61, 0x04, 0x5a, 0x57, 0xfe, 0x5b, 0x90, 0x60, 0x00, 0x52, 0x60, 0x20, 0x60, 0x00, 0x20, 0x01, 0x60, 0x00, 0x91, 0x50, 0x54, 0x90, 0x61, 0x01, 0x00, 0x0a, 0x90, 0x04, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x81, 0x56, 0x5b, 0x60, 0x00, 0x44, 0x42, 0x60, 0x01, 0x60, 0x40, 0x51, 0x60, 0x20, 0x01, 0x80, 0x84, 0x81, 0x52, 0x60, 0x20, 0x01, 0x83, 0x81, 0x52, 0x60, 0x20, 0x01, 0x82, 0x80, 0x54, 0x80, 0x15, 0x61, 0x05, 0x06, 0x57, 0x60, 0x20, 0x02, 0x82, 0x01, 0x91, 0x90, 0x60, 0x00, 0x52, 0x60, 0x20, 0x60, 0x00, 0x20, 0x90, 0x5b, 0x81, 0x60, 0x00, 0x90, 0x54, 0x90, 0x61, 0x01, 0x00, 0x0a, 0x90, 0x04, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x81, 0x52, 0x60, 0x20, 0x01, 0x90, 0x60, 0x01, 0x01, 0x90, 0x80, 0x83, 0x11, 0x61, 0x04, 0xbc, 0x57, 0x5b, 0x50, 0x50, 0x93, 0x50, 0x50, 0x50, 0x50, 0x60, 0x40, 0x51, 0x60, 0x20, 0x81, 0x83, 0x03, 0x03, 0x81, 0x52, 0x90, 0x60, 0x40, 0x52, 0x60, 0x40, 0x51, 0x80, 0x82, 0x80, 0x51, 0x90, 0x60, 0x20, 0x01, 0x90, 0x80, 0x83, 0x83, 0x5b, 0x60, 0x20, 0x83, 0x10, 0x15, 0x15, 0x61, 0x05, 0x51, 0x57, 0x80, 0x51, 0x82, 0x52, 0x60, 0x20, 0x82, 0x01, 0x91, 0x50, 0x60, 0x20, 0x81, 0x01, 0x90, 0x50, 0x60, 0x20, 0x83, 0x03, 0x92, 0x50, 0x61, 0x05, 0x2c, 0x56, 0x5b, 0x60, 0x01, 0x83, 0x60, 0x20, 0x03, 0x61, 0x01, 0x00, 0x0a, 0x03, 0x80, 0x19, 0x82, 0x51, 0x16, 0x81, 0x84, 0x51, 0x16, 0x80, 0x82, 0x17, 0x85, 0x52, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x90, 0x50, 0x01, 0x91, 0x50, 0x50, 0x60, 0x40, 0x51, 0x80, 0x91, 0x03, 0x90, 0x20, 0x60, 0x01, 0x90, 0x04, 0x90, 0x50, 0x90, 0x56, 0x5b, 0x82, 0x80, 0x54, 0x82, 0x82, 0x55, 0x90, 0x60, 0x00, 0x52, 0x60, 0x20, 0x60, 0x00, 0x20, 0x90, 0x81, 0x01, 0x92, 0x82, 0x15, 0x61, 0x06, 0x00, 0x57, 0x91, 0x60, 0x20, 0x02, 0x82, 0x01, 0x5b, 0x82, 0x81, 0x11, 0x15, 0x61, 0x05, 0xff, 0x57, 0x82, 0x51, 0x82, 0x60, 0x00, 0x61, 0x01, 0x00, 0x0a, 0x81, 0x54, 0x81, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x02, 0x19, 0x16, 0x90, 0x83, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0x02, 0x17, 0x90, 0x55, 0x50, 0x91, 0x60, 0x20, 0x01, 0x91, 0x90, 0x60, 0x01, 0x01, 0x90, 0x61, 0x05, 0xa7, 0x56, 0x5b, 0x5b, 0x50, 0x90, 0x50, 0x61, 0x06, 0x0d, 0x91, 0x90, 0x61, 0x06, 0x11, 0x56, 0x5b, 0x50, 0x90, 0x56, 0x5b, 0x61, 0x06, 0x51, 0x91, 0x90, 0x5b, 0x80, 0x82, 0x11, 0x15, 0x61, 0x06, 0x4d, 0x57, 0x60, 0x00, 0x81, 0x81, 0x61, 0x01, 0x00, 0x0a, 0x81, 0x54, 0x90, 0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x02, 0x19, 0x16, 0x90, 0x55, 0x50, 0x60, 0x01, 0x01, 0x61, 0x06, 0x17, 0x56, 0x5b, 0x50, 0x90, 0x56, 0x5b, 0x90, 0x56, 0x00, 0xa1, 0x65, 0x62, 0x7a, 0x7a, 0x72, 0x30, 0x58, 0x20, 0x91, 0x9b, 0x1f, 0x32, 0x9a, 0xfa, 0x64, 0x06, 0xac, 0x32, 0xcc, 0xc8, 0x12, 0xf6, 0x5d, 0x08, 0xdc, 0xb2, 0x9f, 0x0c, 0x1b, 0x9f, 0xb6, 0x3c, 0xf3, 0x4e, 0x5a, 0xbf, 0xde, 0xf8, 0xda, 0x07, 0x00, 0x29};
  //  machine::context ctx = {true, 0x00};
  //  machine::message msg = {};
  //  machine::chain_adapter adapter = make_chain_adapter();
  //  machine::machine m(ctx, input, msg, adapter);
  //  assert_message( "machine should start running", m.get_state() == machine::machine_state::running );

  //  std::string line;
  //  while (m.is_running())
  //  {
  //    m.step();
  //    while ( std::getline(m.get_logger(), line) )
  //      std::cerr << "\e[36m" << "LOG: " << line << "\e[0m" << std::endl;
  //    std::cout << m.to_json() << std::endl;
  //  }
  //  while ( std::getline(m.get_logger(), line) )
  //    std::cerr << "\e[36m" << "LOG: " << line << "\e[0m" << std::endl;
  //  std::cout << m.to_json() << std::endl;

  //  assert_message( "machine should stop when done executing", m.get_state() == machine::machine_state::stopped );
  //  assert_message( "stack has correct length", m.stack_length() == 0 );
  //}

  test_that("push opcodes / to_big_word work")
  {
    std::vector<machine::word> input = {0x7f, 0x01, 0x01, 0x01, 0x01, 0x01,
      0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
      0x01, 0x01, 0x01, 0x01,
      0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00};
    machine::context ctx = {true, 0x00};
    machine::message msg = {};
    machine::chain_adapter adapter = make_chain_adapter();
    machine::machine m(ctx, input, msg, adapter);
    assert_message( "machine should start running", m.get_state() == machine::machine_state::running );

    std::string line;
    while (m.is_running())
    {
      m.step();
      while ( std::getline(m.get_logger(), line) )
        std::cerr << "\e[36m" << "LOG: " << line << "\e[0m" << std::endl;
      std::cout << m.to_json() << std::endl;
    }
    while ( std::getline(m.get_logger(), line) )
      std::cerr << "\e[36m" << "LOG: " << line << "\e[0m" << std::endl;
    std::cout << m.to_json() << std::endl;

    assert_message( "machine should stop when done executing", m.get_state() == machine::machine_state::stopped );
    assert_message( "stack has correct length", m.stack_length() == 1 );
  }

  return 0;
}
