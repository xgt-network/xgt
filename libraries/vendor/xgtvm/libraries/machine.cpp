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

  uint16_t get_bytes(boost::multiprecision::uint256_t x, int n)
  {
    std::vector<unsigned char> bytes;
    boost::multiprecision::export_bits(x, std::back_inserter(bytes), 8);
    if (x > 255) {
      return ((uint16_t)bytes[n] << 8) | bytes[n + 1];
    }
    return bytes[n];
  }

  size_t uint256_t_to_size_t(boost::multiprecision::uint256_t& x)
  {
    return x.convert_to<size_t>();
  }

  typed_big_word to_typed_big_word(int64_t a, bool is_sparse = false)
  {
    return typed_big_word(a, is_sparse);
  }

  typed_big_word to_typed_big_word(size_t a, bool is_sparse = false)
  {
    return typed_big_word(a, is_sparse);
  }

  typed_big_word to_typed_big_word(typed_word a, bool is_sparse = false)
  {
    return typed_big_word(a.value, is_sparse);
  }

  typed_big_word to_typed_big_word(typed_word a, typed_word b, bool is_sparse = false)
  {
    typed_big_word va = (to_typed_big_word(a).value << 8) | to_typed_big_word(b).value;
    va.is_sparse = is_sparse;
    return va;
  }

  typed_big_word to_typed_big_word(typed_word a, typed_word b, typed_word c, typed_word d, bool is_sparse = false)
  {
    typed_big_word va = (to_typed_big_word(a).value << 8) | to_typed_big_word(b).value;
    typed_big_word vb = (va.value << 8) | c.value;
    typed_big_word vc = (vb.value << 8) | d.value;
    vc.is_sparse = is_sparse;
    return vc;
  }

  typed_big_word to_typed_big_word(typed_word a, typed_word b, typed_word c, typed_word d, typed_word e, typed_word f, typed_word g, typed_word h, bool is_sparse = false)
  {
    typed_big_word va = (to_typed_big_word(a).value << 8) | to_typed_big_word(b).value;
    typed_big_word vb = (va.value << 8) | c.value;
    typed_big_word vc = (vb.value << 8) | d.value;
    typed_big_word vd = (vc.value << 8) | e.value;
    typed_big_word ve = (vd.value << 8) | f.value;
    typed_big_word vf = (ve.value << 8) | g.value;
    typed_big_word vg = (vf.value << 8) | h.value;
    vg.is_sparse = is_sparse;
    return vg;
  }

  typed_big_word to_typed_big_word(typed_word a, typed_word b, typed_word c, typed_word d, typed_word e, typed_word f, typed_word g,
      typed_word h, typed_word i, typed_word j, typed_word k, typed_word l, typed_word m, typed_word n, typed_word o, typed_word p, bool is_sparse = false)
  {
    typed_big_word va = (to_typed_big_word(a).value << 8) | to_typed_big_word(b).value;
    typed_big_word vb = (va.value << 8) | c.value;
    typed_big_word vc = (vb.value << 8) | d.value;
    typed_big_word vd = (vc.value << 8) | e.value;
    typed_big_word ve = (vd.value << 8) | f.value;
    typed_big_word vf = (ve.value << 8) | g.value;
    typed_big_word vg = (vf.value << 8) | h.value;
    typed_big_word vh = (vg.value << 8) | i.value;
    typed_big_word vi = (vh.value << 8) | j.value;
    typed_big_word vj = (vi.value << 8) | k.value;
    typed_big_word vk = (vj.value << 8) | l.value;
    typed_big_word vl = (vk.value << 8) | m.value;
    typed_big_word vm = (vl.value << 8) | n.value;
    typed_big_word vn = (vm.value << 8) | o.value;
    typed_big_word vo = (vn.value << 8) | p.value;
    vo.is_sparse = is_sparse;
    return vo;
  }

  typed_big_word to_typed_big_word(typed_word a, typed_word b, typed_word c, typed_word d, typed_word e, typed_word f, typed_word g,
      typed_word h, typed_word i, typed_word j, typed_word k, typed_word l, typed_word m, typed_word n, typed_word o, typed_word p,
      typed_word q, typed_word r, typed_word s, typed_word t, typed_word u, typed_word v, typed_word w, typed_word x, typed_word y,
      typed_word z, typed_word aa, typed_word ab, typed_word ac, typed_word ad, typed_word ae, typed_word af, bool is_sparse = false)
  {
    typed_big_word va  = (to_typed_big_word(a).value << 8) | to_typed_big_word(b).value;
    typed_big_word vb  = (va.value << 8)  | c.value;
    typed_big_word vc  = (vb.value << 8)  | d.value;
    typed_big_word vd  = (vc.value << 8)  | e.value;
    typed_big_word ve  = (vd.value << 8)  | f.value;
    typed_big_word vf  = (ve.value << 8)  | g.value;
    typed_big_word vg  = (vf.value << 8)  | h.value;
    typed_big_word vh  = (vg.value << 8)  | i.value;
    typed_big_word vi  = (vh.value << 8)  | j.value;
    typed_big_word vj  = (vi.value << 8)  | k.value;
    typed_big_word vk  = (vj.value << 8)  | l.value;
    typed_big_word vl  = (vk.value << 8)  | m.value;
    typed_big_word vm  = (vl.value << 8)  | n.value;
    typed_big_word vn  = (vm.value << 8)  | o.value;
    typed_big_word vo  = (vn.value << 8)  | p.value;
    typed_big_word vp  = (vo.value << 8)  | q.value;
    typed_big_word vq  = (vp.value << 8)  | r.value;
    typed_big_word vr  = (vq.value << 8)  | s.value;
    typed_big_word vs  = (vr.value << 8)  | t.value;
    typed_big_word vt  = (vs.value << 8)  | u.value;
    typed_big_word vu  = (vt.value << 8)  | v.value;
    typed_big_word vv  = (vu.value << 8)  | w.value;
    typed_big_word vw  = (vv.value << 8)  | x.value;
    typed_big_word vx  = (vw.value << 8)  | y.value;
    typed_big_word vy  = (vx.value << 8)  | z.value;
    typed_big_word vz  = (vy.value << 8)  | aa.value;
    typed_big_word vaa = (vz.value << 8)  | ab.value;
    typed_big_word vab = (vaa.value << 8) | ac.value;
    typed_big_word vac = (vab.value << 8) | ad.value;
    typed_big_word vad = (vac.value << 8) | ae.value;
    typed_big_word vae = (vad.value << 8) | af.value;
    vae.is_sparse = is_sparse;
    return vae;
  }

  std::vector<typed_word> from_typed_big_word(typed_big_word a)
  {
    std::vector<typed_word> vec;
    vec.push_back( typed_word( static_cast<word>(a.value & 0xFF), a.is_sparse ) );
    for (size_t i = 0; i < 31; i++) {
      vec.push_back( typed_word( static_cast<word>( (a.value >>= 8) & 0xFF ), a.is_sparse ) );
    }

    return vec;
  }

  std::string inspect(std::vector<typed_word> words)
  {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < words.size(); i++)
    {
      ss << std::to_string(words.at(i).value);
      if (i != words.size() - 1)
        ss << ", ";
    }
    ss << "]";
    return ss.str();
  }

  std::string inspect(std::map<size_t, typed_word> words)
  {
    std::stringstream ss;
    ss << "[";
    std::map<size_t,typed_word>::iterator it;
    size_t i = 0;
    while(true)
    {
      it = words.find(i);
      if (it != words.end()) {
        ss << std::to_string(it->second.value);
        ss << ", ";
      }
      else {
        ss << std::to_string(it->second.value);
        break;
      }
      i++;
    }
    ss << "]";
    return ss.str();
  }

  void machine::push_word(typed_big_word v)
  {
    stack.push_front(v);
  }

  typed_big_word machine::pop_word()
  {
    typed_big_word& stack_object = stack.front();
    stack.pop_front();
    return stack_object;
  }

  typed_big_word machine::peek_word()
  {
    typed_big_word& stack_object = stack.front();
    return stack_object;
  }

  void machine::print_stack()
  {
    for (auto it = stack.cbegin(); it != stack.cend(); ++it)
    {
      typed_big_word w = *it;
      logger << w.value;
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
    typed_word a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x,
         y, z, aa, ab, ac, ad, ae, af;
    typed_big_word va, vb, vc, vd, ve, vf, vg, vh, vi, vj, vk, vl, vm, vn, vo, vp, vq;

    word jumpdest_instruction;
    opcode jumpdest_op;
    typed_signed_big_word sa, sb, sc;
    size_t offset, dest_offset, length, code_size = 0;
    // TODO XXX convert words to typed_words
    std::vector<typed_word> vec1;
    std::vector<word> contract_args;
    std::vector<word> retval;
    std::vector<word> ext_contract_code;
    std::pair< word, std::vector<word> > contract_call_return;
    std::vector<typed_word>::const_iterator first, last;
    std::stringstream sstream;
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
        if ( vb == typed_big_word(0) )
          push_word( typed_big_word(0) );
        else
          vc = va / vb;
        push_word(vc);
        break;
      case sdiv_opcode:
        logger << "op sdiv" << std::endl;
        sa = alias_to_int256_t( pop_word().value );
        sb = alias_to_int256_t( pop_word().value );
        sc = sa / sb;
        push_word( alias_to_uint256_t(sc.value) );
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
        sa = alias_to_int256_t( pop_word().value );
        sb = alias_to_int256_t( pop_word().value );
        sc = sa % sb;
        push_word( alias_to_uint256_t(sc.value) );
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
        vc = typed_big_word(1);
        for (int i = 0; i < vb.value; i++) {
          vc *= va;
        }
        push_word(vc);
        break;
      case signextend_opcode:
        logger << "op signextend" << std::endl;
        sa = alias_to_int256_t( pop_word().value ); // b
        sb = alias_to_int256_t( pop_word().value ); // x

        if (sa.value < 31)
        {
          unsigned testBit = static_cast<unsigned>(sa.value) * 8 + 7;
          typed_big_word one(1);
          typed_big_word mask = ((one.value << testBit) - 1);
          if ( (sb & static_cast<signed_big_word>( (one.value << testBit) ) ) != typed_signed_big_word(0) )
            sc = sb.value | static_cast<signed_big_word>(~mask.value);
          else
            sc = sb.value & static_cast<signed_big_word>(mask.value);
        }

        push_word( alias_to_uint256_t(sc.value) );
        break;
      case lt_opcode:
        logger << "op lt" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = typed_big_word(va < vb);
        push_word(vc);
        break;
      case gt_opcode:
        logger << "op gt" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = typed_big_word(va > vb);
        push_word(vc);
        break;
      case slt_opcode:
        logger << "op stl" << std::endl;
        sa = alias_to_int256_t( pop_word().value );
        sb = alias_to_int256_t( pop_word().value );
        sc = sa < sb ? typed_signed_big_word(1) : typed_signed_big_word(0);
        push_word( alias_to_uint256_t(sc.value) );
        break;
      case sgt_opcode:
        logger << "op sgt" << std::endl;
        sa = alias_to_int256_t( pop_word().value );
        sb = alias_to_int256_t( pop_word().value );
        sc = sa > sb ? typed_signed_big_word(1) : typed_signed_big_word(0);
        push_word( alias_to_uint256_t(sc.value) );
        break;
      case eq_opcode:
        logger << "op eq" << std::endl;
        va = pop_word();
        vb = pop_word();
        vc = va == vb ? typed_big_word(1) : typed_big_word(0);
        push_word(vc);
        break;
      case iszero_opcode:
        logger << "op iszero" << std::endl;
        va = pop_word();
        vb = va == typed_big_word(0) ? typed_big_word(1) : typed_big_word(0);
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

        sa = static_cast<signed_big_word>( pop_word().value ); // index
        sb = static_cast<signed_big_word>( pop_word().value ); // num

        sc = sa.value < 32 ? (sb.value >> (unsigned)(8 * (31 - sa.value))) & 0xff : 0;

        push_word( alias_to_uint256_t(sc.value) );
        break;
      case shl_opcode:
        logger << "op shl" << std::endl;

        va = pop_word(); // shift
        vb = pop_word(); // value

        vc = vb.value << static_cast<size_t>(va.value);

        push_word(vc);
        break;
      case shr_opcode:
        logger << "op shr" << std::endl;
        va = pop_word(); // shift
        vb = pop_word(); // value

        // TODO Implement shift in a loop for va sizes larger than 256
        vc = vb.value >> static_cast<size_t>(va.value);
        push_word(vc);

        break;
      case sar_opcode:
        logger << "op sar" << std::endl;
        sa = alias_to_int256_t( pop_word().value ); // shift
        sb = static_cast<signed_big_word>( pop_word().value ); // value

        sc = sb.value >> static_cast<size_t>(sa.value);
        push_word( alias_to_uint256_t(sc.value) );

        break;
      case sha3_opcode:
        logger << "op sha3" << std::endl;
        va = pop_word(); // offset
        vb = pop_word(); // length

        if ( vb == typed_big_word(0) ) {
          push_word( typed_big_word(0) );
        } else {
          for (size_t i = static_cast<size_t>(va.value); i < static_cast<size_t>(va.value) + static_cast<size_t>(vb.value); i++) {
            std::map<size_t,typed_word>::iterator it;
            it = memory.find(i);
            if (it != memory.end()) {
              if ( it->second.is_sparse ) {
                for (size_t i = 0; i < 31; i++) {
                  retval.push_back( word(0) );
                }
              }
              retval.push_back( word(it->second.value) );
            }
            else {
              if ( it->second.is_sparse ) {
                for (size_t i = 0; i < 32; i++) {
                  retval.push_back( word(0) );
                }
              } else {
                retval.push_back( word(0) );
              }
            }
          }

          vc = adapter.sha3( retval );
          push_word( vc ); // hash
        }

        break;
      case address_opcode:
        logger << "op address" << std::endl;
        push_word( typed_big_word(msg.destination) );
        break;
      case balance_opcode:
        logger << "op balance" << std::endl;
        va = pop_word();
        push_word( typed_big_word( adapter.get_balance(va.value) ) );
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
        push_word( typed_big_word(msg.value) );
        break;
      case calldataload_opcode:
        logger << "op calldataload" << std::endl;
        va = pop_word();
        offset = static_cast<size_t>( va.value );

        if (offset > msg.input_size) {
          logger << "calldataload start index is larger than message input_size" << std::endl;
          break;
        }

        vb = to_typed_big_word(
            msg.input_data[offset + 0],
            msg.input_data[offset + 1],
            msg.input_data[offset + 2],
            msg.input_data[offset + 3],
            msg.input_data[offset + 4],
            msg.input_data[offset + 5],
            msg.input_data[offset + 6],
            msg.input_data[offset + 7],
            msg.input_data[offset + 8],
            msg.input_data[offset + 9],
            msg.input_data[offset + 10],
            msg.input_data[offset + 11],
            msg.input_data[offset + 12],
            msg.input_data[offset + 13],
            msg.input_data[offset + 14],
            msg.input_data[offset + 15],
            msg.input_data[offset + 16],
            msg.input_data[offset + 17],
            msg.input_data[offset + 18],
            msg.input_data[offset + 19],
            msg.input_data[offset + 20],
            msg.input_data[offset + 21],
            msg.input_data[offset + 22],
            msg.input_data[offset + 23],
            msg.input_data[offset + 24],
            msg.input_data[offset + 25],
            msg.input_data[offset + 26],
            msg.input_data[offset + 27],
            msg.input_data[offset + 28],
            msg.input_data[offset + 29],
            msg.input_data[offset + 30],
            msg.input_data[offset + 31],
            true
        );

        push_word(vb);
        break;
      case calldatasize_opcode:
        logger << "op calldatasize" << std::endl;
        push_word( to_typed_big_word(msg.input_size) );
        break;
      case calldatacopy_opcode:
        logger << "op calldatacopy" << std::endl;

        dest_offset = static_cast<size_t>( pop_word().value );
        offset = static_cast<size_t>( pop_word().value );
        length = static_cast<size_t>( pop_word().value );

        if ((offset + length) > msg.input_size) {
          logger << "calldatacopy end index is larger than message input_size" << std::endl;
          break;
        }

        for (size_t i = 0; i < length; ++i) {
          memory[dest_offset + i] = typed_word(msg.input_data[offset + i], true);
          // TODO This isn't being set correctly in the constructor
          memory[dest_offset + i].is_sparse = true;
        }

        break;
      case codesize_opcode:
        logger << "op codesize" << std::endl;
        push_word( to_typed_big_word( code.size() ) );
        break;
      case codecopy_opcode:
        logger << "op codecopy" << std::endl;

        dest_offset = static_cast<size_t>( pop_word().value );
        offset = static_cast<size_t>( pop_word().value );
        length = static_cast<size_t>( pop_word().value );

        for (size_t i = 0; i < length; ++i) {
          memory[dest_offset + i] = typed_word(code[offset + i], true);
          // TODO This isn't being set correctly in the constructor
          memory[dest_offset + i].is_sparse = true;
        }

        break;
      case energyprice_opcode:
        logger << "op energyprice" << std::endl;
        push_word( to_typed_big_word( ctx.tx_energyprice ) );
        break;
      case extcodesize_opcode:
        logger << "op extcodesize" << std::endl;
        va = pop_word(); // Address
        ext_contract_code = adapter.get_code_at_addr(va.value);
        push_word( typed_big_word( sizeof(ext_contract_code) / sizeof(ext_contract_code[0]) ) );
        break;
      case extcodecopy_opcode:
        logger << "op extcodecopy" << std::endl;
        va = pop_word(); // address
        ext_contract_code = adapter.get_code_at_addr(va.value);
        code_size = sizeof(ext_contract_code) / sizeof(ext_contract_code[0]);
        dest_offset = static_cast<size_t>( pop_word().value );
        offset = static_cast<size_t>( pop_word().value );
        length = static_cast<size_t>( pop_word().value );

        if ((offset + length) > code_size) {
          logger << "codecopy end index exceeds external contract code length" << std::endl;
          break;
        }

        for (size_t i = 0; i < length; ++i) {
          memory[dest_offset + i] = typed_word(ext_contract_code[offset + i], true);
          // TODO This isn't being set correctly in the constructor
          memory[dest_offset + i].is_sparse = true;
        }

        break;
      case returndatasize_opcode:
        logger << "op returndatasize" << std::endl;
        push_word( typed_big_word( ext_return_data.size() ) );
        break;
      case returndatacopy_opcode:
        logger << "op returndatacopy" << std::endl;
        dest_offset = static_cast<size_t>( pop_word().value );
        offset = static_cast<size_t>( pop_word().value );
        length = static_cast<size_t>( pop_word().value );

        if ( (offset + length) > ext_return_data.size() ) {
          logger << "returndatacopy end index exceeds return data size" << std::endl;
          break;
        }

        for (size_t i = 0; i < length; ++i)
          memory[dest_offset + i] = ext_return_data[offset + i];

        break;
      case extcodehash_opcode:
        logger << "op extcodehash" << std::endl;
        va = pop_word(); // addr
        push_word( typed_big_word( adapter.get_code_hash(va.value) ) );
        break;
      case blockhash_opcode:
        logger << "op blockhash" << std::endl;
        va = pop_word();
        push_word( adapter.get_block_hash( static_cast<uint64_t>(va.value) ) );
        break;
      case coinbase_opcode:
        logger << "op coinbase" << std::endl;
        push_word( typed_big_word(ctx.block_coinbase) );
        break;
      case timestamp_opcode:
        logger << "op timestamp" << std::endl;
        push_word( typed_big_word(ctx.block_timestamp) );
        break;
      case number_opcode:
        logger << "op number" << std::endl;
        push_word( typed_big_word(ctx.block_number) );
        break;
      case difficulty_opcode:
        logger << "op difficulty" << std::endl;
        push_word( typed_big_word(ctx.block_difficulty) );
        break;
      case energylimit_opcode:
        logger << "op energylimit" << std::endl;
        push_word( typed_big_word(ctx.block_energylimit) );
        break;
      case selfbalance_opcode:
        logger << "op selfbalance" << std::endl;
        push_word( adapter.get_balance( msg.destination ) );
        break;
      case pop_opcode:
        logger << "op pop" << std::endl;
        pop_word();
        break;
      case mload_opcode:
        logger << "op mload" << std::endl;
        va = pop_word(); // offset
        vb = to_typed_big_word(
            memory[static_cast<size_t>(va.value) + 0],
            memory[static_cast<size_t>(va.value) + 1],
            memory[static_cast<size_t>(va.value) + 2],
            memory[static_cast<size_t>(va.value) + 3],
            memory[static_cast<size_t>(va.value) + 4],
            memory[static_cast<size_t>(va.value) + 5],
            memory[static_cast<size_t>(va.value) + 6],
            memory[static_cast<size_t>(va.value) + 7],
            memory[static_cast<size_t>(va.value) + 8],
            memory[static_cast<size_t>(va.value) + 9],
            memory[static_cast<size_t>(va.value) + 10],
            memory[static_cast<size_t>(va.value) + 11],
            memory[static_cast<size_t>(va.value) + 12],
            memory[static_cast<size_t>(va.value) + 13],
            memory[static_cast<size_t>(va.value) + 14],
            memory[static_cast<size_t>(va.value) + 15],
            memory[static_cast<size_t>(va.value) + 16],
            memory[static_cast<size_t>(va.value) + 17],
            memory[static_cast<size_t>(va.value) + 18],
            memory[static_cast<size_t>(va.value) + 19],
            memory[static_cast<size_t>(va.value) + 20],
            memory[static_cast<size_t>(va.value) + 21],
            memory[static_cast<size_t>(va.value) + 22],
            memory[static_cast<size_t>(va.value) + 23],
            memory[static_cast<size_t>(va.value) + 24],
            memory[static_cast<size_t>(va.value) + 25],
            memory[static_cast<size_t>(va.value) + 26],
            memory[static_cast<size_t>(va.value) + 27],
            memory[static_cast<size_t>(va.value) + 28],
            memory[static_cast<size_t>(va.value) + 29],
            memory[static_cast<size_t>(va.value) + 30],
            memory[static_cast<size_t>(va.value) + 31],
            memory[static_cast<size_t>(va.value)].is_sparse
        );

        push_word(vb);
        break;
      case mstore_opcode:
        logger << "op mstore" << std::endl;
        va = pop_word(); // offset
        vb = pop_word(); // value
        logger << "memory before: " << inspect(memory) << std::endl;

        vec1 = from_typed_big_word(vb);
        for (size_t i = 0; i < 32; i++)
          memory[static_cast<size_t>(va.value) + i] = vec1[31 - i];

        logger << "memory after: " << inspect(memory) << std::endl;
        break;
      case mstore8_opcode:
        logger << "op mstore8" << std::endl;
        va = pop_word(); // offset
        vb = pop_word(); // value
        logger << "memory before: " << inspect(memory) << std::endl;
        memory[static_cast<size_t>(va.value)] = get_byte(vb.value, 0); // TODO verify byte of typed_big_word vb
        logger << "memory after: " << inspect(memory) << std::endl;
        break;
      case sload_opcode:
        logger << "op sload" << std::endl;
        va = pop_word();
        vb = typed_big_word( adapter.get_storage(va.value) );

        push_word(vb);
        break;
      case sstore_opcode:
        logger << "op sstore" << std::endl;
        va = pop_word();
        vb = pop_word();

        adapter.set_storage(va.value, vb.value);
        break;
      case jump_opcode:
        logger << "op jump" << std::endl;
        va = pop_word(); // destination

        jumpdest_instruction = code[get_bytes( va.value, 0 )];
        jumpdest_op = (opcode)jumpdest_instruction;
        if (jumpdest_op == jumpdest_opcode)
          pc = get_bytes( va.value, 0 );
        break;
      case jumpi_opcode:
        logger << "op jumpi" << std::endl;
        print_stack();
        va = pop_word(); // destination
        vb = pop_word(); // condition

        if (vb != typed_big_word(0)) {
          jumpdest_instruction = code[get_bytes( va.value, 0 )];
          jumpdest_op = (opcode)jumpdest_instruction;
          if (jumpdest_op == jumpdest_opcode)
            pc = get_bytes( va.value, 0 );
        }
        break;
      case pc_opcode:
        logger << "op pc" << std::endl;
        va = typed_big_word(pc);
        push_word(va);
        break;
      case msize_opcode:
        logger << "op msize" << std::endl;
        push_word( to_typed_big_word( memory.size() ) );
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
        push_word(to_typed_big_word(a));
        break;
      case push2_opcode:
        logger << "op push2" << std::endl;
        a = code[pc];
        pc++;
        b = code[pc];
        pc++;
        va = to_typed_big_word(a, b);
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
        va = to_typed_big_word(0, a, b, c);
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
        va = to_typed_big_word(a, b, c, d);
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
        va = to_typed_big_word(0, 0, 0, a, b, c, d, e);
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
        va = to_typed_big_word(0, 0, a, b, c, d, e, f);
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
        va = to_typed_big_word(0, a, b, c, d, e, f, g);
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
        va = to_typed_big_word(a, b, c, d, e, f, g, h);
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
        va = to_typed_big_word(0, 0, 0, 0, 0, 0, 0, a, b, c, d, e, f, g, h, i);
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
        va = to_typed_big_word(0, 0, 0, 0, 0, 0, a, b, c, d, e, f, g, h, i, j);
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
        va = to_typed_big_word(0, 0, 0, 0, 0, a, b, c, d, e, f, g, h, i, j, k);
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
        va = to_typed_big_word(0, 0, 0, 0, a, b, c, d, e, f, g, h, i, j, k, l);
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
        va = to_typed_big_word(0, 0, 0, a, b, c, d, e, f, g, h, i, j, k, l, m);
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
        va = to_typed_big_word(0, 0, a, b, c, d, e, f, g, h, i, j, k, l, m, n);
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
        va = to_typed_big_word(0, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o);
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
        va = to_typed_big_word(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p);
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
        va = to_typed_big_word(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, a, b, c, d,
            e, f, g, h, i, j, k, l, m, n, o, p, q);
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
        va = to_typed_big_word(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, a, b, c, d, e,
            f, g, h, i, j, k, l, m, n, o, p, q, r);
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
        va = to_typed_big_word(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, a, b, c, d, e, f,
            g, h, i, j, k, l, m, n, o, p, q, r, s);
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
        va = to_typed_big_word(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, a, b, c, d, e, f, g,
            h, i, j, k, l, m, n, o, p, q, r, s, t);
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
        va = to_typed_big_word(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, a, b, c, d, e, f, g, h,
            i, j, k, l, m, n, o, p, q, r, s, t, u);
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
        va = to_typed_big_word(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, a, b, c, d, e, f, g, h, i,
            j, k, l, m, n, o, p, q, r, s, t, u, v);
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
        va = to_typed_big_word(0, 0, 0, 0, 0, 0, 0, 0, 0, a, b, c, d, e, f, g, h, i, j,
            k, l, m, n, o, p, q, r, s, t, u, v, w);
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
        va = to_typed_big_word(0, 0, 0, 0, 0, 0, 0, 0, a, b, c, d, e, f, g, h, i, j, k,
            l, m, n, o, p, q, r, s, t, u, v, w, x);
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
        va = to_typed_big_word(0, 0, 0, 0, 0, 0, 0, a, b, c, d, e, f, g, h, i, j, k, l,
            m, n, o, p, q, r, s, t, u, v, w, x, y);
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
        va = to_typed_big_word(0, 0, 0, 0, 0, 0, a, b, c, d, e, f, g, h, i, j, k, l, m,
            n, o, p, q, r, s, t, u, v, w, x, y, z);
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
        va = to_typed_big_word(0, 0, 0, 0, 0, a, b, c, d, e, f, g, h, i, j, k, l, m, n,
            o, p, q, r, s, t, u, v, w, x, y, z, aa);
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
        va = to_typed_big_word(0, 0, 0, 0, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o,
            p, q, r, s, t, u, v, w, x, y, z, aa, ab);
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
        va = to_typed_big_word(0, 0, 0, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p,
            q, r, s, t, u, v, w, x, y, z, aa, ab, ac);
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
        va = to_typed_big_word(0, 0, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p,
            q, r, s, t, u, v, w, x, y, z, aa, ab, ac, ad);
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
        va = to_typed_big_word(0, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p,
            q, r, s, t, u, v, w, x, y, z, aa, ab, ac, ad, ae);
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
        va = to_typed_big_word(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p,
            q, r, s, t, u, v, w, x, y, z, aa, ab, ac, ad, ae, af);
        push_word(va);
        break;
      case dup1_opcode:
        logger << "op dup1" << std::endl;
        va = stack.at(0);
        push_word(va);
        break;
      case dup2_opcode:
        logger << "op dup2" << std::endl;
        va = stack.at(1);
        push_word(va);
        break;
      case dup3_opcode:
        logger << "op dup3" << std::endl;
        va = stack.at(2);
        push_word(va);
        break;
      case dup4_opcode:
        logger << "op dup4" << std::endl;
        va = stack.at(3);
        push_word(va);
        break;
      case dup5_opcode:
        logger << "op dup5" << std::endl;
        va = stack.at(4);
        push_word(va);
        break;
      case dup6_opcode:
        logger << "op dup6" << std::endl;
        va = stack.at(5);
        push_word(va);
        break;
      case dup7_opcode:
        logger << "op dup7" << std::endl;
        va = stack.at(6);
        push_word(va);
        break;
      case dup8_opcode:
        logger << "op dup8" << std::endl;
        va = stack.at(7);
        push_word(va);
        break;
      case dup9_opcode:
        logger << "op dup9" << std::endl;
        va = stack.at(8);
        push_word(va);
        break;
      case dup10_opcode:
        logger << "op dup10" << std::endl;
        va = stack.at(9);
        push_word(va);
        break;
      case dup11_opcode:
        logger << "op dup11" << std::endl;
        va = stack.at(10);
        push_word(va);
        break;
      case dup12_opcode:
        logger << "op dup12" << std::endl;
        va = stack.at(11);
        push_word(va);
        break;
      case dup13_opcode:
        logger << "op dup13" << std::endl;
        va = stack.at(12);
        push_word(va);
        break;
      case dup14_opcode:
        logger << "op dup14" << std::endl;
        va = stack.at(13);
        push_word(va);
        break;
      case dup15_opcode:
        logger << "op dup15" << std::endl;
        va = stack.at(14);
        push_word(va);
        break;
      case dup16_opcode:
        logger << "op dup16" << std::endl;
        va = stack.at(15);
        push_word(va);
        break;
      case swap1_opcode:
        logger << "op swap1" << std::endl;
        va = pop_word();
        vb = pop_word();
        push_word(va);
        push_word(vb);
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
          offset = static_cast<size_t>(va.value);
          length = static_cast<size_t>(vb.value);
          std::vector<word> slice;
          slice.resize(length);
          for (size_t i = 0; i < length; i++) {
            slice[i] = memory[offset + i].value;
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
          offset = static_cast<size_t>(va.value);
          length = static_cast<size_t>(vb.value);
          std::vector<word> slice;
          slice.resize(length);
          for (size_t i = 0; i < length; i++) {
            slice[i] = memory[offset + i].value;
          }

          std::vector<big_word> topics;

          topics.push_back( pop_word().value );

          const log_object o = { slice, topics };
          this->emit_log(o);
          break;
        }
      case log2_opcode:
        {
          logger << "op log2" << std::endl;
          va = pop_word(); // offset
          vb = pop_word(); // length
          offset = static_cast<size_t>(va.value);
          length = static_cast<size_t>(vb.value);
          std::vector<word> slice;
          slice.resize(length);
          for (size_t i = 0; i < length; i++) {
            slice[i] = memory[offset + i].value;
          }

          std::vector<big_word> topics;

          for (size_t i = 0; i < 2; i++) {
            topics.push_back( pop_word().value );
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
          offset = static_cast<size_t>(va.value);
          length = static_cast<size_t>(vb.value);
          std::vector<word> slice;
          slice.resize(length);
          for (size_t i = 0; i < length; i++) {
            slice[i] = memory[offset + i].value;
          }

          std::vector<big_word> topics;

          for (size_t i = 0; i < 3; i++) {
            topics.push_back( pop_word().value );
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
          offset = static_cast<size_t>(va.value);
          length = static_cast<size_t>(vb.value);
          std::vector<word> slice;
          slice.resize(length);
          for (size_t i = 0; i < length; i++) {
            slice[i] = memory[offset + i].value;
          }

          std::vector<big_word> topics;

          for (size_t i = 0; i < 4; i++) {
            topics.push_back( pop_word().value );
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

        for (size_t i = static_cast<size_t>(vb.value); i < static_cast<size_t>(vb.value) + static_cast<size_t>(vc.value); i++) {
          std::map<size_t,typed_word>::iterator it;
          it = memory.find(i);
          if (it != memory.end()) {
            retval.push_back(it->second.value);
          }
          else {
            retval.push_back(word(0));
          }
        }

        push_word( typed_big_word( adapter.contract_create( retval, va.value ) ) ); // addr
        break;
      case call_opcode:
        logger << "op call" << std::endl;
        va = pop_word(); // energy

        vb = pop_word(); // address
        vc = pop_word(); // value
        vd = pop_word(); // argsOffset
        ve = pop_word(); // argsLength
        vf = pop_word(); // retOffset
        vg = pop_word(); // retLength

        for (size_t i = static_cast<size_t>(vd.value); i < static_cast<size_t>(vd.value) + static_cast<size_t>(ve.value); i++) {
          std::map<size_t,typed_word>::iterator it;
          it = memory.find(i);
          if (it != memory.end()) {
            contract_args.push_back(it->second.value);
          }
          else {
            contract_args.push_back(word(0));
          }
        }

        contract_call_return = adapter.contract_call(vb.value, static_cast<uint64_t>(va.value), vc.value, contract_args);

        if (contract_call_return.second.size() > 0) {
          ext_return_data = contract_call_return.second;

          for (size_t i = 0; i < static_cast<size_t>(vf.value); i++)
            memory[static_cast<size_t>(ve.value) + i] = contract_call_return.second[i];
        }

        push_word(typed_big_word(contract_call_return.first));
        break;
      case callcode_opcode:
        logger << "op callcode" << std::endl;
        va = pop_word(); // energy

        vb = pop_word(); // address
        vc = pop_word(); // value
        vd = pop_word(); // argsOffset
        ve = pop_word(); // argsLength
        vf = pop_word(); // retOffset
        vg = pop_word(); // retLength

        for (size_t i = static_cast<size_t>(vd.value); i < static_cast<size_t>(vd.value) + static_cast<size_t>(ve.value); i++) {
          std::map<size_t,typed_word>::iterator it;
          it = memory.find(i);
          if (it != memory.end()) {
            contract_args.push_back(it->second.value);
          }
          else {
            contract_args.push_back(word(0));
          }
        }

        contract_call_return = adapter.contract_callcode(vb.value, static_cast<uint64_t>(va.value), vc.value, contract_args);

        if (contract_call_return.second.size() > 0) {
          ext_return_data = contract_call_return.second;

          for (size_t i = 0; i < static_cast<size_t>(vf.value); i++)
            memory[static_cast<size_t>(ve.value) + i] = contract_call_return.second[i];
        }

        push_word(typed_big_word(contract_call_return.first));
        break;
      case return_opcode:
        logger << "op return" << std::endl;
        // XXX
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

        for (size_t i = static_cast<size_t>(va.value); i < static_cast<size_t>(va.value + vb.value); i++) {
          std::map<size_t,typed_word>::iterator it;
          it = memory.find(i);
          if (it != memory.end()) {
            retval.push_back(it->second.value);
          }
          else {
            retval.push_back(word(0));
          }
        }

        return_value = retval;
        adapter.contract_return( retval );
        state = machine_state::stopped;
        break;
      case delegatecall_opcode:
        logger << "op delegatecall" << std::endl;
        va = pop_word(); // energy
        vb = pop_word(); // addr
        vc = pop_word(); // argsOffset
        vd = pop_word(); // argsLength
        ve = pop_word(); // retOffset
        vf = pop_word(); // retLength

        for (size_t i = static_cast<size_t>(vc.value); i < static_cast<size_t>(vc.value) + static_cast<size_t>(vd.value); i++) {
          std::map<size_t,typed_word>::iterator it;
          it = memory.find(i);
          if (it != memory.end()) {
            contract_args.push_back(it->second.value);
          }
          else {
            contract_args.push_back(word(0));
          }
        }

        contract_call_return = adapter.contract_delegatecall(vb.value, static_cast<uint64_t>(va.value), contract_args);

        if (contract_call_return.second.size() > 0) {
          ext_return_data = contract_call_return.second;

          for (size_t i = 0; i < static_cast<size_t>(vf.value); i++)
            memory[static_cast<size_t>(ve.value) + i] = contract_call_return.second[i];
        }

        push_word( typed_big_word(contract_call_return.first) );
        break;
      case create2_opcode:
        logger << "op create2" << std::endl;
        va = pop_word(); // value
        vb = pop_word(); // offset
        vc = pop_word(); // length
        vd = pop_word(); // salt

        for (size_t i = static_cast<size_t>(vb.value); i < static_cast<size_t>(vb.value) + static_cast<size_t>(vc.value); i++) {
          std::map<size_t, typed_word>::iterator it;
          it = memory.find(i);
          if (it != memory.end()) {
            if ( it->second.is_sparse ) {
              for (size_t i = 0; i < 31; i++) {
                contract_args.push_back( word(0) );
              }
            }
            contract_args.push_back( word(it->second.value) );
          }
          else {
            if ( it->second.is_sparse ) {
              for (size_t i = 0; i < 32; i++) {
                contract_args.push_back( word(0) );
              }
            } else {
              contract_args.push_back( word(0) );
            }
          }
        }

        push_word( typed_big_word( adapter.contract_create2( contract_args, va.value, vd.value ) ) );

        break;
      case staticcall_opcode:
        logger << "op staticcall" << std::endl;
        va = pop_word(); // energy

        vb = pop_word(); // address
        vc = pop_word(); // argsOffset
        vd = pop_word(); // argsLength
        ve = pop_word(); // retOffset
        vf = pop_word(); // retLength

        for (size_t i = static_cast<size_t>(vc.value); i < static_cast<size_t>(vc.value) + static_cast<size_t>(vd.value); i++) {
          std::map<size_t,typed_word>::iterator it;
          it = memory.find(i);
          if (it != memory.end()) {
            contract_args.push_back(it->second.value);
          }
          else {
            contract_args.push_back(word(0));
          }
        }

        contract_call_return = adapter.contract_staticcall(vb.value, static_cast<uint64_t>(va.value), contract_args);

        ext_return_data = contract_call_return.second;

        for (size_t i = 0; i < static_cast<size_t>(vf.value); i++)
          memory[static_cast<size_t>(ve.value) + i] = contract_call_return.second[i];

        push_word(typed_big_word(contract_call_return.first));
        break;
      case revert_opcode:
        logger << "op revert" << std::endl;
        va = pop_word(); // offset
        vb = pop_word(); // length

        for (size_t i = static_cast<size_t>(va.value); i < static_cast<size_t>(vb.value); i++) {
          std::map<size_t,typed_word>::iterator it;
          it = memory.find(i);
          if (it != memory.end()) {
            retval.push_back(it->second.value);
          }
          else {
            retval.push_back(word(0));
          }
          memory[i] = 0;
        }
        return_value = retval;
        adapter.revert( retval );
        state = machine_state::stopped;
        break;
      case invalid_opcode:
        logger << "op invalid" << std::endl;
        break;
      case selfdestruct_opcode:
        logger << "op selfdestruct" << std::endl;
        va = stack.front(); // addr
        adapter.self_destruct(va.value);
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

  std::vector<word>& machine::get_return_value()
  {
    return return_value;
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
        s << "\"pc\":" << std::hex << pc << ",";
        s << "\"stack\":" << "[";
        for (auto it = stack.cbegin(); it != stack.cend(); ++it)
        {
          typed_big_word bw = *it;
          s << bw.value;
          s << ":";
          s << bw.is_sparse;
          if (stack.size() > 1 && std::next(it, 1) != stack.cend())
            s << ",";
        }
        s << "],";
        s << "\"returnValue\":" << "[";
        for (auto it = return_value.cbegin(); it != return_value.cend(); ++it)
        {
          word w = *it;
          s << std::to_string(w);
          if (return_value.size() > 1 && std::next(it, 1) != return_value.cend())
            s << ",";
        }
        s << "],";

        s << "\"machineState\":\"";
        switch (state) {
          case machine_state::stopped:
            s << "stopped";
            break;
          case machine_state::running:
            s << "running";
            break;
          case machine_state::error:
            s << "error";
            break;
          default:
            s << "unknown";
        }
        s << "\",";

        if (code.size() != 0) {
          word current_instruction = code[pc];
          opcode op = (opcode)current_instruction;
          s << "\"opcode\":" << std::hex << op << ",";
        } else {
          s << "\"opcode\":null,";
        }

        if (error_message == boost::none)
          s << "\"exceptionError\":" << "null";
        else
          s << "\"exceptionError\":" << "\"" << error_message.value() << "\"";
        s << ",";

        s << "\"memory\":" << "{";
        for (auto it = memory.cbegin(); it != memory.cend(); ++it)
        {
          s << "\"" << it->first << "\"";
          s << ":";
          s << unsigned(it->second.value);
          s << " : ";
          s << it->second.is_sparse;
          if (memory.size() > 1 && std::next(it, 1) != memory.cend())
            s << ",";
        }
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
