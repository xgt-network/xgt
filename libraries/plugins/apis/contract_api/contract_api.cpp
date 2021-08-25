#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/plugins/contract_api/contract_api_plugin.hpp>
#include <xgt/plugins/contract_api/contract_api.hpp>

#include <xgt/chain/wallet_object.hpp>
#include <xgt/chain/database.hpp>
#include <xgt/chain/index.hpp>

#include "keccak256.h"

#include <machine.hpp>

namespace xgt { namespace plugins { namespace contract {

namespace detail {

class contract_api_impl
{
   public:
      contract_api_impl()
      : _db( appbase::app().get_plugin<xgt::plugins::chain::chain_plugin>().db() ) {}

      DECLARE_API_IMPL(
         (get_contract)
         (list_owner_contracts)
         (invoke) )

      chain::database& _db;
};

DEFINE_API_IMPL( contract_api_impl, get_contract )
{
   // TODO: Temporary
   machine::message msg = {};
   ilog( "machine::message msg.flags ${f}", ("f",msg.flags) );

   // TODO: Temporary
   std::string message = "testing";
   unsigned char output[32];
   SHA3_CTX ctx;
   keccak_init(&ctx);
   keccak_update(&ctx, (unsigned char*)message.c_str(), message.size());
   keccak_final(&ctx, output);

   get_contract_return result;
   result.example = true;
   return result;
}

DEFINE_API_IMPL( contract_api_impl, list_owner_contracts )
{
   list_owner_contracts_return result;

   const auto& idx = _db.get_index< chain::contract_index, chain::by_owner >();
   auto itr = idx.lower_bound( args.owner );
   auto end = idx.end();
   wlog("!!!!!! LIST_OWNER_CONTRACTS");
   while( itr != end )
   {
      if (itr->owner != args.owner) break;
      wlog("!!!!!! LIST_OWNER_CONTRACTS ${w}", ("w",itr->owner));
      result.contracts.push_back(*itr);
      ++itr;
   }

   return result;
}

machine::chain_adapter make_chain_adapter(chain::database& _db)
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

  std::function< std::string(uint64_t) > get_block_hash = [](uint64_t block_num) -> std::string
  {
    return "";
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

  std::function< bool(std::string, std::string, machine::big_word) > set_storage = [](std::string destination, std::string key, machine::big_word value) -> bool
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

DEFINE_API_IMPL( contract_api_impl, invoke )
{
   // TODO: Temporary
   machine::message msg = {};
   ilog( "machine::message msg.flags ${f}", ("f",msg.flags) );

   const bool is_debug = true;
   const uint64_t block_timestamp = static_cast<uint64_t>( _db.head_block_time().sec_since_epoch() );
   const uint64_t block_number = _db.head_block_num();
   const uint64_t block_difficulty = static_cast<uint64_t>( _db.get_pow_summary_target() );
   const uint64_t block_energylimit = 0;
   const uint64_t tx_energyprice = 0;
   std::string tx_origin = "XGT0000000000000000000000000000000000000000";
   std::string block_coinbase = "XGT0000000000000000000000000000000000000000";

   // machine::context ctx = {true, 0x5c477758};
   machine::context ctx = {
     is_debug,
     block_timestamp,
     block_number,
     block_difficulty,
     block_energylimit,
     tx_energyprice,
     tx_origin,
     block_coinbase
   };

   std::vector<machine::word> code = {0x31, 0x00};

   machine::chain_adapter adapter = make_chain_adapter(_db);
   // machine::machine m(ctx, args.code, msg, adapter);
   machine::machine m(ctx, code, msg, adapter);

   std::string init_miner = "XGT0000000000000000000000000000000000000000";
   m.push_string(init_miner);

   m.print_stack();

   std::string line;
   while (m.is_running())
   {
     std::cerr << "step\n";
     m.step();
     // Print out any logging that was generated
     while ( std::getline(m.get_logger(), line) )
       std::cerr << "\e[36m" << "LOG: " << line << "\e[0m" << std::endl;
   }
   while ( std::getline(m.get_logger(), line) )
     std::cerr << "\e[36m" << "LOG: " << line << "\e[0m" << std::endl;
   std::cout << m.to_json() << std::endl;

   invoke_return result;

   return result;
}

} // detail

contract_api::contract_api(): my( new detail::contract_api_impl() )
{
   JSON_RPC_REGISTER_API( XGT_CONTRACT_API_PLUGIN_NAME );
}

contract_api::~contract_api() {}

DEFINE_LOCKLESS_APIS( contract_api,
   (get_contract)
   (list_owner_contracts)
   (invoke)
)

} } } //xgt::plugins::contract
