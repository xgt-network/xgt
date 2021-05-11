#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/protocol/xgt_operations.hpp>

#include <xgt/plugins/wallet_by_key_api/wallet_by_key_api_plugin.hpp>
#include <xgt/plugins/wallet_by_key_api/wallet_by_key_api.hpp>

#include <xgt/plugins/wallet_by_key/wallet_by_key_objects.hpp>

namespace xgt { namespace plugins { namespace wallet_by_key {

namespace detail {

class wallet_by_key_api_impl
{
   public:
      wallet_by_key_api_impl() : _db( appbase::app().get_plugin< xgt::plugins::chain::chain_plugin >().db() ) {}

      get_key_references_return get_key_references( const get_key_references_args& args )const;
      generate_wallet_name_return generate_wallet_name( const generate_wallet_name_args& args)const;

      chain::database& _db;
};

get_key_references_return wallet_by_key_api_impl::get_key_references( const get_key_references_args& args )const
{
   get_key_references_return final_result;
   final_result.wallets.reserve( args.keys.size() );

   const auto& key_idx = _db.get_index< wallet_by_key::key_lookup_index >().indices().get< wallet_by_key::by_key >();

   for( auto& key : args.keys )
   {
      vector< xgt::protocol::wallet_name_type > result;
      auto lookup_itr = key_idx.lower_bound( key );

      while( lookup_itr != key_idx.end() && lookup_itr->key == key )
      {
         result.push_back( lookup_itr->account );
         ++lookup_itr;
      }

      final_result.wallets.emplace_back( std::move( result ) );
   }

   return final_result;
}

generate_wallet_name_return wallet_by_key_api_impl::generate_wallet_name( const generate_wallet_name_args& args )const
{
   string wallet_name = wallet_create_operation::get_wallet_name(args.recovery_keys);
   generate_wallet_name_return final_result = {wallet_name};
   return final_result;
}

} // detail

wallet_by_key_api::wallet_by_key_api(): my( new detail::wallet_by_key_api_impl() )
{
   JSON_RPC_REGISTER_API( XGT_WALLET_BY_KEY_API_PLUGIN_NAME );
}

wallet_by_key_api::~wallet_by_key_api() {}

DEFINE_READ_APIS( wallet_by_key_api,
   (get_key_references)
   (generate_wallet_name)
)

} } } // xgt::plugins::wallet_by_key
