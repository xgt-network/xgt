#include <xgt/chain/xgt_fwd.hpp>

#include <xgt/plugins/wallet_by_key/wallet_by_key_plugin.hpp>
#include <xgt/plugins/wallet_by_key/wallet_by_key_objects.hpp>

#include <xgt/chain/wallet_object.hpp>
#include <xgt/chain/database.hpp>
#include <xgt/chain/index.hpp>

namespace xgt { namespace plugins { namespace wallet_by_key {

namespace detail {

class wallet_by_key_plugin_impl
{
   public:
      wallet_by_key_plugin_impl( wallet_by_key_plugin& _plugin ) :
         _db( appbase::app().get_plugin< xgt::plugins::chain::chain_plugin >().db() ),
         _self( _plugin ) {}

      void on_pre_apply_operation( const operation_notification& note );
      void on_post_apply_operation( const operation_notification& note );
      void on_pre_apply_transaction( const transaction_notification& note );
      vector<wallet_name_type> get_key_references( const flat_set<public_key_type> key_set )const;
      void clear_cache();
      void cache_auths( const account_authority_object& a );
      void update_key_lookup( const account_authority_object& a );


      flat_set< public_key_type >   cached_keys;
      database&                     _db;
      wallet_by_key_plugin&         _self;
      boost::signals2::connection   _pre_apply_operation_conn;
      boost::signals2::connection   _post_apply_operation_conn;
      boost::signals2::connection   _pre_apply_transaction_conn;
};

struct pre_operation_visitor
{
   wallet_by_key_plugin_impl& _plugin;

   pre_operation_visitor( wallet_by_key_plugin_impl& plugin ) : _plugin( plugin ) {}

   typedef void result_type;

   template< typename T >
   void operator()( const T& )const {}

   void operator()( const wallet_create_operation& op )const
   {
      _plugin.clear_cache();
   }

   void operator()( const wallet_update_operation& op )const
   {
      _plugin.clear_cache();
      auto acct_itr = _plugin._db.find< account_authority_object, by_account >( op.wallet );
      if( acct_itr ) _plugin.cache_auths( *acct_itr );
   }

   void operator()( const recover_wallet_operation& op )const
   {
      _plugin.clear_cache();
      auto acct_itr = _plugin._db.find< account_authority_object, by_account >( op.account_to_recover );
      if( acct_itr ) _plugin.cache_auths( *acct_itr );
   }

   void operator()( const pow_operation& op )const
   {
      _plugin.clear_cache();
   }
};

struct energy_cost_visitor
{
   energy_cost_visitor( const chain::database& db ) : _db( db ) {}

   const chain::database& _db;

   typedef uint64_t result_type;

   template< typename T >
   uint64_t operator()( const T& )const { return 0; }

   uint64_t operator()( const transfer_operation& o )const
   {
      wlog("!!!!!! ENERGY OPERATION VISITOR TRANSFER OPERATION");
      return 1;
   }
};

struct pow_work_get_account_visitor
{
   typedef const wallet_name_type* result_type;

   template< typename WorkType >
   result_type operator()( const WorkType& work )const
   {
      return &work.input.worker_account;
   }
};

struct post_operation_visitor
{
   wallet_by_key_plugin_impl& _plugin;

   post_operation_visitor( wallet_by_key_plugin_impl& plugin ) : _plugin( plugin ) {}

   typedef void result_type;

   template< typename T >
   void operator()( const T& )const {}

   void operator()( const wallet_create_operation& op )const
   {
      auto acct_itr = _plugin._db.find< account_authority_object, by_account >( op.get_wallet_name() );
      if( acct_itr ) _plugin.update_key_lookup( *acct_itr );
   }

   void operator()( const wallet_update_operation& op )const
   {
      auto acct_itr = _plugin._db.find< account_authority_object, by_account >( op.wallet );
      if( acct_itr ) _plugin.update_key_lookup( *acct_itr );
   }

   void operator()( const recover_wallet_operation& op )const
   {
      auto acct_itr = _plugin._db.find< account_authority_object, by_account >( op.account_to_recover );
      if( acct_itr ) _plugin.update_key_lookup( *acct_itr );
   }

   void operator()( const pow_operation& op )const
   {
      const wallet_name_type* worker_account = op.work.visit( pow_work_get_account_visitor() );
      if( worker_account == nullptr )
         return;
      auto acct_itr = _plugin._db.find< account_authority_object, by_account >( *worker_account );
      if( acct_itr ) _plugin.update_key_lookup( *acct_itr );
   }

   void operator()( const hardfork_operation& op )const
   {
   }
};

void wallet_by_key_plugin_impl::clear_cache()
{
   cached_keys.clear();
}

void wallet_by_key_plugin_impl::cache_auths( const account_authority_object& a )
{
   for( const auto& item : a.recovery.key_auths )
      cached_keys.insert( item.first );
   for( const auto& item : a.money.key_auths )
      cached_keys.insert( item.first );
   for( const auto& item : a.social.key_auths )
      cached_keys.insert( item.first );
}

void wallet_by_key_plugin_impl::update_key_lookup( const account_authority_object& a )
{
   flat_set< public_key_type > new_keys;

   // Construct the set of keys in the account's authority
   for( const auto& item : a.recovery.key_auths )
      new_keys.insert( item.first );
   for( const auto& item : a.money.key_auths )
      new_keys.insert( item.first );
   for( const auto& item : a.social.key_auths )
      new_keys.insert( item.first );

   // For each key that needs a lookup
   for( const auto& key : new_keys )
   {
      // If the key was not in the authority, add it to the lookup
      if( cached_keys.find( key ) == cached_keys.end() )
      {
         auto lookup_itr = _db.find< key_lookup_object, by_key >( boost::make_tuple( key, a.account ) );

         if( lookup_itr == nullptr )
         {
            _db.create< key_lookup_object >( [&]( key_lookup_object& o )
            {
               o.key = key;
               o.account = a.account;
            });
         }
      }
      else
      {
         // If the key was already in the auths, remove it from the set so we don't delete it
         cached_keys.erase( key );
      }
   }

   // Loop over the keys that were in authority but are no longer and remove them from the lookup
   for( const auto& key : cached_keys )
   {
      auto lookup_itr = _db.find< key_lookup_object, by_key >( boost::make_tuple( key, a.account ) );

      if( lookup_itr != nullptr )
      {
         _db.remove( *lookup_itr );
      }
   }

   cached_keys.clear();
}

void wallet_by_key_plugin_impl::on_pre_apply_operation( const operation_notification& note )
{
   note.op.visit( pre_operation_visitor( *this ) );
}

void wallet_by_key_plugin_impl::on_post_apply_operation( const operation_notification& note )
{
   note.op.visit( post_operation_visitor( *this ) );
}

void wallet_by_key_plugin_impl::on_pre_apply_transaction( const transaction_notification& note )
{
   uint64_t energy_cost = 0;
   auto visitor = energy_cost_visitor(_db);
   for( const operation& op : note.transaction.operations )
   {
      uint64_t cost = op.visit( visitor );
      energy_cost += cost;
   }

   if (energy_cost == 0) {
      return;
   }

   const auto& gpo = _db.get_dynamic_global_properties();
   auto now = gpo.time.sec_since_epoch();

   flat_set<public_key_type> key_set = note.transaction.get_signature_keys(_db.get_chain_id(), fc::ecc::fc_canonical);
   vector<wallet_name_type> wallets = get_key_references(key_set);
   // TODO: Figure out how to divvy energy costs for multisig
   //uint64_t energy_share = energy_cost;

   for (auto& wallet_name : wallets)
   {
      const wallet_object* w = _db.find_account( wallet_name );
      if (w == nullptr)
      {
         // TODO: Keep an eye on this
         //ilog("Could not find wallet for whom to calculate energy costs for ${w}", ("w",wallet_name));
      }
      else
      {
         _db.modify( *w, [&]( xgt::chain::wallet_object& _wallet ) {
            int64_t value = _wallet.balance.amount.value;
            try
            {
               // TODO: Fail if not enough energy
               util::energybar_params params(value, XGT_VOTING_ENERGY_REGENERATION_SECONDS);
               util::energybar bar = _wallet.energybar;
               if (bar.last_update_time == 0) {
                  bar.last_update_time = now;
               }
               bar.regenerate_energy(params, now);
               bar.use_energy(energy_cost);
               _wallet.energybar = bar;
            }
            catch (fc::assert_exception &e)
            {
               // TODO: Keep an eye on this
               //ilog("Could not update energy bar for ${w}", ("w",wallet_name));
            }
         } );
      }
   }
}

vector<wallet_name_type> wallet_by_key_plugin_impl::get_key_references( const flat_set<public_key_type> key_set )const
{
   vector<wallet_name_type> wallets;
   for (auto& key : key_set) {
      wallets.push_back(_db.find< key_lookup_object, by_key >(  key )->account);
   }
   return wallets;
}

} // detail

wallet_by_key_plugin::wallet_by_key_plugin() {}
wallet_by_key_plugin::~wallet_by_key_plugin() {}

void wallet_by_key_plugin::set_program_options( options_description& cli, options_description& cfg ){}

void wallet_by_key_plugin::plugin_initialize( const boost::program_options::variables_map& options )
{
   my = std::make_unique< detail::wallet_by_key_plugin_impl >( *this );
   try
   {
      ilog( "Initializing wallet_by_key plugin" );
      chain::database& db = appbase::app().get_plugin< xgt::plugins::chain::chain_plugin >().db();

      my->_pre_apply_operation_conn = db.add_pre_apply_operation_handler( [&]( const operation_notification& note ){ my->on_pre_apply_operation( note ); }, *this, 0 );
      my->_post_apply_operation_conn = db.add_post_apply_operation_handler( [&]( const operation_notification& note ){ my->on_post_apply_operation( note ); }, *this, 0 );
      my->_pre_apply_transaction_conn = my->_db.add_pre_apply_transaction_handler( [&]( const transaction_notification& note ) { try { my->on_pre_apply_transaction( note ); } FC_LOG_AND_RETHROW() }, *this, 0 );

      XGT_ADD_PLUGIN_INDEX(db, key_lookup_index);
   }
   FC_CAPTURE_AND_RETHROW()
}

void wallet_by_key_plugin::plugin_startup() {}

void wallet_by_key_plugin::plugin_shutdown()
{
   chain::util::disconnect_signal( my->_pre_apply_operation_conn );
   chain::util::disconnect_signal( my->_post_apply_operation_conn );
   chain::util::disconnect_signal( my->_pre_apply_transaction_conn );
}

} } } // xgt::plugins::wallet_by_key
