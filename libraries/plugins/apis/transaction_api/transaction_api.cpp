#include <xgt/plugins/transaction_api/transaction_api.hpp>
#include <xgt/plugins/transaction_api/transaction_api_plugin.hpp>

#include <xgt/plugins/database_api/database_api_plugin.hpp>
#include <xgt/plugins/block_api/block_api_plugin.hpp>

#include <xgt/plugins/transaction_status/transaction_status_objects.hpp>

#include <appbase/application.hpp>

namespace xgt { namespace plugins { namespace transaction_api {

namespace detail
{
   class transaction_api_impl
   {
      public:
         transaction_api_impl() :
            _p2p( appbase::app().get_plugin< xgt::plugins::p2p::p2p_plugin >() ),
            _chain( appbase::app().get_plugin< xgt::plugins::chain::chain_plugin >() ),
            _tsp( appbase::app().get_plugin< xgt::plugins::transaction_status::transaction_status_plugin >() )
         {
         }

         DECLARE_API_IMPL(
            (get_transaction_hex)
            (broadcast_transaction)
            (broadcast_block)
	    (find_transaction)
         )

         void on_post_apply_block( const signed_block& b );

         xgt::plugins::p2p::p2p_plugin&                 _p2p;
         xgt::plugins::chain::chain_plugin&             _chain;
         std::shared_ptr< database_api::database_api >  _database_api;
         transaction_status::transaction_status_plugin& _tsp;
   };

   DEFINE_API_IMPL( transaction_api_impl, get_transaction_hex )
   {
      return _database_api->get_transaction_hex( { args[0].as< signed_transaction >() }).hex;
   }

   DEFINE_API_IMPL( transaction_api_impl, broadcast_transaction )
   {
      signed_transaction trx = signed_transaction( args[0].as< signed_transaction >() );
      _chain.accept_transaction( trx );
      _p2p.broadcast_transaction( trx );

      return broadcast_transaction_return(trx.id());
   }

   DEFINE_API_IMPL( transaction_api_impl, broadcast_block )
   {
      _chain.accept_block( args.block, /*currently syncing*/ false, /*skip*/ chain::database::skip_nothing );
      _p2p.broadcast_block( args.block );
      return broadcast_block_return();
   }


   DEFINE_API_IMPL( transaction_api_impl, find_transaction )
   {
      auto &_db = _chain.db();
      uint32_t earliest_tracked_block_num = _tsp.earliest_tracked_block_num();

      // Have we begun tracking?
      if ( _db.head_block_num() >= earliest_tracked_block_num )
      {
         auto last_irreversible_block_num = _db.get_dynamic_global_properties().last_irreversible_block_num;
         auto tso = _db.find< transaction_status::transaction_status_object, transaction_status::by_trx_id >( args.transaction_id );

         // If we are actively tracking this transaction
         if ( tso != nullptr)
         {
            // If we're not within a block
            if ( !tso->block_num )
               return {
                  .status = transaction_status::within_mempool
               };

            // If we're in an irreversible block
            if ( tso->block_num <= last_irreversible_block_num )
               return {
                  .status = transaction_status::within_irreversible_block,
                  .block_num = tso->block_num
               };
            // We're in a reversible block
            else
               return {
                  .status = transaction_status::within_reversible_block,
                  .block_num = tso->block_num
               };
         }

         // If the user has provided us with an expiration
         if ( args.expiration.valid() )
         {
            const auto& expiration = *args.expiration;

            // Check if the expiration is before our earliest tracked block plus maximum transaction expiration
            auto earliest_tracked_block = _db.fetch_block_by_number( earliest_tracked_block_num );
            if ( expiration < earliest_tracked_block->timestamp + XGT_MAX_TIME_UNTIL_EXPIRATION )
               return {
                  .status = transaction_status::too_old
               };

            // If the expiration is on or before our last irreversible block
            if ( last_irreversible_block_num > 0 )
            {
               auto last_irreversible_block = _db.fetch_block_by_number( last_irreversible_block_num );
               if ( expiration <= last_irreversible_block->timestamp )
                  return {
                     .status = transaction_status::expired_irreversible
                  };
            }
            // If our expiration is in the past
            if ( expiration < _db.head_block_time() )
               return {
                  .status = transaction_status::expired_reversible
               };
         }
      }

      // Either the user did not provide an expiration or it is not expired and we didn't hear about this transaction
      return { .status = transaction_status::unknown };
   }

   void transaction_api_impl::on_post_apply_block( const signed_block& b )
   { try {
   } FC_LOG_AND_RETHROW() }

} // detail

transaction_api::transaction_api() : my( new detail::transaction_api_impl() )
{
   JSON_RPC_REGISTER_API( XGT_TRANSACTION_API_PLUGIN_NAME );
}

transaction_api::~transaction_api() {}

void transaction_api::api_startup()
{
   auto database = appbase::app().find_plugin< database_api::database_api_plugin >();
   if( database != nullptr )
   {
      my->_database_api = database->api;
   }
}

DEFINE_LOCKLESS_APIS( transaction_api,
   (get_transaction_hex)
   (broadcast_transaction)
   (broadcast_block)
   (find_transaction)
)

} } } // xgt::plugins::transaction_api
