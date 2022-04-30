#include <xgt/plugins/wallet_history_api/wallet_history_api_plugin.hpp>
#include <xgt/plugins/wallet_history_api/wallet_history_api.hpp>

#include <xgt/plugins/wallet_history_rocksdb/wallet_history_rocksdb_plugin.hpp>

namespace xgt { namespace plugins { namespace wallet_history {

namespace detail {

class abstract_wallet_history_api_impl
{
   public:
      abstract_wallet_history_api_impl() : _db( appbase::app().get_plugin< xgt::plugins::chain::chain_plugin >().db() ) {}
      virtual ~abstract_wallet_history_api_impl() {}

      virtual get_ops_in_block_return get_ops_in_block( const get_ops_in_block_args& ) = 0;
      virtual get_transaction_return get_transaction( const get_transaction_args& ) = 0;
      virtual get_wallet_history_return get_wallet_history( const get_wallet_history_args& ) = 0;
      virtual enum_virtual_ops_return enum_virtual_ops( const enum_virtual_ops_args& ) = 0;

      chain::database& _db;
};

class wallet_history_api_chainbase_impl : public abstract_wallet_history_api_impl
{
   public:
      wallet_history_api_chainbase_impl() : abstract_wallet_history_api_impl() {}
      ~wallet_history_api_chainbase_impl() {}

      get_ops_in_block_return get_ops_in_block( const get_ops_in_block_args& ) override;
      get_transaction_return get_transaction( const get_transaction_args& ) override;
      get_wallet_history_return get_wallet_history( const get_wallet_history_args& ) override;
      enum_virtual_ops_return enum_virtual_ops( const enum_virtual_ops_args& ) override;
};

DEFINE_API_IMPL( wallet_history_api_chainbase_impl, get_ops_in_block )
{
   return _db.with_read_lock( [&]()
   {
      const auto& idx = _db.get_index< chain::operation_index, chain::by_location >();
      auto itr = idx.lower_bound( args.block_num );

      get_ops_in_block_return result;

      while( itr != idx.end() && itr->block == args.block_num )
      {
         api_operation_object temp = *itr;
         if( !args.only_virtual || is_virtual_operation( temp.op ) )
            result.ops.emplace( std::move( temp ) );
         ++itr;
      }

      return result;
   });
}

DEFINE_API_IMPL( wallet_history_api_chainbase_impl, get_transaction )
{
#ifdef SKIP_BY_TX_ID
   FC_ASSERT( false, "This node's operator has disabled operation indexing by transaction_id" );
#else

   return _db.with_read_lock( [&]()
   {
      get_transaction_return result;

      const auto& idx = _db.get_index< chain::operation_index, chain::by_transaction_id >();
      auto itr = idx.lower_bound( args.id );
      if( itr != idx.end() && itr->trx_id == args.id )
      {
         auto blk = _db.fetch_block_by_number( itr->block );
         FC_ASSERT( blk.valid() );
         FC_ASSERT( blk->transactions.size() > itr->trx_in_block );
         result = blk->transactions[itr->trx_in_block];
         result.block_num       = itr->block;
         result.transaction_num = itr->trx_in_block;
      }
      else
      {
         FC_ASSERT( false, "Unknown Transaction ${t}", ("t",args.id) );
      }

      return result;
   });
#endif
}

DEFINE_API_IMPL( wallet_history_api_chainbase_impl, get_wallet_history )
{
   FC_ASSERT( args.limit <= 10000, "limit of ${l} is greater than maxmimum allowed", ("l",args.limit) );
   FC_ASSERT( args.start >= args.limit, "start must be greater than limit" );

   return _db.with_read_lock( [&]()
   {
      const auto& idx = _db.get_index< chain::account_history_index, chain::by_account >();
      auto itr = idx.lower_bound( boost::make_tuple( args.wallet, args.start ) );
      uint32_t n = 0;

      get_wallet_history_return result;
      while( true )
      {
         if( itr == idx.end() )
            break;
         if( itr->account != args.wallet )
            break;
         if( n >= args.limit )
            break;
         result.history[ itr->sequence ] = _db.get( itr->op );
         ++itr;
         ++n;
      }

      return result;
   });
}

DEFINE_API_IMPL( wallet_history_api_chainbase_impl, enum_virtual_ops )
{
   FC_ASSERT( false, "This API is not supported for wallet history backed by Chainbase" );
}

class wallet_history_api_rocksdb_impl : public abstract_wallet_history_api_impl
{
   public:
      wallet_history_api_rocksdb_impl() :
         abstract_wallet_history_api_impl(), _dataSource( appbase::app().get_plugin< xgt::plugins::wallet_history_rocksdb::wallet_history_rocksdb_plugin >() ) {}
      ~wallet_history_api_rocksdb_impl() {}

      get_ops_in_block_return get_ops_in_block( const get_ops_in_block_args& ) override;
      get_transaction_return get_transaction( const get_transaction_args& ) override;
      get_wallet_history_return get_wallet_history( const get_wallet_history_args& ) override;
      enum_virtual_ops_return enum_virtual_ops( const enum_virtual_ops_args& ) override;

      const wallet_history_rocksdb::wallet_history_rocksdb_plugin& _dataSource;
};

DEFINE_API_IMPL( wallet_history_api_rocksdb_impl, get_ops_in_block )
{
   get_ops_in_block_return result;
   _dataSource.find_operations_by_block(args.block_num,
      [&result, &args](const wallet_history_rocksdb::rocksdb_operation_object& op)
      {
         api_operation_object temp(op);
         if( !args.only_virtual || is_virtual_operation( temp.op ) )
            result.ops.emplace(std::move(temp));
      }
   );
   return result;
}

DEFINE_API_IMPL( wallet_history_api_rocksdb_impl, get_wallet_history )
{
   FC_ASSERT( args.limit <= 10000, "limit of ${l} is greater than maxmimum allowed", ("l",args.limit) );
   FC_ASSERT( args.start >= args.limit, "start must be greater than limit" );

   get_wallet_history_return result;

   _dataSource.find_wallet_history_data(args.wallet, args.start, args.limit,
      [&result](unsigned int sequence, const wallet_history_rocksdb::rocksdb_operation_object& op)
      {
         result.history[sequence] = api_operation_object( op );
      });

   return result;
}

DEFINE_API_IMPL( wallet_history_api_rocksdb_impl, get_transaction )
{
   FC_ASSERT( false, "This API is not supported for wallet history backed by RocksDB" );
}

DEFINE_API_IMPL( wallet_history_api_rocksdb_impl, enum_virtual_ops)
{
   enum_virtual_ops_return result;

   result.next_block_range_begin = _dataSource.enum_operations_from_block_range(args.block_range_begin,
      args.block_range_end,
      [&result](const wallet_history_rocksdb::rocksdb_operation_object& op)
      {
         result.ops.emplace_back(api_operation_object(op));
      }
   );

   return result;
}

} // detail

wallet_history_api::wallet_history_api()
{
   auto ah_rocks = appbase::app().find_plugin< xgt::plugins::wallet_history_rocksdb::wallet_history_rocksdb_plugin >();

   if( ah_rocks != nullptr )
   {
      my = std::make_unique< detail::wallet_history_api_rocksdb_impl >();
   }
   else
   {
      FC_ASSERT( false, "Account History API only works if the wallet_history_rocksdb plugin is enabled" );
   }

   JSON_RPC_REGISTER_API( XGT_WALLET_HISTORY_API_PLUGIN_NAME );
}

wallet_history_api::~wallet_history_api() {}

DEFINE_LOCKLESS_APIS( wallet_history_api ,
   (get_ops_in_block)
   (get_transaction)
   (get_wallet_history)
   (enum_virtual_ops)
)

} } } // xgt::plugins::wallet_history
