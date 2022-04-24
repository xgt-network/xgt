#pragma once

#include <xgt/chain/xgt_fwd.hpp>
#include <appbase/application.hpp>
#include <xgt/chain/database.hpp>
#include <xgt/plugins/chain/abstract_block_producer.hpp>

#include <boost/signals2.hpp>

#define XGT_CHAIN_PLUGIN_NAME "chain"

namespace xgt { namespace plugins { namespace chain {

namespace detail { class chain_plugin_impl; }

using std::unique_ptr;
using namespace appbase;
using namespace xgt::chain;

namespace bfs = boost::filesystem;

class chain_plugin : public plugin< chain_plugin >
{
public:
   APPBASE_PLUGIN_REQUIRES()

   chain_plugin();
   virtual ~chain_plugin();

   bfs::path state_storage_dir() const;

   static const std::string& name() { static std::string name = XGT_CHAIN_PLUGIN_NAME; return name; }

   virtual void set_program_options( options_description& cli, options_description& cfg ) override;
   virtual void plugin_initialize( const variables_map& options ) override;
   virtual void plugin_startup() override;
   virtual void plugin_shutdown() override;

   void report_state_options( const string& plugin_name, const fc::variant_object& opts );
   flat_map< string, fc::variant_object >& get_state_options() const;

   bool accept_block( const xgt::chain::signed_block& block, bool currently_syncing, uint32_t skip );
   void accept_transaction( const xgt::chain::signed_transaction& trx );

   xgt::chain::signed_block generate_block(
      const fc::time_point_sec when,
      const wallet_name_type& witness_recovery,
      const fc::ecc::private_key& block_signing_private_key,
      fc::optional< xgt::chain::signed_transaction > block_reward,
      uint32_t skip = database::skip_nothing
      );

   /**
    * Set a class to be called for block generation.
    *
    * This function must be called during abtract_plugin::plugin_initialize().
    * Calling this during abstract_plugin::plugin_startup() will be too late
    * and will not take effect.
    */
   void register_block_generator( const std::string& plugin_name, std::shared_ptr< abstract_block_producer > block_producer );

   bool block_is_on_preferred_chain( const xgt::chain::block_id_type& block_id );

   void check_time_in_block( const xgt::chain::signed_block& block );

   template< typename MultiIndexType >
   bool has_index() const
   {
      return db().has_index< MultiIndexType >();
   }

   template< typename MultiIndexType >
   const chainbase::generic_index< MultiIndexType >& get_index() const
   {
      return db().get_index< MultiIndexType >();
   }

   template< typename ObjectType, typename IndexedByType, typename CompatibleKey >
   const ObjectType* find( CompatibleKey&& key ) const
   {
      return db().find< ObjectType, IndexedByType, CompatibleKey >( key );
   }

   template< typename ObjectType >
   const ObjectType* find( chainbase::oid< ObjectType > key = chainbase::oid< ObjectType >() )
   {
      return db().find< ObjectType >( key );
   }

   template< typename ObjectType, typename IndexedByType, typename CompatibleKey >
   const ObjectType& get( CompatibleKey&& key ) const
   {
      return db().get< ObjectType, IndexedByType, CompatibleKey >( key );
   }

   template< typename ObjectType >
   const ObjectType& get( const chainbase::oid< ObjectType >& key = chainbase::oid< ObjectType >() )
   {
      return db().get< ObjectType >( key );
   }

   // Exposed for backwards compatibility. In the future, plugins should manage their own internal database
   database& db();
   const database& db() const;

   // Emitted when the blockchain is syncing/live.
   // This is to synchronize plugins that have the chain plugin as an optional dependency.
   boost::signals2::signal<void()> on_sync;

private:
   std::unique_ptr< detail::chain_plugin_impl > my;
};

} } } // xgt::plugins::chain
