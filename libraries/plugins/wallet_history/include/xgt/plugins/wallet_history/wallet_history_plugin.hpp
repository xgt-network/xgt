#pragma once

#include <xgt/chain/xgt_fwd.hpp>
#include <xgt/plugins/wallet_history/wallet_history_objects.hpp>

#include <xgt/plugins/chain/chain_plugin.hpp>

#include <appbase/application.hpp>

#include <functional>
#include <memory>

namespace xgt {

namespace plugins { namespace wallet_history {

namespace bfs = boost::filesystem;



class wallet_history_plugin final : public appbase::plugin< wallet_history_plugin >
{
public:
   APPBASE_PLUGIN_REQUIRES((xgt::plugins::chain::chain_plugin))

   wallet_history_plugin();
   virtual ~wallet_history_plugin();

   static const std::string& name() { static std::string name = "wallet_history"; return name; }

   virtual void set_program_options(
      boost::program_options::options_description &command_line_options,
      boost::program_options::options_description &config_file_options
      ) override;

   virtual void plugin_initialize(const boost::program_options::variables_map& options) override;
   virtual void plugin_startup() override;
   virtual void plugin_shutdown() override;

   void find_wallet_history_data(const protocol::wallet_name_type& name, uint64_t start, uint32_t limit,
      std::function<void(unsigned int, const rocksdb_operation_object&)> processor) const;
   bool find_operation_object(size_t opId, rocksdb_operation_object* data) const;
   void find_operations_by_block(size_t blockNum,
      std::function<void(const rocksdb_operation_object&)> processor) const;
   uint32_t enum_operations_from_block_range(uint32_t blockRangeBegin, uint32_t blockRangeEnd,
      std::function<void(const rocksdb_operation_object&)> processor) const;

private:
   class impl;

   std::unique_ptr<impl> _my;
   uint32_t              _blockLimit = 0;
   bool                  _doImmediateImport = false;
};


} } } // xgt::plugins::wallet_history
