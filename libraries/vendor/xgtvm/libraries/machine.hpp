#include <cstdint>
#include <vector>
#include <map>
#include <deque>
#include <iostream>
#include <sstream>
#include <boost/optional.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/variant.hpp>
#include <typeinfo>
#include <bitset>

namespace machine
{

typedef uint8_t word;
typedef boost::multiprecision::uint256_t big_word;
typedef boost::multiprecision::int256_t signed_big_word;
typedef boost::variant<big_word, std::string> stack_variant;

enum opcode
{
  // ARITHMETIC
  stop_opcode = 0x00,
  add_opcode = 0x01,
  mul_opcode = 0x02,
  sub_opcode = 0x03,
  div_opcode = 0x04,
  sdiv_opcode = 0x05,
  mod_opcode = 0x06,
  smod_opcode = 0x07,
  addmod_opcode = 0x08,
  mulmod_opcode = 0x09,
  exp_opcode = 0x0a,

  signextend_opcode = 0x0b,

  // COMPARISON
  lt_opcode = 0x10,
  gt_opcode = 0x11,
  slt_opcode = 0x12,
  sgt_opcode = 0x13,
  eq_opcode = 0x14,
  iszero_opcode = 0x15,

  // BITWISE
  and_opcode = 0x16,
  or_opcode = 0x17,
  xor_opcode = 0x18,
  not_opcode = 0x19,
  byte_opcode = 0x1A,
  shl_opcode = 0x1B,
  shr_opcode = 0x1C,
  sar_opcode = 0x1D,

  sha3_opcode = 0x20, // XXX depends on crypto++ -- needs cmake integration
  address_opcode = 0x30,
  balance_opcode = 0x31, // XXX Look up balance for address on top of stack. Is this an rpc call to the chain?
  origin_opcode = 0x32, // XXX Transaction origin address
  caller_opcode = 0x33,
  callvalue_opcode = 0x34,
  calldataload_opcode = 0x35, // XXX Reads a uint256 from message data
  calldatasize_opcode = 0x36, // XXX Message data length in bytes
  calldatacopy_opcode = 0x37, // XXX Copy message data to memory
  codesize_opcode = 0x38,
  codecopy_opcode = 0x39, // XXX Copy executing contract's bytecode to memory
  gasprice_opcode = 0x3A, // XXX Price of executing contract. Energyprice? 
  extcodesize_opcode = 0x3B, // XXX Length of the contract bytecode at addr (top of stack) in bytes
  extcodecopy_opcode = 0x3C, // XXX Copy contract's code to memory
  returndatasize_opcode = 0x3D, // XXX Size of returned data from last external call in bytes
  returndatacopy_opcode = 0x3E, // XXX Copy returned data to memory
  extcodehash_opcode = 0x3F, // XXX Hash of contract bytecode at addr (top of stack)
  blockhash_opcode = 0x40, // XXX Hash of specific block (blocknumber is top of stack)
  coinbase_opcode = 0x41, // TODO REVIEW
  timestamp_opcode = 0x42,
  number_opcode = 0x43,
  difficulty_opcode = 0x44,
  gaslimit_opcode = 0x45,
  pop_opcode = 0x50,
  mload_opcode = 0x51, // TODO
  mstore_opcode = 0x52, // TODO
  mstore8_opcode = 0x53, // TODO
  sload_opcode = 0x54, // TODO
  sstore_opcode = 0x55, // TODO
  jump_opcode = 0x56,
  jumpi_opcode = 0x57, 
  pc_opcode = 0x58,
  msize_opcode = 0x59, // TODO
  gas_opcode = 0x5A, // TODO energy?
  jumpdest_opcode = 0x5B,

  // PUSH
  push1_opcode = 0x60,
  push2_opcode = 0x61,
  push3_opcode = 0x62,
  push4_opcode = 0x63,
  push5_opcode = 0x64,
  push6_opcode = 0x65,
  push7_opcode = 0x66,
  push8_opcode = 0x67,
  push9_opcode = 0x68,
  push10_opcode = 0x69,
  push11_opcode = 0x6A,
  push12_opcode = 0x6B,
  push13_opcode = 0x6C,
  push14_opcode = 0x6D,
  push15_opcode = 0x6E,
  push16_opcode = 0x6F,
  push17_opcode = 0x70,
  push18_opcode = 0x71,
  push19_opcode = 0x72,
  push20_opcode = 0x73,
  push21_opcode = 0x74,
  push22_opcode = 0x75,
  push23_opcode = 0x76,
  push24_opcode = 0x77,
  push25_opcode = 0x78,
  push26_opcode = 0x79,
  push27_opcode = 0x7A,
  push28_opcode = 0x7B,
  push29_opcode = 0x7C,
  push30_opcode = 0x7D,
  push31_opcode = 0x7E,
  push32_opcode = 0x7F,

  // DUP
  dup1_opcode = 0x80,
  dup2_opcode = 0x81,
  dup3_opcode = 0x82,
  dup4_opcode = 0x83,
  dup5_opcode = 0x84,
  dup6_opcode = 0x85,
  dup7_opcode = 0x86,
  dup8_opcode = 0x87,
  dup9_opcode = 0x88,
  dup10_opcode = 0x89,
  dup11_opcode = 0x8A,
  dup12_opcode = 0x8B,
  dup13_opcode = 0x8C,
  dup14_opcode = 0x8D,
  dup15_opcode = 0x8E,
  dup16_opcode = 0x8F,

  // SWAP
  swap1_opcode = 0x90,
  swap2_opcode = 0x91,
  swap3_opcode = 0x92,
  swap4_opcode = 0x93,
  swap5_opcode = 0x94,
  swap6_opcode = 0x95,
  swap7_opcode = 0x96,
  swap8_opcode = 0x97,
  swap9_opcode = 0x98,
  swap10_opcode = 0x99,
  swap11_opcode = 0x9A,
  swap12_opcode = 0x9B,
  swap13_opcode = 0x9C,
  swap14_opcode = 0x9D,
  swap15_opcode = 0x9E,
  swap16_opcode = 0x9F,

  // LOG
  log0_opcode = 0xA0, // TODO
  log1_opcode = 0xA1, // TODO
  log2_opcode = 0xA2, // TODO
  log3_opcode = 0xA3, // TODO
  log4_opcode = 0xA4, // TODO

  // Generic push/dup/swap opcodes
  //push_opcode = 0xB0, // TODO
  //dup_opcode = 0xB1, // TODO
  //swap_opcode = 0xB2, // TODO

  //create_opcode = 0xF0, // TODO
  //call_opcode = 0xF1, // TODO
  //callcode_opcode = 0xF2, // TODO
  return_opcode = 0xF3,
  //delegatecall_opcode = 0xF4, // TODO
  //create2_opcode = 0xF5, // TODO
  //staticcall_opcode = 0xFA, // TODO
  //revert_opcode = 0xFD, // TODO
  //selfdestruct_opcode = 0xFF, // TODO
};

enum class machine_state
{
  stopped,
  running,
  error
};

struct log_object
{
  uint8_t data;
  uint8_t data_size;
  std::vector<big_word> topics;
};


struct message
{
  uint32_t flags;
  int32_t depth;
  int64_t gas;

  // TODO Need address datatype
  // address sender;
  big_word sender;
  // address destination;
  big_word destination;

  big_word value;
  size_t input_size;
  const uint8_t* input_data;
  size_t code_size;
};

struct context
{
  bool is_debug;
  uint64_t block_timestamp;
  uint64_t block_number;
  uint64_t block_difficulty;
  uint64_t block_gaslimit;
  uint64_t tx_gasprice;
  // Should be address datatype
  uint64_t block_coinbase;
};

/* TODO switch stack to variant implementing this pattern

   if (stack_variant* it = std::get_if<stack_variant>(from))
   {
     stack_variant& stack_object = std::get<stack_variant>(from);
    // Use `stack_object` as normal
   }
 */

struct chain_adapter
{
  std::function< uint64_t(std::string) > get_balance;
};

class machine
{
  size_t pc = 0;
  std::deque<stack_variant> stack;
  machine_state state = machine_state::running;
  context ctx;
  std::vector<word> code;
  message msg;
  std::vector<word> memory;
  std::vector<word> return_value;
  boost::optional<std::string> error_message;
  std::stringstream logger;
  chain_adapter adapter;

  void push_word(stack_variant v);
  big_word pop_word();
  void log(std::string output);

  public:
  machine(context ctx, std::vector<word> code, message msg, chain_adapter adapter)
    : ctx(ctx), code(code), msg(msg), adapter(adapter)
  {
  }

  big_word peek_word();
  void push_string(std::string s);
  void print_stack();
  size_t stack_length();
  void step();
  bool is_running();
  machine_state get_state();
  boost::optional<std::string> get_error_message();
  std::stringstream& get_logger();
  std::string to_json();

  void emit_log(const log_object& o);
};

}
