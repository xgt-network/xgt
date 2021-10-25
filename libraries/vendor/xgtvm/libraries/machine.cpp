#include "machine.hpp"

namespace machine
{

  // void emit_log(const evmc::address& _addr, const uint8_t* _data, size_t _dataSize,
  // const evmc::bytes32 _topics[], size_t _numTopics);

  boost::multiprecision::uint256_t alias_to_uint256_t(boost::multiprecision::int256_t i)
  {
    return (boost::multiprecision::uint256_t)i;
  }

  boost::multiprecision::int256_t alias_to_int256_t(boost::multiprecision::uint256_t u)
  {
    return (boost::multiprecision::int256_t)u;
  }

  uint8_t get_byte(boost::multiprecision::uint256_t x, int n)
  {
    std::vector<unsigned char> bytes;
    boost::multiprecision::export_bits(x, std::back_inserter(bytes), 8);
    return bytes[n];
  }

  void set_byte(boost::multiprecision::uint256_t& x, int n, int v)
  {
    std::vector<unsigned char> bytes;
    boost::multiprecision::export_bits(x, std::back_inserter(bytes), 8);
    bytes[n] = v;
    boost::multiprecision::import_bits(x, bytes.begin(), bytes.end());
  }

  size_t uint256_t_to_size_t(boost::multiprecision::uint256_t& x)
  {
    return x.convert_to<size_t>();
  }

  big_word to_big_word(int64_t a)
  {
    return a;
  }

  big_word to_big_word(size_t a)
  {
    return a;
  }

  big_word to_big_word(word a)
  {
    return a;
  }

  big_word to_big_word(word a, word b)
  {
    big_word va = 0;
    set_byte(va, 0, a);
    set_byte(va, 1, b);
    return va;
  }

  big_word to_big_word(word a, word b, word c, word d)
  {
    big_word va = 0;
    set_byte(va, 0, a);
    set_byte(va, 1, b);
    set_byte(va, 2, c);
    set_byte(va, 3, d);
    return va;
  }

  big_word to_big_word(word a, word b, word c, word d, word e, word f, word g, word h)
  {
    big_word va = 0;
    set_byte(va, 0, a);
    set_byte(va, 1, b);
    set_byte(va, 2, c);
    set_byte(va, 3, d);
    set_byte(va, 4, e);
    set_byte(va, 5, f);
    set_byte(va, 6, g);
    set_byte(va, 7, h);
    return va;
  }

  big_word to_big_word(word a, word b, word c, word d, word e, word f, word g,
      word h, word i, word j, word k, word l, word m, word n, word o, word p)
  {
    big_word va = 0;
    set_byte(va, 0, a);
    set_byte(va, 1, b);
    set_byte(va, 2, c);
    set_byte(va, 3, d);
    set_byte(va, 4, e);
    set_byte(va, 5, f);
    set_byte(va, 6, g);
    set_byte(va, 7, h);
    set_byte(va, 8, i);
    set_byte(va, 9, j);
    set_byte(va, 10, k);
    set_byte(va, 11, l);
    set_byte(va, 12, m);
    set_byte(va, 13, n);
    set_byte(va, 14, o);
    set_byte(va, 15, p);
    return va;
  }

  big_word to_big_word(word a, word b, word c, word d, word e, word f, word g,
      word h, word i, word j, word k, word l, word m, word n, word o, word p,
      word q, word r, word s, word t, word u, word v, word w, word x, word y,
      word z, word aa, word ab, word ac, word ad, word ae, word af)
  {
    big_word va = 0;
    set_byte(va, 0, a);
    set_byte(va, 1, b);
    set_byte(va, 2, c);
    set_byte(va, 3, d);
    set_byte(va, 4, e);
    set_byte(va, 5, f);
    set_byte(va, 6, g);
    set_byte(va, 7, h);
    set_byte(va, 8, i);
    set_byte(va, 9, j);
    set_byte(va, 10, k);
    set_byte(va, 11, l);
    set_byte(va, 12, m);
    set_byte(va, 13, n);
    set_byte(va, 14, o);
    set_byte(va, 15, p);
    set_byte(va, 16, q);
    set_byte(va, 17, r);
    set_byte(va, 18, s);
    set_byte(va, 19, t);
    set_byte(va, 20, u);
    set_byte(va, 21, v);
    set_byte(va, 22, w);
    set_byte(va, 23, x);
    set_byte(va, 24, y);
    set_byte(va, 25, z);
    set_byte(va, 26, aa);
    set_byte(va, 27, ab);
    set_byte(va, 28, ac);
    set_byte(va, 29, ad);
    set_byte(va, 30, ae);
    set_byte(va, 31, af);
    return va;
  }

  std::string inspect(std::vector<word> words)
  {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < words.size(); i++)
    {
      // TODO: Print hex
      ss << std::to_string(words.at(i));
      if (i != words.size() - 1)
        ss << ", ";
    }
    ss << "]";
    return ss.str();
  }

  std::string inspect(std::map<size_t, word> words)
  {
    std::stringstream ss;
    ss << "[";
    std::map<size_t,word>::iterator it;
    size_t i = 0;
    while(true)
    {
      it = words.find(i);
      if (it != words.end()) {
        // TODO: Print hex
        ss << std::to_string(it->second);
        ss << ", ";
      }
      else {
        ss << std::to_string(it->second);
        break;
      }
      i++;
    }
    ss << "]";
    return ss.str();
  }

  void machine::push_word(stack_variant v)
  {
    stack.push_front(v);
  }

  big_word machine::pop_word()
  {
    stack_variant& stack_object = stack.front();
    if (big_word* it = boost::get<big_word>(&stack_object))
    {
      stack.pop_front();
      return *it;
    }
    throw;
  }

  big_word machine::peek_word()
  {
    stack_variant& stack_object = stack.front();
    if (big_word* it = boost::get<big_word>(&stack_object))
    {
      stack.pop_front();
      return *it;
    }
    else {
      throw;
    }
  }

  void machine::push_string(std::string s)
  {
    stack.push_front(s);
  }

  // TODO: Make better
  // TODO: Return string instead?
  void machine::print_stack()
  {
    for (auto it = stack.cbegin(); it != stack.cend(); ++it)
    {
      stack_variant w = *it;
      logger << w;
      if (it + 1 != stack.cend())
        logger << " ";
    }
    logger << std::endl;
  }

  size_t machine::stack_length()
  {
    return stack.size();
  }

  void machine::step()
  {
    if (code.size() == 0 || pc > code.size())
    {
      logger << "stop" << std::endl;
      state = machine_state::stopped;
      return;
    }
    word current_instruction = code[pc];
    opcode op = (opcode)current_instruction;
    current_opcode = op;
    pc++;

    logger << "step pc " << std::to_string(pc) << " opcode " << std::hex << op << std::dec << std::endl;

    // Allow to skip evaluation by throwing exception.
    // TODO: Verify this behavior.
    word a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x,
         y, z, aa, ab, ac, ad, ae, af;
    big_word va, vb, vc, vd, ve, vf, vg, vh, vi, vj, vk, vl, vm, vn, vo, vp, vq;
    stack_variant sv;
    signed_big_word sa, sb, sc;
    size_t offset, dest_offset, length, code_size = 0;
    std::vector<word> contract_args;
    std::vector<word> retval;
    std::vector<word> ext_contract_code;
    std::vector<word>::const_iterator first, last;
    std::string* ss;
    switch (op)
    {
      case stop_opcode:
        logger << "op stop" << std::endl;
        state = machine_state::stopped;
        break;
      case add_opcode:
        logger << "op add" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = va + vb;
        push_word(vc);
        break;
      case mul_opcode:
        logger << "op mul" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = va * vb;
        push_word(vc);
        break;
      case sub_opcode:
        logger << "op sub" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = va - vb;
        push_word(vc);
        break;
      case div_opcode:
        logger << "op div" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = va / vb;
        push_word(vc);
        break;
      case sdiv_opcode:
        logger << "op sdiv" << std::endl;
        sa = alias_to_int256_t( pop_word() );
        sb = alias_to_int256_t( pop_word() );
        sc = sa / sb;
        push_word( alias_to_uint256_t(sc) );
        break;
      case mod_opcode:
        logger << "op mod" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = va % vb;
        push_word(vc);
        break;
      case smod_opcode:
        logger << "op smod" << std::endl;
        sa = alias_to_int256_t( pop_word() );
        sb = alias_to_int256_t( pop_word() );
        sc = sa % sb;
        push_word( alias_to_uint256_t(sc) );
        break;
      case addmod_opcode:
        logger << "op addmod" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = pop_word();
        vd = va + vb;
        ve = vd % vc;
        push_word(ve);
        break;
      case mulmod_opcode:
        logger << "op addmod" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = pop_word();
        vd = va * vb;
        ve = vd % vc;
        push_word(ve);
        break;
      case exp_opcode:
        // TODO Implement using boost::multiprecision::pow
        logger << "op exp" << std::endl;
        va = pop_word(); // base
        vb = pop_word(); // exponent
        vc = 1;
        for (int i = 0; i < vb; i++) {
          vc *= va;
        }
        push_word(vc);
        break;
      case signextend_opcode:
        logger << "op signextend" << std::endl;
        sa = alias_to_int256_t( pop_word() ); // b
        sb = alias_to_int256_t( pop_word() ); // x

        if (sa < 31)
        {
          unsigned testBit = static_cast<unsigned>(sa) * 8 + 7;
          big_word one = 1;
          big_word mask = ((one << testBit) - 1);
          if (sb & (one << testBit))
            sc = sb | ~mask;
          else
            sc = sb & mask;
        }

        push_word( alias_to_uint256_t(sc) );
        break;
      case lt_opcode:
        logger << "op lt" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = va < vb;
        push_word(vc);
        break;
      case gt_opcode:
        logger << "op gt" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = va > vb;
        push_word(vc);
        break;
      case slt_opcode:
        logger << "op stl" << std::endl;
        sa = alias_to_int256_t( pop_word() );
        sb = alias_to_int256_t( pop_word() );
        sc = sa < sb;
        push_word( alias_to_uint256_t(sc) );
        break;
      case sgt_opcode:
        logger << "op sgt" << std::endl;
        sa = alias_to_int256_t( pop_word() );
        sb = alias_to_int256_t( pop_word() );
        sc = sa > sb;
        push_word( alias_to_uint256_t(sc) );
        break;
      case eq_opcode:
        logger << "op eq" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = va == vb;
        push_word(vc);
        break;
      case iszero_opcode:
        logger << "op iszero" << std::endl;
        va = pop_word();
        vb = va == 0;
        push_word(vb);
        break;
      case and_opcode:
        logger << "op and" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = va & vb;
        push_word(vc);
        break;
      case or_opcode:
        logger << "op or" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = va | vb;
        push_word(vc);
        break;
      case xor_opcode:
        logger << "op xor" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = va ^ vb;
        push_word(vc);
        break;
      case not_opcode:
        logger << "op not" << std::endl;
        va = pop_word();
        vb = ~va;
        push_word(vb);
        break;
      case byte_opcode:
        logger << "op byte" << std::endl;

        sa = pop_word(); // index
        sb = pop_word(); // num

        sc = sa < 32 ? (sb >> (unsigned)(8 * (31 - sa))) & 0xff : 0;

        push_word( alias_to_uint256_t(sc) );
        break;
      case shl_opcode:
        logger << "op shl" << std::endl;

        va = pop_word(); // shift
        vb = pop_word(); // value

        vc = vb << va.convert_to<size_t>();

        push_word(vc);
        break;
      case shr_opcode:
        logger << "op shr" << std::endl;
        va = pop_word(); // shift
        vb = pop_word(); // value

        vc = vb >> va.convert_to<size_t>();
        push_word(vc);

        break;
      case sar_opcode:
        logger << "op sar" << std::endl;
        sa = alias_to_int256_t( pop_word() ); // shift
        sb = pop_word(); // value

        sc = sb >> sa.convert_to<size_t>();
        push_word( alias_to_uint256_t(sc) );

        break;
      case sha3_opcode:
        logger << "op sha3" << std::endl;
        va = pop_word(); // offset
        vb = pop_word(); // length

        for (size_t i = static_cast<size_t>(va); i < static_cast<size_t>(vb); i++) {
          std::map<size_t,word>::iterator it;
          it = memory.find(i);
          if (it != memory.end()) {
            retval.push_back(it->second);
          }
          else {
            retval.push_back(word(0));
          }
        }

        push_word( adapter.sha3( retval ) ); // hash

        break;
      case address_opcode:
        logger << "op address" << std::endl;
        push_word( msg.destination );
        break;
      case balance_opcode:
        logger << "op balance" << std::endl;
        {
          sv = stack.front(); // addr
          stack.pop_front();
          ss = boost::get<std::string>(&sv);
          if (ss)
          {
            push_word( adapter.get_balance(*ss) );
          }
          else
          {
            state = machine_state::error;
            error_message.emplace("Balance operation type error");
          }
        }
        break;
      case origin_opcode:
        logger << "op origin" << std::endl;
        push_word( ctx.tx_origin );
        break;
      case caller_opcode:
        logger << "op caller" << std::endl;
        push_word( msg.sender );
        break;
      case callvalue_opcode:
        logger << "op callvalue" << std::endl;
        push_word( msg.value );
        break;
      case calldataload_opcode:
        logger << "op calldataload" << std::endl;
        offset = static_cast<size_t>( pop_word() );

        if (offset > msg.input_size) {
          logger << "calldataload start index is larger than message input_size";
          break;
        }

        va = to_big_word(
            msg.input_data[offset + 7],
            msg.input_data[offset + 6],
            msg.input_data[offset + 5],
            msg.input_data[offset + 4],
            msg.input_data[offset + 3],
            msg.input_data[offset + 2],
            msg.input_data[offset + 1],
            msg.input_data[offset + 0]
            );
        push_word(va);
        break;
      case calldatasize_opcode:
        logger << "op calldatasize" << std::endl;
        push_word( to_big_word(msg.input_size) );
        break;
      case calldatacopy_opcode:
        logger << "op calldatacopy" << std::endl;

        dest_offset = static_cast<size_t>( pop_word() );
        offset = static_cast<size_t>( pop_word() );
        length = static_cast<size_t>( pop_word() );

        if ((offset + length) > msg.input_size) {
          logger << "calldatacopy end index is larger than message input_size";
          break;
        }

        for (size_t i = 0; i < length; ++i)
          memory[dest_offset + i] = msg.input_data[offset + i];

        break;
      case codesize_opcode:
        logger << "op codesize" << std::endl;
        push_word( to_big_word( code.size() ) );
        break;
      case codecopy_opcode:
        logger << "op codecopy" << std::endl;

        dest_offset = static_cast<size_t>( pop_word() );
        offset = static_cast<size_t>( pop_word() );
        length = static_cast<size_t>( pop_word() );

        if ((offset + length) > msg.input_size) {
          logger << "Codecopy end index is larger than message input_size";
          break;
        }

        for (size_t i = 0; i < length; ++i)
          memory[dest_offset + i] = code[offset + i];

        break;
      case energyprice_opcode:
        logger << "op energyprice" << std::endl;
        push_word( to_big_word( ctx.tx_energyprice ) );
        break;
      case extcodesize_opcode:
        logger << "op extcodesize" << std::endl;
        sv = stack.front(); // addr
        ss = boost::get<std::string>(&sv);
        if (ss)
        {
          stack.pop_front();
          ext_contract_code = adapter.get_code_at_addr(*ss);
          push_word( sizeof(ext_contract_code) / sizeof(ext_contract_code[0]) );
        }
        else
        {
          state = machine_state::error;
          error_message.emplace("Extcodesize operation type error");
        }
        break;
      case extcodecopy_opcode:
        logger << "op extcodecopy" << std::endl;
        sv = stack.front(); // addr
        ss = boost::get<std::string>(&sv);
        if (ss)
        {
          stack.pop_front();
          ext_contract_code = adapter.get_code_at_addr(*ss);
          code_size = sizeof(ext_contract_code) / sizeof(ext_contract_code[0]);
          dest_offset = static_cast<size_t>( pop_word() );
          offset = static_cast<size_t>( pop_word() );
          length = static_cast<size_t>( pop_word() );

          if ((offset + length) > code_size) {
            logger << "codecopy end index exceeds external contract code length";
            break;
          }

          for (size_t i = 0; i < length; ++i)
            memory[dest_offset + i] = ext_contract_code[offset + i];
        }
        else
        {
          state = machine_state::error;
          error_message.emplace("Extcodecopy operation type error");
        }

        break;
      case returndatasize_opcode:
        logger << "op returndatasize" << std::endl;
        push_word( ext_return_data.size() );
        break;
      case returndatacopy_opcode:
        logger << "op returndatacopy" << std::endl;
        dest_offset = static_cast<size_t>( pop_word() );
        offset = static_cast<size_t>( pop_word() );
        length = static_cast<size_t>( pop_word() );

        if ( (offset + length) > ext_return_data.size() ) {
          logger << "returndatacopy end index exceeds return data size";
          break;
        }

        for (size_t i = 0; i < length; ++i)
          memory[dest_offset + i] = ext_return_data[offset + i];

        break;
      case extcodehash_opcode:
        logger << "op extcodehash" << std::endl;
        sv = stack.front(); // addr
        ss = boost::get<std::string>(&sv);
        if (ss)
        {
          stack.pop_front();
          push_word( adapter.get_code_hash(*ss) );
        }
        else
        {
          state = machine_state::error;
          error_message.emplace("Extcodehash operation type error");
        }
        break;
      case blockhash_opcode:
        logger << "op blockhash" << std::endl;
        va = pop_word();
        push_word( adapter.get_block_hash( static_cast<uint64_t>(va) ) );
        break;
      case coinbase_opcode:
        logger << "op coinbase" << std::endl;
        push_word( ctx.block_coinbase );
        break;
      case timestamp_opcode:
        logger << "op timestamp" << std::endl;
        push_word( ctx.block_timestamp );
        break;
      case number_opcode:
        logger << "op number" << std::endl;
        push_word( ctx.block_number );
        break;
      case difficulty_opcode:
        logger << "op difficulty" << std::endl;
        push_word( ctx.block_difficulty );
        break;
      case energylimit_opcode:
        logger << "op energylimit" << std::endl;
        push_word( ctx.block_energylimit );
        break;
      case pop_opcode:
        logger << "op pop" << std::endl;
        pop_word();
        break;
      case mload_opcode:
        logger << "op mload" << std::endl;
        va = pop_word(); // offset
        // TODO: Verify order
        vb = to_big_word(
            memory[static_cast<size_t>(va) + 7],
            memory[static_cast<size_t>(va) + 6],
            memory[static_cast<size_t>(va) + 5],
            memory[static_cast<size_t>(va) + 4],
            memory[static_cast<size_t>(va) + 3],
            memory[static_cast<size_t>(va) + 2],
            memory[static_cast<size_t>(va) + 1],
            memory[static_cast<size_t>(va) + 0]
            );
        push_word(vb);
        break;
      case mstore_opcode:
        logger << "op mstore" << std::endl;
        va = pop_word(); // offset
        vb = pop_word(); // value
        logger << "memory before: " << inspect(memory) << std::endl;

        // TODO: Verify order
        memory[static_cast<size_t>(va) + 0] = get_byte(vb, 7);
        memory[static_cast<size_t>(va) + 1] = get_byte(vb, 6);
        memory[static_cast<size_t>(va) + 2] = get_byte(vb, 5);
        memory[static_cast<size_t>(va) + 3] = get_byte(vb, 4);
        memory[static_cast<size_t>(va) + 4] = get_byte(vb, 3);
        memory[static_cast<size_t>(va) + 5] = get_byte(vb, 2);
        memory[static_cast<size_t>(va) + 6] = get_byte(vb, 1);
        memory[static_cast<size_t>(va) + 7] = get_byte(vb, 0);
        logger << "memory after: " << inspect(memory) << std::endl;
        break;
      case mstore8_opcode:
        logger << "op mstore8" << std::endl;
        va = pop_word(); // offset
        vb = pop_word(); // value
        logger << "memory before: " << inspect(memory) << std::endl;
        // TODO: Verify order
        memory[static_cast<size_t>(va)] = get_byte(vb, 0); // TODO verify byte of big_word vb
        logger << "memory after: " << inspect(memory) << std::endl;
        break;
      case sload_opcode:
        logger << "op sload" << std::endl;
        sv = stack.front();
        stack.pop_front();
        if (big_word* it = boost::get<big_word>(&sv))
        {
          va = adapter.get_storage(*it);
        }
        else {
          throw; // TODO
        }
        push_word(va);
        break;
      case sstore_opcode:
        logger << "op sstore" << std::endl;
        sv = stack.front();
        stack.pop_front();
        if (big_word* it = boost::get<big_word>(&sv))
        {
          va = *it;
        }
        else {
          throw; // TODO
        }

        sv = stack.front();
        stack.pop_front();
        if (big_word* it = boost::get<big_word>(&sv))
        {
          vb = *it;
        }
        else {
          throw; // TODO
        }
        adapter.set_storage(va, vb);
        break;
      case jump_opcode:
        logger << "op jump" << std::endl;
        va = pop_word(); // destination
        if (code[get_byte(va, 0)] == jumpdest_opcode)
          pc = get_byte(va, 0);
        break;
      case jumpi_opcode:
        logger << "op jumpi" << std::endl;
        print_stack();
        va = pop_word();
        vb = pop_word();
        if (vb != 0)
          if (code[get_byte(va, 0)] == jumpdest_opcode)
            pc = get_byte(va, 0);
        break;
      case pc_opcode:
        logger << "op pc" << std::endl;
        va = pc;
        push_word(va);
        break;
      case msize_opcode:
        logger << "op msize" << std::endl;
        push_word( to_big_word( memory.size() ) );
        break;
      case energy_opcode:
        logger << "op energy" << std::endl;
        push_word(energy_left);
        break;
      case jumpdest_opcode:
        logger << "op jumpdest" << std::endl;
        break;
      case push1_opcode:
        logger << "op push1" << std::endl;
        a = code[pc];
        pc++;
        push_word(a);
        break;
      case push2_opcode:
        logger << "op push2" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        va = to_big_word(0, 0, a, b); // TODO: Verify
        push_word(va);
        break;
      case push3_opcode:
        logger << "op push3" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        va = to_big_word(0, a, b, c); // TODO: Verify
        push_word(va);
        break;
      case push4_opcode:
        logger << "op push4" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        va = to_big_word(a, b, c, d); // TODO: Verify
        push_word(va);
        break;
      case push5_opcode:
        logger << "op push5" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, a, b, c, d, e); // TODO: Verify
        push_word(va);
        break;
      case push6_opcode:
        logger << "op push6" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        va = to_big_word(0, 0, a, b, c, d, e, f); // TODO: Verify
        push_word(va);
        break;
      case push7_opcode:
        logger << "op push7" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        va = to_big_word(0, a, b, c, d, e, f, g); // TODO: Verify
        push_word(va);
        break;
      case push8_opcode:
        logger << "op push8" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        va = to_big_word(a, b, c, d, e, f, g, h); // TODO: Verify
        push_word(va);
        break;
      case push9_opcode:
        logger << "op push9" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, 0, 0, 0, 0, a, b, c, d, e, f, g, h, i); // TODO: Verify
        push_word(va);
        break;
      case push10_opcode:
        logger << "op push10" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, 0, 0, 0, a, b, c, d, e, f, g, h, i, j); // TODO: Verify
        push_word(va);
        break;
      case push11_opcode:
        logger << "op push11" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, 0, 0, a, b, c, d, e, f, g, h, i, j, k); // TODO: Verify
        push_word(va);
        break;
      case push12_opcode:
        logger << "op push12" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, 0, a, b, c, d, e, f, g, h, i, j, k, l); // TODO: Verify
        push_word(va);
        break;
      case push13_opcode:
        logger << "op push13" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, a, b, c, d, e, f, g, h, i, j, k, l, m); // TODO: Verify
        push_word(va);
        break;
      case push14_opcode:
        logger << "op push14" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        va = to_big_word(0, 0, a, b, c, d, e, f, g, h, i, j, k, l, m, n); // TODO: Verify
        push_word(va);
        break;
      case push15_opcode:
        logger << "op push15" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        o = code[pc];
        pc++;
        va = to_big_word(0, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o); // TODO: Verify
        push_word(va);
        break;
      case push16_opcode:
        logger << "op push16" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        o = code[pc];
        pc++;
        p = code[pc];
        pc++;
        va = to_big_word(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p); // TODO: Verify
        push_word(va);
        break;
      case push17_opcode:
        logger << "op push17" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        o = code[pc];
        pc++;
        p = code[pc];
        pc++;
        q = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, a, b, c, d,
            e, f, g, h, i, j, k, l, m, n, o, p, q); // TODO: Verify
        push_word(va);
        break;
      case push18_opcode:
        logger << "op push18" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        o = code[pc];
        pc++;
        p = code[pc];
        pc++;
        q = code[pc];
        pc++;
        r = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, a, b, c, d, e,
            f, g, h, i, j, k, l, m, n, o, p, q, r); // TODO: Verify
        push_word(va);
        break;
      case push19_opcode:
        logger << "op push19" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        o = code[pc];
        pc++;
        p = code[pc];
        pc++;
        q = code[pc];
        pc++;
        r = code[pc];
        pc++;
        s = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, a, b, c, d, e, f,
            g, h, i, j, k, l, m, n, o, p, q, r, s); // TODO: Verify
        push_word(va);
        break;
      case push20_opcode:
        logger << "op push20" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        o = code[pc];
        pc++;
        p = code[pc];
        pc++;
        q = code[pc];
        pc++;
        r = code[pc];
        pc++;
        s = code[pc];
        pc++;
        t = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, a, b, c, d, e, f, g,
            h, i, j, k, l, m, n, o, p, q, r, s, t); // TODO: Verify
        push_word(va);
        break;
      case push21_opcode:
        logger << "op push21" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        o = code[pc];
        pc++;
        p = code[pc];
        pc++;
        q = code[pc];
        pc++;
        r = code[pc];
        pc++;
        s = code[pc];
        pc++;
        t = code[pc];
        pc++;
        u = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, a, b, c, d, e, f, g, h,
            i, j, k, l, m, n, o, p, q, r, s, t, u); // TODO: Verify
        push_word(va);
        break;
      case push22_opcode:
        logger << "op push22" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        o = code[pc];
        pc++;
        p = code[pc];
        pc++;
        q = code[pc];
        pc++;
        r = code[pc];
        pc++;
        s = code[pc];
        pc++;
        t = code[pc];
        pc++;
        u = code[pc];
        pc++;
        v = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, a, b, c, d, e, f, g, h, i,
            j, k, l, m, n, o, p, q, r, s, t, u, v); // TODO: Verify
        push_word(va);
        break;
      case push23_opcode:
        logger << "op push23" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        o = code[pc];
        pc++;
        p = code[pc];
        pc++;
        q = code[pc];
        pc++;
        r = code[pc];
        pc++;
        s = code[pc];
        pc++;
        t = code[pc];
        pc++;
        u = code[pc];
        pc++;
        v = code[pc];
        pc++;
        w = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, 0, 0, 0, 0, 0, 0, a, b, c, d, e, f, g, h, i, j,
            k, l, m, n, o, p, q, r, s, t, u, v, w); // TODO: Verify
        push_word(va);
        break;
      case push24_opcode:
        logger << "op push24" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        o = code[pc];
        pc++;
        p = code[pc];
        pc++;
        q = code[pc];
        pc++;
        r = code[pc];
        pc++;
        s = code[pc];
        pc++;
        t = code[pc];
        pc++;
        u = code[pc];
        pc++;
        v = code[pc];
        pc++;
        w = code[pc];
        pc++;
        x = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, 0, 0, 0, 0, 0, a, b, c, d, e, f, g, h, i, j, k,
            l, m, n, o, p, q, r, s, t, u, v, w, x); // TODO: Verify
        push_word(va);
        break;
      case push25_opcode:
        logger << "op push25" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        o = code[pc];
        pc++;
        p = code[pc];
        pc++;
        q = code[pc];
        pc++;
        r = code[pc];
        pc++;
        s = code[pc];
        pc++;
        t = code[pc];
        pc++;
        u = code[pc];
        pc++;
        v = code[pc];
        pc++;
        w = code[pc];
        pc++;
        x = code[pc];
        pc++;
        y = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, 0, 0, 0, 0, a, b, c, d, e, f, g, h, i, j, k, l,
            m, n, o, p, q, r, s, t, u, v, w, x, y); // TODO: Verify
        push_word(va);
        break;
      case push26_opcode:
        logger << "op push26" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        o = code[pc];
        pc++;
        p = code[pc];
        pc++;
        q = code[pc];
        pc++;
        r = code[pc];
        pc++;
        s = code[pc];
        pc++;
        t = code[pc];
        pc++;
        u = code[pc];
        pc++;
        v = code[pc];
        pc++;
        w = code[pc];
        pc++;
        x = code[pc];
        pc++;
        y = code[pc];
        pc++;
        z = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, 0, 0, 0, a, b, c, d, e, f, g, h, i, j, k, l, m,
            n, o, p, q, r, s, t, u, v, w, x, y, z); // TODO: Verify
        push_word(va);
        break;
      case push27_opcode:
        logger << "op push27" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        o = code[pc];
        pc++;
        p = code[pc];
        pc++;
        q = code[pc];
        pc++;
        r = code[pc];
        pc++;
        s = code[pc];
        pc++;
        t = code[pc];
        pc++;
        u = code[pc];
        pc++;
        v = code[pc];
        pc++;
        w = code[pc];
        pc++;
        x = code[pc];
        pc++;
        y = code[pc];
        pc++;
        z = code[pc];
        pc++;
        aa = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, 0, 0, a, b, c, d, e, f, g, h, i, j, k, l, m, n,
            o, p, q, r, s, t, u, v, w, x, y, z, aa); // TODO: Verify
        push_word(va);
        break;
      case push28_opcode:
        logger << "op push28" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        o = code[pc];
        pc++;
        p = code[pc];
        pc++;
        q = code[pc];
        pc++;
        r = code[pc];
        pc++;
        s = code[pc];
        pc++;
        t = code[pc];
        pc++;
        u = code[pc];
        pc++;
        v = code[pc];
        pc++;
        w = code[pc];
        pc++;
        x = code[pc];
        pc++;
        y = code[pc];
        pc++;
        z = code[pc];
        pc++;
        aa = code[pc];
        pc++;
        ab = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, 0, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o,
            p, q, r, s, t, u, v, w, x, y, z, aa, ab); // TODO: Verify
        push_word(va);
        break;
      case push29_opcode:
        logger << "op push29" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        o = code[pc];
        pc++;
        p = code[pc];
        pc++;
        q = code[pc];
        pc++;
        r = code[pc];
        pc++;
        s = code[pc];
        pc++;
        t = code[pc];
        pc++;
        u = code[pc];
        pc++;
        v = code[pc];
        pc++;
        w = code[pc];
        pc++;
        x = code[pc];
        pc++;
        y = code[pc];
        pc++;
        z = code[pc];
        pc++;
        aa = code[pc];
        pc++;
        ab = code[pc];
        pc++;
        ac = code[pc];
        pc++;
        va = to_big_word(0, 0, 0, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p,
            q, r, s, t, u, v, w, x, y, z, aa, ab, ac); // TODO: Verify
        push_word(va);
        break;
      case push30_opcode:
        logger << "op push30" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        o = code[pc];
        pc++;
        p = code[pc];
        pc++;
        q = code[pc];
        pc++;
        r = code[pc];
        pc++;
        s = code[pc];
        pc++;
        t = code[pc];
        pc++;
        u = code[pc];
        pc++;
        v = code[pc];
        pc++;
        w = code[pc];
        pc++;
        x = code[pc];
        pc++;
        y = code[pc];
        pc++;
        z = code[pc];
        pc++;
        aa = code[pc];
        pc++;
        ab = code[pc];
        pc++;
        ac = code[pc];
        pc++;
        ad = code[pc];
        pc++;
        va = to_big_word(0, 0, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p,
            q, r, s, t, u, v, w, x, y, z, aa, ab, ac, ad); // TODO: Verify
        push_word(va);
        break;
      case push31_opcode:
        logger << "op push31" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        o = code[pc];
        pc++;
        p = code[pc];
        pc++;
        q = code[pc];
        pc++;
        r = code[pc];
        pc++;
        s = code[pc];
        pc++;
        t = code[pc];
        pc++;
        u = code[pc];
        pc++;
        v = code[pc];
        pc++;
        w = code[pc];
        pc++;
        x = code[pc];
        pc++;
        y = code[pc];
        pc++;
        z = code[pc];
        pc++;
        aa = code[pc];
        pc++;
        ab = code[pc];
        pc++;
        ac = code[pc];
        pc++;
        ad = code[pc];
        pc++;
        ae = code[pc];
        pc++;
        va = to_big_word(0, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p,
            q, r, s, t, u, v, w, x, y, z, aa, ab, ac, ad, ae); // TODO: Verify
        push_word(va);
        break;
      case push32_opcode:
        logger << "op push32" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        c = code[pc];
        pc++;
        d = code[pc];
        pc++;
        e = code[pc];
        pc++;
        f = code[pc];
        pc++;
        g = code[pc];
        pc++;
        h = code[pc];
        pc++;
        i = code[pc];
        pc++;
        j = code[pc];
        pc++;
        k = code[pc];
        pc++;
        l = code[pc];
        pc++;
        m = code[pc];
        pc++;
        n = code[pc];
        pc++;
        o = code[pc];
        pc++;
        p = code[pc];
        pc++;
        q = code[pc];
        pc++;
        r = code[pc];
        pc++;
        s = code[pc];
        pc++;
        t = code[pc];
        pc++;
        u = code[pc];
        pc++;
        v = code[pc];
        pc++;
        w = code[pc];
        pc++;
        x = code[pc];
        pc++;
        y = code[pc];
        pc++;
        z = code[pc];
        pc++;
        aa = code[pc];
        pc++;
        ab = code[pc];
        pc++;
        ac = code[pc];
        pc++;
        ad = code[pc];
        pc++;
        ae = code[pc];
        pc++;
        af = code[pc];
        pc++;
        va = to_big_word(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p,
            q, r, s, t, u, v, w, x, y, z, aa, ab, ac, ad, ae, af); // TODO: Verify
        push_word(va);
        break;
      case dup1_opcode:
        logger << "op dup1" << std::endl;
        sv = stack.front();
        push_word(sv);
        break;
      case dup2_opcode:
        logger << "op dup2" << std::endl;
        sv  = stack.at(1);
        push_word(va);
        break;
      case dup3_opcode:
        logger << "op dup3" << std::endl;
        sv = stack.at(2);
        push_word(sv);
        break;
      case dup4_opcode:
        logger << "op dup4" << std::endl;
        sv = stack.at(3);
        push_word(sv);
        break;
      case dup5_opcode:
        logger << "op dup5" << std::endl;
        sv = stack.at(4);
        push_word(sv);
        break;
      case dup6_opcode:
        logger << "op dup6" << std::endl;
        sv = stack.at(5);
        push_word(sv);
        break;
      case dup7_opcode:
        logger << "op dup7" << std::endl;
        sv = stack.at(6);
        push_word(sv);
        break;
      case dup8_opcode:
        logger << "op dup8" << std::endl;
        sv = stack.at(7);
        push_word(sv);
        break;
      case dup9_opcode:
        logger << "op dup9" << std::endl;
        sv = stack.at(8);
        push_word(sv);
        break;
      case dup10_opcode:
        logger << "op dup10" << std::endl;
        sv = stack.at(9);
        push_word(sv);
        break;
      case dup11_opcode:
        logger << "op dup11" << std::endl;
        sv = stack.at(10);
        push_word(sv);
        break;
      case dup12_opcode:
        logger << "op dup12" << std::endl;
        sv = stack.at(11);
        push_word(sv);
        break;
      case dup13_opcode:
        logger << "op dup13" << std::endl;
        sv = stack.at(12);
        push_word(sv);
        break;
      case dup14_opcode:
        logger << "op dup14" << std::endl;
        sv = stack.at(13);
        push_word(sv);
        break;
      case dup15_opcode:
        logger << "op dup15" << std::endl;
        sv = stack.at(14);
        push_word(sv);
        break;
      case dup16_opcode:
        logger << "op dup16" << std::endl;
        sv = stack.at(15);
        push_word(sv);
        break;
      case swap1_opcode:
        logger << "op swap1" << std::endl;
        va = pop_word();
        vb = pop_word();
        push_word(a);
        push_word(b);
        break;
      case swap2_opcode:
        logger << "op swap2" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = pop_word();
        push_word(va);
        push_word(vb);
        push_word(vc);
        break;
      case swap3_opcode:
        logger << "op swap3" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = pop_word();
        vd = pop_word();
        push_word(va);
        push_word(vc);
        push_word(vb);
        push_word(vd);
        break;
      case swap4_opcode:
        logger << "op swap4" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = pop_word();
        vd = pop_word();
        ve = pop_word();
        push_word(va);
        push_word(vd);
        push_word(vc);
        push_word(vb);
        push_word(ve);
        break;
      case swap5_opcode:
        logger << "op swap5" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = pop_word();
        vd = pop_word();
        ve = pop_word();
        vf = pop_word();
        push_word(va);
        push_word(ve);
        push_word(vd);
        push_word(vc);
        push_word(vb);
        push_word(vf);
        break;
      case swap6_opcode:
        logger << "op swap6" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = pop_word();
        vd = pop_word();
        ve = pop_word();
        vf = pop_word();
        vg = pop_word();
        push_word(va);
        push_word(vf);
        push_word(ve);
        push_word(vd);
        push_word(vc);
        push_word(vb);
        push_word(vg);
        break;
      case swap7_opcode:
        logger << "op swap7" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = pop_word();
        vd = pop_word();
        ve = pop_word();
        vf = pop_word();
        vg = pop_word();
        vh = pop_word();
        push_word(va);
        push_word(vg);
        push_word(vf);
        push_word(ve);
        push_word(vd);
        push_word(vc);
        push_word(vb);
        push_word(vh);
        break;
      case swap8_opcode:
        logger << "op swap8" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = pop_word();
        vd = pop_word();
        ve = pop_word();
        vf = pop_word();
        vg = pop_word();
        vh = pop_word();
        vi = pop_word();
        push_word(va);
        push_word(vh);
        push_word(vg);
        push_word(vf);
        push_word(ve);
        push_word(vd);
        push_word(vc);
        push_word(vb);
        push_word(vi);
        break;
      case swap9_opcode:
        logger << "op swap9" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = pop_word();
        vd = pop_word();
        ve = pop_word();
        vf = pop_word();
        vg = pop_word();
        vh = pop_word();
        vi = pop_word();
        vj = pop_word();
        push_word(va);
        push_word(vi);
        push_word(vh);
        push_word(vg);
        push_word(vf);
        push_word(ve);
        push_word(vd);
        push_word(vc);
        push_word(vb);
        push_word(vj);
        break;
      case swap10_opcode:
        logger << "op swap10" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = pop_word();
        vd = pop_word();
        ve = pop_word();
        vf = pop_word();
        vg = pop_word();
        vh = pop_word();
        vi = pop_word();
        vj = pop_word();
        vk = pop_word();
        push_word(va);
        push_word(vj);
        push_word(vi);
        push_word(vh);
        push_word(vg);
        push_word(vf);
        push_word(ve);
        push_word(vd);
        push_word(vc);
        push_word(vb);
        push_word(vk);
        break;
      case swap11_opcode:
        logger << "op swap11" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = pop_word();
        vd = pop_word();
        ve = pop_word();
        vf = pop_word();
        vg = pop_word();
        vh = pop_word();
        vi = pop_word();
        vj = pop_word();
        vk = pop_word();
        vl = pop_word();
        push_word(va);
        push_word(vk);
        push_word(vj);
        push_word(vi);
        push_word(vh);
        push_word(vg);
        push_word(vf);
        push_word(ve);
        push_word(vd);
        push_word(vc);
        push_word(vb);
        push_word(vl);
        break;
      case swap12_opcode:
        logger << "op swap12" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = pop_word();
        vd = pop_word();
        ve = pop_word();
        vf = pop_word();
        vg = pop_word();
        vh = pop_word();
        vi = pop_word();
        vj = pop_word();
        vk = pop_word();
        vl = pop_word();
        vm = pop_word();
        push_word(va);
        push_word(vl);
        push_word(vk);
        push_word(vj);
        push_word(vi);
        push_word(vh);
        push_word(vg);
        push_word(vf);
        push_word(ve);
        push_word(vd);
        push_word(vc);
        push_word(vb);
        push_word(vm);
        break;
      case swap13_opcode:
        logger << "op swap13" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = pop_word();
        vd = pop_word();
        ve = pop_word();
        vf = pop_word();
        vg = pop_word();
        vh = pop_word();
        vi = pop_word();
        vj = pop_word();
        vk = pop_word();
        vl = pop_word();
        vm = pop_word();
        vn = pop_word();
        push_word(va);
        push_word(vm);
        push_word(vl);
        push_word(vk);
        push_word(vj);
        push_word(vi);
        push_word(vh);
        push_word(vg);
        push_word(vf);
        push_word(ve);
        push_word(vd);
        push_word(vc);
        push_word(vb);
        push_word(vn);
        break;
      case swap14_opcode:
        logger << "op swap14" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = pop_word();
        vd = pop_word();
        ve = pop_word();
        vf = pop_word();
        vg = pop_word();
        vh = pop_word();
        vi = pop_word();
        vj = pop_word();
        vk = pop_word();
        vl = pop_word();
        vm = pop_word();
        vn = pop_word();
        vo = pop_word();
        push_word(va);
        push_word(vn);
        push_word(vm);
        push_word(vl);
        push_word(vk);
        push_word(vj);
        push_word(vi);
        push_word(vh);
        push_word(vg);
        push_word(vf);
        push_word(ve);
        push_word(vd);
        push_word(vc);
        push_word(vb);
        push_word(vo);
        break;
      case swap15_opcode:
        logger << "op swap15" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = pop_word();
        vd = pop_word();
        ve = pop_word();
        vf = pop_word();
        vg = pop_word();
        vh = pop_word();
        vi = pop_word();
        vj = pop_word();
        vk = pop_word();
        vl = pop_word();
        vm = pop_word();
        vn = pop_word();
        vo = pop_word();
        vp = pop_word();
        push_word(va);
        push_word(vo);
        push_word(vn);
        push_word(vm);
        push_word(vl);
        push_word(vk);
        push_word(vj);
        push_word(vi);
        push_word(vh);
        push_word(vg);
        push_word(vf);
        push_word(ve);
        push_word(vd);
        push_word(vc);
        push_word(vb);
        push_word(vp);
        break;
      case swap16_opcode:
        logger << "op swap16" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = pop_word();
        vd = pop_word();
        ve = pop_word();
        vf = pop_word();
        vg = pop_word();
        vh = pop_word();
        vi = pop_word();
        vj = pop_word();
        vk = pop_word();
        vl = pop_word();
        vm = pop_word();
        vn = pop_word();
        vo = pop_word();
        vp = pop_word();
        vq = pop_word();
        push_word(va);
        push_word(vp);
        push_word(vo);
        push_word(vn);
        push_word(vm);
        push_word(vl);
        push_word(vk);
        push_word(vj);
        push_word(vi);
        push_word(vh);
        push_word(vg);
        push_word(vf);
        push_word(ve);
        push_word(vd);
        push_word(vc);
        push_word(vb);
        push_word(vq);
        break;
      case log0_opcode:
        {
          logger << "op log0" << std::endl;
          va = pop_word(); // offset
          vb = pop_word(); // length
          offset = size_t(va);
          length = size_t(vb);
          std::vector<word> slice;
          slice.resize(length);
          for (size_t i = 0; i < length; i++) {
            slice[i] = memory[offset + i];
          }
          const log_object o = { slice, {} };
          this->emit_log(o);
          break;
        }
      case log1_opcode:
        {
          logger << "op log1" << std::endl;
          va = pop_word(); // offset
          vb = pop_word(); // length
          offset = size_t(va);
          length = size_t(vb);
          std::vector<word> slice;
          slice.resize(length);
          for (size_t i = 0; i < length; i++) {
            slice[i] = memory[offset + i];
          }

          std::vector<big_word> topics;
          stack_variant& stack_object = stack.front();
          if (big_word* it = boost::get<big_word>(&stack_object))
          {
            stack.pop_front();
            topics.push_back(*it);
          }
          else {
            throw;
          }

          const log_object o = { slice, topics };
          this->emit_log(o);
          break;
        }
      case log2_opcode:
        {
          logger << "op log2" << std::endl;
          va = pop_word(); // offset
          vb = pop_word(); // length
          offset = size_t(va);
          length = size_t(vb);
          std::vector<word> slice;
          slice.resize(length);
          for (size_t i = 0; i < length; i++) {
            slice[i] = memory[offset + i];
          }

          std::vector<big_word> topics;

          stack_variant& stack_object = stack.front();
          if (big_word* it = boost::get<big_word>(&stack_object))
          {
            stack.pop_front();
            topics.push_back(*it);
          }
          else {
            throw;
          }

          stack_object = stack.front();
          if (big_word* it = boost::get<big_word>(&stack_object))
          {
            stack.pop_front();
            topics.push_back(*it);
          }
          else {
            throw;
          }

          const log_object o = { slice, topics };
          this->emit_log(o);
          break;
        }
      case log3_opcode:
        {
          logger << "op log3" << std::endl;
          va = pop_word(); // offset
          vb = pop_word(); // length
          offset = size_t(va);
          length = size_t(vb);
          std::vector<word> slice;
          slice.resize(length);
          for (size_t i = 0; i < length; i++) {
            slice[i] = memory[offset + i];
          }

          std::vector<big_word> topics;

          stack_variant& stack_object = stack.front();
          if (big_word* it = boost::get<big_word>(&stack_object))
          {
            stack.pop_front();
            topics.push_back(*it);
          }
          else {
            throw;
          }

          stack_object = stack.front();
          if (big_word* it = boost::get<big_word>(&stack_object))
          {
            stack.pop_front();
            topics.push_back(*it);
          }
          else {
            throw;
          }

          stack_object = stack.front();
          if (big_word* it = boost::get<big_word>(&stack_object))
          {
            stack.pop_front();
            topics.push_back(*it);
          }
          else {
            throw;
          }

          const log_object o = { slice, topics };
          this->emit_log(o);
          break;
        }
      case log4_opcode:
        {
          logger << "op log4" << std::endl;
          va = pop_word(); // offset
          vb = pop_word(); // length
          offset = size_t(va);
          length = size_t(vb);
          std::vector<word> slice;
          slice.resize(length);
          for (size_t i = 0; i < length; i++) {
            slice[i] = memory[offset + i];
          }

          std::vector<big_word> topics;

          stack_variant& stack_object = stack.front();
          if (big_word* it = boost::get<big_word>(&stack_object))
          {
            stack.pop_front();
            topics.push_back(*it);
          }
          else {
            throw;
          }

          stack_object = stack.front();
          if (big_word* it = boost::get<big_word>(&stack_object))
          {
            stack.pop_front();
            topics.push_back(*it);
          }
          else {
            throw;
          }

          stack_object = stack.front();
          if (big_word* it = boost::get<big_word>(&stack_object))
          {
            stack.pop_front();
            topics.push_back(*it);
          }
          else {
            throw;
          }

          stack_object = stack.front();
          if (big_word* it = boost::get<big_word>(&stack_object))
          {
            stack.pop_front();
            topics.push_back(*it);
          }
          else {
            throw;
          }

          const log_object o = { slice, topics };
          this->emit_log(o);
          break;
        }
      case create_opcode:
        logger << "op create" << std::endl;
        // TODO REVIEW
        va = pop_word(); // value
        vb = pop_word(); // offset
        vc = pop_word(); // length

        for (size_t i = static_cast<size_t>(vb); i < static_cast<size_t>(vc); i++) {
          std::map<size_t,word>::iterator it;
          it = memory.find(i);
          if (it != memory.end()) {
            retval.push_back(it->second);
          }
          else {
            retval.push_back(word(0));
          }
        }

        push_word( adapter.contract_create( retval, va ) ); // addr
        break;
      case call_opcode:
        logger << "op call" << std::endl;
        va = pop_word(); // energy

        sv = stack.front(); // addr
        ss = boost::get<std::string>(&sv);
        if (ss)
        {
          stack.pop_front();
          vb = pop_word(); // value
          vc = pop_word(); // argsOffset
          vd = pop_word(); // argsLength
          ve = pop_word(); // retOffset
          vf = pop_word(); // retLength

          for (size_t i = static_cast<size_t>(vc); i < static_cast<size_t>(vd); i++) {
            std::map<size_t,word>::iterator it;
            it = memory.find(i);
            if (it != memory.end()) {
              contract_args.push_back(it->second);
            }
            else {
              contract_args.push_back(word(0));
            }
          }

          std::vector<word> contract_call_return = adapter.contract_call(*ss, static_cast<uint64_t>(va), vb, contract_args);

          for (size_t i = 0; i < static_cast<size_t>(vf); i++)
            memory[static_cast<size_t>(ve) + i] = contract_call_return[i];

          // TODO revise stack return value?
          push_word("Success"); // success
        }
        else {
          push_word("Failure"); // success
          state = machine_state::error;
          error_message.emplace("Call operation type error");
        }
        break;
      case callcode_opcode:
        logger << "op callcode" << std::endl;
        va = pop_word(); // energy

        sv = stack.front(); // addr
        ss = boost::get<std::string>(&sv);
        if (ss)
        {
          stack.pop_front();
          vb = pop_word(); // value
          vc = pop_word(); // argsOffset
          vd = pop_word(); // argsLength
          ve = pop_word(); // retOffset
          vf = pop_word(); // retLength

          for (size_t i = static_cast<size_t>(vc); i < static_cast<size_t>(vd); i++) {
            std::map<size_t,word>::iterator it;
            it = memory.find(i);
            if (it != memory.end()) {
              contract_args.push_back(it->second);
            }
            else {
              contract_args.push_back(word(0));
            }
          }

          std::vector<word> contract_callcode_return = adapter.contract_callcode(*ss, static_cast<uint64_t>(va), vb, contract_args);

          for (size_t i = 0; i < static_cast<size_t>(vf); i++)
            memory[static_cast<size_t>(ve) + i] = contract_callcode_return[i];

          // TODO revise stack return value?
          push_word("Success"); // success
        }
        else {
          push_word("Failure"); // success
          state = machine_state::error;
          error_message.emplace("Callcode operation type error");
        }
        break;
      case return_opcode:
        logger << "op return" << std::endl;
        // TODO REVIEW
        // a = pop_word(); // offset
        // b = pop_word(); // length
        // logger << std::to_string(a) << std::endl;
        // logger << std::to_string(b) << std::endl; // return_value.resize(b);
        // // TODO: Bounds checking
        // // TODO: Check if size needs to be capped
        // // TODO: Optimize
        // for (int i = 0; i < return_value.size(); i++)
        //   return_value[i] = memory[a + i];
        // state = machine_state::stopped;

        va = pop_word(); // offset
        vb = pop_word(); // length

        for (size_t i = static_cast<size_t>(va); i < static_cast<size_t>(vb); i++) {
          std::map<size_t,word>::iterator it;
          it = memory.find(i);
          if (it != memory.end()) {
            retval.push_back(it->second);
          }
          else {
            retval.push_back(word(0));
          }
        }

        adapter.contract_return( retval );
        break;
      case delegatecall_opcode:
        logger << "op delegatecall" << std::endl;
        va = pop_word(); // energy

        sv = stack.front(); // addr
        ss = boost::get<std::string>(&sv);
        if (ss)
        {
          stack.pop_front();
          vb = pop_word(); // argsOffset
          vc = pop_word(); // argsLength
          vd = pop_word(); // retOffset
          ve = pop_word(); // retLength

          for (size_t i = static_cast<size_t>(va); i < static_cast<size_t>(vb); i++) {
            std::map<size_t,word>::iterator it;
            it = memory.find(i);
            if (it != memory.end()) {
              contract_args.push_back(it->second);
            }
            else {
              contract_args.push_back(word(0));
            }
          }

          std::vector<word> contract_delegatecall_return = adapter.contract_delegatecall(*ss, static_cast<uint64_t>(va), contract_args);

          for (size_t i = 0; i < static_cast<size_t>(ve); i++)
            memory[static_cast<size_t>(vd) + i] = contract_delegatecall_return[i];

          // TODO revise stack return value?
          push_word("Success"); // success
        }
        else {
          push_word("Failure"); // success
          state = machine_state::error;
          error_message.emplace("Delegatecall operation type error");
        }
        break;
      case create2_opcode:
        logger << "op create2" << std::endl;
        va = pop_word(); // value
        vb = pop_word(); // offset
        vc = pop_word(); // length

        sv = stack.front(); // salt
        ss = boost::get<std::string>(&sv);
        if (ss)
        {
          std::vector<word> args;

          for (size_t i = static_cast<size_t>(vb); i < static_cast<size_t>(vc); i++) {
            std::map<size_t,word>::iterator it;
            it = memory.find(i);
            if (it != memory.end()) {
              args.push_back(it->second);
            }
            else {
              args.push_back(word(0));
            }
          }

          push_word( adapter.contract_create2( args, va, *ss ) );
        }
        else {
          state = machine_state::error;
          error_message.emplace("Create2 operation type error");
        }
        break;
      case staticcall_opcode:
        logger << "op staticcall" << std::endl;
        va = pop_word(); // energy

        sv = stack.front(); // addr
        ss = boost::get<std::string>(&sv);
        if (ss)
        {
          stack.pop_front();
          vb = pop_word(); // argsOffset
          vc = pop_word(); // argsLength
          vd = pop_word(); // retOffset
          ve = pop_word(); // retLength

          for (size_t i = static_cast<size_t>(vb); i < static_cast<size_t>(vc); i++) {
            std::map<size_t,word>::iterator it;
            it = memory.find(i);
            if (it != memory.end()) {
              contract_args.push_back(it->second);
            }
            else {
              contract_args.push_back(word(0));
            }
          }

          std::vector<word> contract_staticcall_return = adapter.contract_staticcall(*ss, static_cast<uint64_t>(va), contract_args);

          for (size_t i = 0; i < static_cast<size_t>(ve); i++)
            memory[static_cast<size_t>(vd) + i] = contract_staticcall_return[i];

          // TODO revise stack return value?
          push_word("Success"); // success
        }
        else {
          push_word("Failure"); // success
          state = machine_state::error;
          error_message.emplace("Staticcall operation type error");
        }
        break;
      case revert_opcode:
        logger << "op revert" << std::endl;
        va = pop_word(); // offset
        vb = pop_word(); // length

        for (size_t i = static_cast<size_t>(va); i < static_cast<size_t>(vb); i++) {
          std::map<size_t,word>::iterator it;
          it = memory.find(i);
          if (it != memory.end()) {
            retval.push_back(it->second);
          }
          else {
            retval.push_back(word(0));
          }
        }
        adapter.revert( retval );
        break;
      case selfdestruct_opcode:
        logger << "op selfdestruct" << std::endl;
        sv = stack.front(); // addr
        ss = boost::get<std::string>(&sv);
        if (ss)
        {
          adapter.self_destruct(*ss);
        }
        break;
    }
  }

  machine_state machine::get_state()
  {
    return state;
  }

  boost::optional<std::string> machine::get_error_message()
  {
    return error_message;
  }

  std::stringstream& machine::get_logger()
  {
    return logger;
  }

  bool machine::is_running()
  {
    return state == machine_state::running;
  }

  std::string machine::to_json()
  {
    std::stringstream s;
    s << "{";
    {
      s << "\"finalState\":" << "{";
      {
        s << "\"pc\":" << pc << ",";
        s << "\"stack\":" << "[";
        for (auto it = stack.cbegin(); it != stack.cend(); ++it)
        {
          stack_variant sv = *it;
          s << sv;
          if (it + 1 != stack.cend())
            s << ",";
        }
        s << "],";
        s << "\"returnValue\":" << "[";
        for (auto it = return_value.cbegin(); it != return_value.cend(); ++it)
        {
          word w = *it;
          s << std::to_string(w);
          if (it + 1 != return_value.cend())
            s << ",";
        }
        s << "],";
        if (error_message == boost::none)
          s << "\"exceptionError\":" << "null";
        else
          s << "\"exceptionError\":" << "\"" << error_message.value() << "\"";
      }
      s << "}";
    }
    s << "}";
    return s.str();
  }

  void machine::emit_log(const log_object& o)
  {
  }
}
