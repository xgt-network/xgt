/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 */
#pragma once

#include <xgt/chain/block_log.hpp>
#include <xgt/chain/fork_database.hpp>
#include <xgt/chain/global_property_object.hpp>
#include <xgt/chain/hardfork_property_object.hpp>
#include <xgt/chain/node_property_object.hpp>
#include <xgt/chain/notifications.hpp>
#include <xgt/chain/contract_objects.hpp>
#include <xgt/chain/machine.hpp>

#include <xgt/chain/util/advanced_benchmark_dumper.hpp>
#include <xgt/chain/util/signal.hpp>

#include <xgt/protocol/protocol.hpp>
#include <xgt/protocol/hardfork.hpp>

#include <appbase/plugin.hpp>

#include <fc/signals.hpp>

#include <fc/log/logger.hpp>

#include <functional>
#include <map>

namespace xgt { namespace chain {

   using xgt::protocol::signed_transaction;
   using xgt::protocol::operation;
   using xgt::protocol::authority;
   using xgt::protocol::asset;
   using xgt::protocol::asset_symbol_type;
   using xgt::protocol::price;
   using abstract_plugin = appbase::abstract_plugin;

   struct hardfork_versions
   {
      fc::time_point_sec         times[ XGT_NUM_HARDFORKS + 1 ];
      protocol::hardfork_version versions[ XGT_NUM_HARDFORKS + 1 ];
   };

   class database;

   struct index_delegate {
   };

   using index_delegate_map = std::map< std::string, index_delegate >;

   class database_impl;
   class custom_operation_interpreter;
   class custom_operation_notification;

   namespace util {
      class advanced_benchmark_dumper;
   }

   struct reindex_notification;

   struct generate_optional_actions_notification {};

   /**
    *   @class database
    *   @brief tracks the blockchain state in an extensible manner
    */
   class database : public chainbase::database
   {
      public:
         database();
         ~database();

         bool is_producing()const { return _is_producing; }
         void set_producing( bool p ) { _is_producing = p;  }

         bool is_pending_tx()const { return _is_pending_tx; }
         void set_pending_tx( bool p ) { _is_pending_tx = p; }

         bool _is_producing = false;
         bool _is_pending_tx = false;

         bool _log_hardforks = true;

         enum validation_steps
         {
            skip_nothing                = 0,
            skip_witness_signature      = 1 << 0,  ///< used while reindexing
            skip_transaction_signatures = 1 << 1,  ///< used by non-witness nodes
            skip_transaction_dupe_check = 1 << 2,  ///< used while reindexing
            skip_fork_db                = 1 << 3,  ///< used while reindexing
            skip_block_size_check       = 1 << 4,  ///< used when applying locally generated transactions
            skip_tapos_check            = 1 << 5,  ///< used while reindexing -- note this skips expiration check as well
            skip_authority_check        = 1 << 6,  ///< used while reindexing -- disables any checking of authority on transactions
            skip_merkle_check           = 1 << 7,  ///< used while reindexing
            skip_undo_history_check     = 1 << 8,  ///< used while reindexing
            skip_witness_schedule_check = 1 << 9,  ///< used while reindexing
            skip_validate               = 1 << 10, ///< used prior to checkpoint, skips validate() call on transaction
            skip_validate_invariants    = 1 << 11, ///< used to skip database invariant check on block application
            skip_undo_block             = 1 << 12, ///< used to skip undo db on reindex
            skip_block_log              = 1 << 13  ///< used to skip block logging on reindex
         };

         typedef std::function<void(uint32_t, const abstract_index_cntr_t&)> TBenchmarkMidReport;
         typedef std::pair<uint32_t, TBenchmarkMidReport> TBenchmark;

         struct open_args
         {
            fc::path data_dir;
            fc::path blockchain_dir;
            uint64_t initial_supply = XGT_INIT_SUPPLY;
            uint32_t chainbase_flags = 0;
            bool do_validate_invariants = false;
            bool benchmark_is_enabled = false;
            fc::variant database_cfg;

            std::shared_ptr< std::function< void( database&, const open_args& ) > > genesis_func;

            // The following fields are only used on reindexing
            uint32_t stop_at_block = 0;
            TBenchmark benchmark = TBenchmark(0, []( uint32_t, const abstract_index_cntr_t& ){});
         };

         /**
          * @brief Open a database, creating a new one if necessary
          *
          * Opens a database in the specified directory. If no initialized database is found the database
          * will be initialized with the default state.
          *
          * @param data_dir Path to open or create database in
          */
         void open( const open_args& args );

         /**
          * @brief Rebuild object graph from block history and open detabase
          *
          * This method may be called after or instead of @ref database::open, and will rebuild the object graph by
          * replaying blockchain history. When this method exits successfully, the database will be open.
          *
          * @return the last replayed block number.
          */
         uint32_t reindex( const open_args& args );

         /**
          * @brief wipe Delete database from disk, and potentially the raw chain as well.
          * @param include_blocks If true, delete the raw chain as well as the database.
          *
          * Will close the database before wiping. Database will be closed when this function returns.
          */
         void wipe(const fc::path& data_dir, const fc::path& blockchain_dir, bool include_blocks);
         void close(bool rewind = true);

         //////////////////// db_block.cpp ////////////////////

         /**
          *  @return true if the block is in our fork DB or saved to disk as
          *  part of the official chain, otherwise return false
          */
         bool                       is_known_block( const block_id_type& id )const;
         bool                       is_known_transaction( const transaction_id_type& id )const;
         fc::sha256                 get_pow_target()const;
         uint32_t                   get_pow_summary_target()const;
         block_id_type              find_block_id_for_num( uint32_t block_num )const;
         block_id_type              get_block_id_for_num( uint32_t block_num )const;
         optional<signed_block>     fetch_block_by_id( const block_id_type& id )const;
         optional<signed_block>     fetch_block_by_number( uint32_t num )const;
         const signed_transaction   get_recent_transaction( const transaction_id_type& trx_id )const;
         std::vector<block_id_type> get_block_ids_on_fork(block_id_type head_of_fork) const;

         chain_id_type xgt_chain_id = XGT_CHAIN_ID;
         chain_id_type get_chain_id() const;
         void set_chain_id( const chain_id_type& chain_id );

         std::set< wallet_name_type > witnesses;
         std::set< wallet_name_type > get_witnesses();
         void set_witnesses( std::set< wallet_name_type > ws );

         /** Allows to visit all stored blocks until processor returns true. Caller is responsible for block disasembling
          * const signed_block_header& - header of previous block
          * const signed_block& - block to be processed currently
         */
         void foreach_block(std::function<bool(const signed_block_header&, const signed_block&)> processor) const;

         /// Allows to process all blocks visit all transactions held there until processor returns true.
         void foreach_tx(std::function<bool(const signed_block_header&, const signed_block&,
            const signed_transaction&, uint32_t)> processor) const;
         /// Allows to process all operations held in blocks and transactions until processor returns true.
         void foreach_operation(std::function<bool(const signed_block_header&, const signed_block&,
            const signed_transaction&, uint32_t, const operation&, uint16_t)> processor) const;

         const witness_object&  get_witness(  const wallet_name_type& name )const;
         const witness_object*  find_witness( const wallet_name_type& name )const;

         const wallet_object&  get_account(  const wallet_name_type& name )const;
         const wallet_object*  find_account( const wallet_name_type& name )const;

         const comment_object&  get_comment(  const wallet_name_type& author, const std::string& permlink )const;
         const comment_object*  find_comment( const wallet_name_type& author, const std::string& permlink )const;

         const escrow_object&   get_escrow(  const wallet_name_type& name, uint32_t escrow_id )const;
         const escrow_object*   find_escrow( const wallet_name_type& name, uint32_t escrow_id )const;

         //const contract_object& get_contract( const contract_hash_type& contract_hash )const;

         const dynamic_global_property_object&  get_dynamic_global_properties()const;
         const node_property_object&            get_node_properties()const;
         const hardfork_property_object&        get_hardfork_property_object()const;

         void max_bandwidth_per_share()const;

         /**
          *  Calculate the percent of block production slots that were missed in the
          *  past 128 blocks, not including the current block.
          */
         uint32_t witness_participation_rate()const;

         void                                   add_checkpoints( const flat_map<uint32_t,block_id_type>& checkpts );
         const flat_map<uint32_t,block_id_type> get_checkpoints()const { return _checkpoints; }
         bool                                   before_last_checkpoint()const;

         bool push_block( const signed_block& b, uint32_t skip = skip_nothing );
         void push_transaction( const signed_transaction& trx, uint32_t skip = skip_nothing );
         void _maybe_warn_multiple_production( uint32_t height )const;
         bool _push_block( const signed_block& b );
         void _push_transaction( const signed_transaction& trx );

         void pop_block();
         void clear_pending();

         void push_virtual_operation( const operation& op );
         void pre_push_virtual_operation( const operation& op );
         void post_push_virtual_operation( const operation& op );

         /*
          * Pushing an action without specifying an execution time will execute at head block.
          * The execution time must be greater than or equal to head block.
          */
         void push_required_action( const required_automated_action& a, time_point_sec execution_time );
         void push_required_action( const required_automated_action& a );

         void push_optional_action( const optional_automated_action& a, time_point_sec execution_time );
         void push_optional_action( const optional_automated_action& a );

         void notify_pre_apply_required_action( const required_action_notification& note );
         void notify_post_apply_required_action( const required_action_notification& note );

         void notify_pre_apply_optional_action( const optional_action_notification& note );
         void notify_post_apply_optional_action( const optional_action_notification& note );
         /**
          *  This method is used to track applied operations during the evaluation of a block, these
          *  operations should include any operation actually included in a transaction as well
          *  as any implied/virtual operations that resulted, such as filling an order.
          *  The applied operations are cleared after post_apply_operation.
          */
         void notify_pre_apply_operation( const operation_notification& note );
         void notify_post_apply_operation( const operation_notification& note );
         void notify_pre_apply_block( const block_notification& note );
         void notify_post_apply_block( const block_notification& note );
         void notify_irreversible_block( uint32_t block_num );
         void notify_pre_apply_transaction( const transaction_notification& note );
         void notify_post_apply_transaction( const transaction_notification& note );
         void notify_pre_apply_custom_operation( const custom_operation_notification& note );
         void notify_post_apply_custom_operation( const custom_operation_notification& note );

         using apply_required_action_handler_t = std::function< void(const required_action_notification&) >;
         using apply_optional_action_handler_t = std::function< void(const optional_action_notification&) >;
         using apply_operation_handler_t = std::function< void(const operation_notification&) >;
         using apply_transaction_handler_t = std::function< void(const transaction_notification&) >;
         using apply_block_handler_t = std::function< void(const block_notification&) >;
         using apply_custom_operation_handler_t = std::function< void(const custom_operation_notification&) >;
         using irreversible_block_handler_t = std::function< void(uint32_t) >;
         using reindex_handler_t = std::function< void(const reindex_notification&) >;
         using generate_optional_actions_handler_t = std::function< void(const generate_optional_actions_notification&) >;


      private:
         template <typename TSignal,
                   typename TNotification = std::function<typename TSignal::signature_type>>
         boost::signals2::connection connect_impl( TSignal& signal, const TNotification& func,
            const abstract_plugin& plugin, int32_t group, const std::string& item_name = "" );

         template< bool IS_PRE_OPERATION >
         boost::signals2::connection any_apply_operation_handler_impl( const apply_operation_handler_t& func,
            const abstract_plugin& plugin, int32_t group );

      public:

         boost::signals2::connection add_pre_apply_required_action_handler  ( const apply_required_action_handler_t&     func, const abstract_plugin& plugin, int32_t group = -1 );
         boost::signals2::connection add_post_apply_required_action_handler ( const apply_required_action_handler_t&     func, const abstract_plugin& plugin, int32_t group = -1 );
         boost::signals2::connection add_pre_apply_optional_action_handler  ( const apply_optional_action_handler_t&     func, const abstract_plugin& plugin, int32_t group = -1 );
         boost::signals2::connection add_post_apply_optional_action_handler ( const apply_optional_action_handler_t&     func, const abstract_plugin& plugin, int32_t group = -1 );
         boost::signals2::connection add_pre_apply_operation_handler        ( const apply_operation_handler_t&           func, const abstract_plugin& plugin, int32_t group = -1 );
         boost::signals2::connection add_post_apply_operation_handler       ( const apply_operation_handler_t&           func, const abstract_plugin& plugin, int32_t group = -1 );
         boost::signals2::connection add_pre_apply_transaction_handler      ( const apply_transaction_handler_t&         func, const abstract_plugin& plugin, int32_t group = -1 );
         boost::signals2::connection add_post_apply_transaction_handler     ( const apply_transaction_handler_t&         func, const abstract_plugin& plugin, int32_t group = -1 );
         boost::signals2::connection add_pre_apply_custom_operation_handler ( const apply_custom_operation_handler_t&    func, const abstract_plugin& plugin, int32_t group = -1 );
         boost::signals2::connection add_post_apply_custom_operation_handler( const apply_custom_operation_handler_t&    func, const abstract_plugin& plugin, int32_t group = -1 );
         boost::signals2::connection add_pre_apply_block_handler            ( const apply_block_handler_t&               func, const abstract_plugin& plugin, int32_t group = -1 );
         boost::signals2::connection add_post_apply_block_handler           ( const apply_block_handler_t&               func, const abstract_plugin& plugin, int32_t group = -1 );
         boost::signals2::connection add_irreversible_block_handler         ( const irreversible_block_handler_t&        func, const abstract_plugin& plugin, int32_t group = -1 );
         boost::signals2::connection add_pre_reindex_handler                ( const reindex_handler_t&                   func, const abstract_plugin& plugin, int32_t group = -1 );
         boost::signals2::connection add_post_reindex_handler               ( const reindex_handler_t&                   func, const abstract_plugin& plugin, int32_t group = -1 );
         boost::signals2::connection add_generate_optional_actions_handler  ( const generate_optional_actions_handler_t& func, const abstract_plugin& plugin, int32_t group = -1 );

         void        adjust_balance( const wallet_object& a, const asset& delta );
         void        adjust_balance( const wallet_name_type& name, const asset& delta );
         void        adjust_supply( const asset& delta );
         void        update_recovery_authority( const wallet_object& account, const authority& recovery_authority );

         asset       get_balance( const wallet_object& a, asset_symbol_type symbol )const;
         asset       get_balance( const wallet_name_type& aname, asset_symbol_type symbol )const;

         void account_recovery_processing();
         void expire_escrow_ratification();

         time_point_sec   head_block_time()const;
         uint32_t         head_block_num()const;
         block_id_type    head_block_id()const;
         fc::optional<block_id_type> previous_block_id()const;

         node_property_object& node_properties();

         uint32_t last_non_undoable_block_num() const;
         //////////////////// db_init.cpp ////////////////////

         void initialize_evaluators();
         void register_custom_operation_interpreter( std::shared_ptr< custom_operation_interpreter > interpreter );
         std::shared_ptr< custom_operation_interpreter > get_custom_json_evaluator( const custom_id_type& id );

         /// Reset the object graph in-memory
         void initialize_indexes();
         void init_genesis(uint64_t initial_supply = XGT_INIT_SUPPLY );

         /**
          *  This method validates transactions without adding it to the pending state.
          *  @throw if an error occurs
          */
         void validate_transaction( const signed_transaction& trx );

         /** when popping a block, the transactions that were removed get cached here so they
          * can be reapplied at the proper time */
         std::deque< signed_transaction >       _popped_tx;
         vector< signed_transaction >           _pending_tx;

         void retally_comment_children();

         void update_global_dynamic_data( const signed_block& b );

         bool has_hardfork( uint32_t hardfork )const;

         uint32_t get_hardfork()const;

         /* For testing and debugging only. Given a hardfork
            with id N, applies all hardforks with id <= N */
         void set_hardfork( uint32_t hardfork, bool process_now = true );

         void validate_invariants()const;
         /**
          * @}
          */

         const std::string& get_json_schema() const;

         void set_flush_interval( uint32_t flush_blocks );
         void check_free_memory( bool force_print, uint32_t current_block_num );

         void process_energy( const signed_transaction& trx );

         void apply_transaction( const signed_transaction& trx, uint32_t skip = skip_nothing );
         void apply_required_action( const required_automated_action& a );
         void apply_optional_action( const optional_automated_action& a );

         optional< chainbase::database::session >& pending_transaction_session();

         void set_index_delegate( const std::string& n, index_delegate&& d );
         const index_delegate& get_index_delegate( const std::string& n );
         bool has_index_delegate( const std::string& n );
         const index_delegate_map& index_delegates();

         ///Smart Media Tokens related methods
         ///@{
         void validate_xtt_invariants()const;
         ///@}

   protected:
         //Mark pop_undo() as protected -- we do not want outside calling pop_undo(); it should call pop_block() instead
         //void pop_undo() { object_database::pop_undo(); }

      private:
         optional< chainbase::database::session > _pending_tx_session;

         void apply_block( const signed_block& next_block, uint32_t skip = skip_nothing );
         void _apply_block( const signed_block& next_block );
         void _apply_transaction( const signed_transaction& trx );
         void apply_operation( const operation& op );

         void process_required_actions( const required_automated_actions& actions );
         void process_optional_actions( const optional_automated_actions& actions );

         ///Steps involved in applying a new block
         ///@{

         const witness_object& validate_block_header( uint32_t skip, const signed_block& next_block )const;
         void create_block_summary(const signed_block& next_block);

         void clear_null_wallet_balance();

         void update_signing_witness(const witness_object& signing_witness, const signed_block& new_block);
         void update_last_irreversible_block();
         void migrate_irreversible_state();
         void clear_expired_transactions();
         void process_header_extensions( const signed_block& next_block, required_automated_actions& req_actions, optional_automated_actions& opt_actions );

         void generate_required_actions();
         void generate_optional_actions();

         void init_hardforks();
         void process_hardforks();
         void apply_hardfork( uint32_t hardfork );

         ///@}
         template< typename xtt_balance_object_type, class balance_operator_type >
         void adjust_xtt_balance( const wallet_name_type& name, const asset& delta, bool check_account,
                                  balance_operator_type balance_operator );
         void modify_balance( const wallet_object& a, const asset& delta, bool check_balance );

         operation_notification create_operation_notification( const operation& op )const
         {
            operation_notification note(op);
            note.trx_id       = _current_trx_id;
            note.block        = _current_block_num;
            note.trx_in_block = _current_trx_in_block;
            note.op_in_trx    = _current_op_in_trx;
            return note;
         }

      public:

         const transaction_id_type& get_current_trx() const
         {
            return _current_trx_id;
         }
         uint16_t get_current_op_in_trx() const
         {
            return _current_op_in_trx;
         }

         util::advanced_benchmark_dumper& get_benchmark_dumper()
         {
            return _benchmark_dumper;
         }

         const hardfork_versions& get_hardfork_versions()
         {
            return _hardfork_versions;
         }

      private:

         std::unique_ptr< database_impl > _my;

         fork_database                 _fork_db;
         hardfork_versions             _hardfork_versions;

         block_log                     _block_log;

         // this function needs access to _plugin_index_signal
         template< typename MultiIndexType >
         friend void add_plugin_index( database& db );

         transaction_id_type           _current_trx_id;
         uint32_t                      _current_block_num    = 0;
         int32_t                       _current_trx_in_block = 0;
         uint16_t                      _current_op_in_trx    = 0;
         uint16_t                      _current_virtual_op   = 0;

         flat_map<uint32_t,block_id_type>  _checkpoints;

         node_property_object              _node_property_object;

         uint32_t                      _flush_blocks = 0;
         uint32_t                      _next_flush_block = 0;

         flat_map< custom_id_type, std::shared_ptr< custom_operation_interpreter > >   _custom_operation_interpreters;
         std::string                   _json_schema;

         util::advanced_benchmark_dumper  _benchmark_dumper;
         index_delegate_map            _index_delegate_map;

         fc::signal<void(const required_action_notification&)> _pre_apply_required_action_signal;
         fc::signal<void(const required_action_notification&)> _post_apply_required_action_signal;

         fc::signal<void(const optional_action_notification&)> _pre_apply_optional_action_signal;
         fc::signal<void(const optional_action_notification&)> _post_apply_optional_action_signal;

         fc::signal<void(const operation_notification&)>       _pre_apply_operation_signal;
         /**
          *  This signal is emitted for plugins to process every operation after it has been fully applied.
          */
         fc::signal<void(const operation_notification&)>       _post_apply_operation_signal;

         fc::signal<void(const custom_operation_notification&)> _pre_apply_custom_operation_signal;
         fc::signal<void(const custom_operation_notification&)> _post_apply_custom_operation_signal;

         /**
          *  This signal is emitted when we start processing a block.
          *
          *  You may not yield from this callback because the blockchain is holding
          *  the write lock and may be in an "inconstant state" until after it is
          *  released.
          */
         fc::signal<void(const block_notification&)>           _pre_apply_block_signal;

         fc::signal<void(uint32_t)>                            _on_irreversible_block;

         /**
          *  This signal is emitted after all operations and virtual operation for a
          *  block have been applied but before the get_applied_operations() are cleared.
          *
          *  You may not yield from this callback because the blockchain is holding
          *  the write lock and may be in an "inconstant state" until after it is
          *  released.
          */
         fc::signal<void(const block_notification&)>           _post_apply_block_signal;

         /**
          * This signal is emitted any time a new transaction is about to be applied
          * to the chain state.
          */
         fc::signal<void(const transaction_notification&)>     _pre_apply_transaction_signal;

         /**
          * This signal is emitted any time a new transaction has been applied to the
          * chain state.
          */
         fc::signal<void(const transaction_notification&)>     _post_apply_transaction_signal;

         /**
          * Emitted when reindexing starts
          */
         fc::signal<void(const reindex_notification&)>         _pre_reindex_signal;

         /**
          * Emitted when reindexing finishes
          */
         fc::signal<void(const reindex_notification&)>         _post_reindex_signal;

         fc::signal<void(const generate_optional_actions_notification& )> _generate_optional_actions_signal;

         /**
          *  Emitted After a block has been applied and committed.  The callback
          *  should not yield and should execute quickly.
          */
         //fc::signal<void(const vector< object_id_type >&)> changed_objects;

         /** this signal is emitted any time an object is removed and contains a
          * pointer to the last value of every object that was removed.
          */
         //fc::signal<void(const vector<const object*>&)>  removed_objects;

         /**
          * Internal signal to execute deferred registration of plugin indexes.
          */
         fc::signal<void()>                                    _plugin_index_signal;
   };

   struct reindex_notification
   {
      reindex_notification( const database::open_args& a ) : args( a ) {}

      bool reindex_success = false;
      uint32_t last_block_number = 0;
      const database::open_args& args;
   };

} }
