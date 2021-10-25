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

    sha3_opcode = 0x20,
    address_opcode = 0x30,
    balance_opcode = 0x31,
    origin_opcode = 0x32,
    caller_opcode = 0x33,
    callvalue_opcode = 0x34,
    calldataload_opcode = 0x35,
    calldatasize_opcode = 0x36,
    calldatacopy_opcode = 0x37,
    codesize_opcode = 0x38,
    codecopy_opcode = 0x39,
    energyprice_opcode = 0x3A,
    extcodesize_opcode = 0x3B,
    extcodecopy_opcode = 0x3C,
    returndatasize_opcode = 0x3D,
    returndatacopy_opcode = 0x3E,
    extcodehash_opcode = 0x3F,
    blockhash_opcode = 0x40,
    coinbase_opcode = 0x41,
    timestamp_opcode = 0x42,
    number_opcode = 0x43,
    difficulty_opcode = 0x44,
    energylimit_opcode = 0x45,
    pop_opcode = 0x50,
    mload_opcode = 0x51,
    mstore_opcode = 0x52,
    mstore8_opcode = 0x53,
    sload_opcode = 0x54,
    sstore_opcode = 0x55,
    jump_opcode = 0x56,
    jumpi_opcode = 0x57,
    pc_opcode = 0x58,
    msize_opcode = 0x59,
    energy_opcode = 0x5A,
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
    log0_opcode = 0xA0,
    log1_opcode = 0xA1,
    log2_opcode = 0xA2,
    log3_opcode = 0xA3,
    log4_opcode = 0xA4,

    create_opcode = 0xF0,
    call_opcode = 0xF1,
    callcode_opcode = 0xF2,
    return_opcode = 0xF3,
    delegatecall_opcode = 0xF4,
    create2_opcode = 0xF5,
    staticcall_opcode = 0xFA,
    revert_opcode = 0xFD,
    selfdestruct_opcode = 0xFF,
  };

  enum class machine_state
  {
    stopped,
    running,
    error
  };

  struct log_object
  {
    std::vector<uint8_t> data;
    std::vector<big_word> topics;
  };

  struct message
  {
    uint32_t flags;
    int32_t depth;
    int64_t energy;

    std::string sender;
    std::string destination;

    big_word value;
    size_t input_size;
    std::vector<word> input_data = {};
    size_t code_size;
  };

  // TODO ensure correct data types
  struct context
  {
    bool is_debug;
    uint64_t block_timestamp;
    uint64_t block_number;
    uint64_t block_difficulty;
    uint64_t block_energylimit;
    int64_t tx_energyprice;
    std::string tx_origin;
    std::string block_coinbase;
  };

  struct chain_adapter
  {
    // TODO sha3 opcode
    std::function< std::string(std::vector<word>) > sha3;

    std::function< uint64_t(std::string) > get_balance;

    // TODO for hashing address -- extcodehash opcode
    std::function< std::string(std::string) > get_code_hash;

    // TODO for hashing block number -- blockhash opcode
    std::function< machine::big_word(uint64_t) > get_block_hash;

    // TODO get contract bytecode at address
    std::function< std::vector<word>(std::string) > get_code_at_addr;

    // TODO creates a child contract -- create opcode
    std::function< std::string(std::vector<word>, big_word) > contract_create;

    // TODO call a method from another contract -- call opcode -- address, energy, value, args
    std::function< std::vector<word>(std::string, uint64_t, big_word, std::vector<word>) > contract_call;

    // TODO call a method from another contract(?) -- callcode opcode -- address, energy, value, args
    std::function< std::vector<word>(std::string, uint64_t, big_word, std::vector<word>) > contract_callcode;

    // TODO call a method from another contract using the storage of the current
    // opcode -- delegatecall opcode -- address, energy, args
    std::function< std::vector<word>(std::string, uint64_t, std::vector<word>) > contract_delegatecall;

    // TODO call a method from another contract with state changes disallowed -- staticcall opcode -- address, energy, args
    std::function< std::vector<word>(std::string, uint64_t, std::vector<word>) > contract_staticcall;

    // TODO creates a child contract -- create2 opcode
    std::function< std::string(std::vector<word>, big_word, std::string) > contract_create2;

    // TODO revert opcode
    std::function< bool(std::vector<word>) > revert;

    // TODO load opcode -- takes key as a parameter and returns value
    std::function< big_word(big_word) > get_storage;

    // TODO sstore opcode -- destination, key, value
    std::function< void(big_word, big_word) > set_storage;

    // TODO return opcode
    std::function< bool(std::vector<word>) > contract_return;

    // TODO selfdestruct opcode
    std::function< bool(std::string) > self_destruct;

    // TODO used to initialize message data
    std::function< std::vector<word>(std::string) > get_input_data;

    std::function< void(const log_object&) > emit_log;
  };

  class machine
  {
    size_t pc = 0;
    std::deque<stack_variant> stack;
    machine_state state = machine_state::running;
    context ctx;
    std::vector<word> code;
    message msg;
    std::map<size_t, word> memory;
    std::map<big_word, big_word> storage;
    std::vector<word> return_value;
    std::vector<word> ext_return_data;
    boost::optional<std::string> error_message;
    std::stringstream logger;
    chain_adapter adapter;
    big_word energy_left;
    opcode current_opcode = stop_opcode;

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
    opcode get_current_opcode() { return current_opcode; };
    std::string to_json();

    void emit_log(const log_object& o);
  };

}
