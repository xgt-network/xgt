#pragma once

#include <boost/any.hpp>
#include <boost/chrono.hpp>
#include <boost/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/throw_exception.hpp>
#include <boost/container/deque.hpp>
#include <boost/container/set.hpp>
#include <boost/container/map.hpp>

#include <chainbase/util/object_id.hpp>

#include <array>
#include <atomic>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <typeindex>
#include <typeinfo>

#ifndef CHAINBASE_NUM_RW_LOCKS
   #define CHAINBASE_NUM_RW_LOCKS 10
#endif

#define CHAINBASE_REQUIRE_READ_LOCK(m, t) require_read_lock(m, typeid(t).name())
#define CHAINBASE_REQUIRE_WRITE_LOCK(m, t) require_write_lock(m, typeid(t).name())

namespace helpers
{
   struct index_statistic_info
   {
      std::string _value_type_name;
      size_t      _item_count = 0;
      size_t      _item_sizeof = 0;
      /// Additional (ie dynamic container) allocations held in stored items
      size_t      _item_additional_allocation = 0;
      /// Additional memory used for container internal structures (like tree nodes).
      size_t      _additional_container_allocation = 0;
   };

   template <class IndexType>
   void gather_index_static_data(const IndexType& index, index_statistic_info* info)
   {
      info->_value_type_name = boost::core::demangle(typeid(typename IndexType::value_type).name());
      info->_item_count = index.size();
      info->_item_sizeof = sizeof(typename IndexType::value_type);
      info->_item_additional_allocation = 0;
   }

   template <class IndexType>
   class index_statistic_provider
   {
   public:
      index_statistic_info gather_statistics(const IndexType& index, bool onlyStaticInfo) const
      {
         index_statistic_info info;
         gather_index_static_data(index, &info);
         return info;
      }
   };
} /// namespace helpers

namespace chainbase {

   namespace bfs = boost::filesystem;
   using std::unique_ptr;
   using std::vector;

   enum open_flags
   {
      skip_nothing               = 0,
      skip_env_check             = 1 << 0 // Skip environment check on db open
   };

   struct strcmp_less
   {
      bool operator()( const std::string& a, const std::string& b )const
      {
         return less( a.c_str(), b.c_str() );
      }

      private:
         inline bool less( const char* a, const char* b )const
         {
            return std::strcmp( a, b ) < 0;
         }
   };

   template<uint16_t TypeNumber, typename Derived>
   struct object
   {
      typedef oid<Derived> id_type;
      static const uint16_t type_id = TypeNumber;

   };

   /** this class is ment to be specified to enable lookup of index type by object type using
    * the SET_INDEX_TYPE macro.
    **/
   template<typename T>
   struct get_index_type {};

   /**
    *  This macro must be used at global scope and OBJECT_TYPE and INDEX_TYPE must be fully qualified
    */
   #define CHAINBASE_SET_INDEX_TYPE( OBJECT_TYPE, INDEX_TYPE )  \
   namespace chainbase { template<> struct get_index_type<OBJECT_TYPE> { typedef INDEX_TYPE type; }; }

   template< typename value_type >
   class undo_state
   {
      public:
         typedef typename value_type::id_type                      id_type;

         undo_state(  )
         :old_values(  ),
          removed_values(  ),
          new_ids(  ){}

         typedef boost::container::map< id_type, value_type, std::less<id_type>>  id_value_type_map;
         typedef boost::container::set< id_type, std::less<id_type>>                    id_type_set;

         id_value_type_map            old_values;
         id_value_type_map            removed_values;
         id_type_set                  new_ids;
         id_type                      old_next_id = 0;
         int64_t                      revision = 0;
   };

   /**
    * The code we want to implement is this:
    *
    * ++target; try { ... } finally { --target }
    *
    * In C++ the only way to implement finally is to create a class
    * with a destructor, so that's what we do here.
    */
   class int_incrementer
   {
      public:
         int_incrementer( std::atomic<int32_t>& target ) : _target(target)
         { ++_target; }

         int_incrementer( int_incrementer& ii ) : _target( ii._target )
         { ++_target; }

         ~int_incrementer()
         { --_target; }

         int32_t get()const
         { return _target; }

      private:
         std::atomic<int32_t>& _target;
   };

   class counted_shared_lock {
      public:
      counted_shared_lock ( std::atomic<int32_t>& counter, boost::shared_mutex& mutex) : count(counter), mutex(mutex) {
         if (!mutex.try_lock_shared_for(boost::chrono::seconds(60))) {
            throw std::runtime_error("failed to acquire lock for 60s");
         }
         ++count;
      }
      ~counted_shared_lock() {
         --count;
         mutex.unlock_shared();
      }

      std::atomic<int32_t>& count;
      boost::shared_mutex& mutex;
   };

   class counted_lock {
      public:
      counted_lock ( std::atomic<int32_t>& counter, boost::shared_mutex& mutex) : count(counter), mutex(mutex) {
         if (!mutex.try_lock_for(boost::chrono::seconds(60))) {
            throw std::runtime_error("failed to acquire write lock for 60s");
         }
         ++count;
      }
      ~counted_lock() {
         --count;
         mutex.unlock();
      }

      std::atomic<int32_t>& count;
      boost::shared_mutex& mutex;
   };


   /**
    *  The value_type stored in the multiindex container must have a integer field with the name 'id'.  This will
    *  be the primary key and it will be assigned and managed by generic_index.
    */
   template<typename MultiIndexType>
   class generic_index
   {
      public:
         typedef MultiIndexType                                        index_type;
         typedef typename index_type::value_type                       value_type;
         typedef undo_state< value_type >                              undo_state_type;

         generic_index( bfs::path p )
         :_stack(),_indices( p ),_size_of_value_type( sizeof(typename MultiIndexType::value_type) ),_size_of_this(sizeof(*this))
         {
            _revision = _indices.revision();
         }

         generic_index( )
         :_stack(),_indices( ),_size_of_value_type( sizeof(typename MultiIndexType::value_type) ),_size_of_this(sizeof(*this))
         {
            _revision = _indices.revision();
         }

         void validate()const {
            if( sizeof(typename MultiIndexType::value_type) != _size_of_value_type || sizeof(*this) != _size_of_this )
               BOOST_THROW_EXCEPTION( std::runtime_error("content of memory does not match data expected by executable") );
         }

         /**
          * Construct a new element in the multi_index_container.
          * Set the ID to the next available ID, then increment _next_id and fire off on_create().
          */
         const value_type& emplace( const std::function <void (value_type&)>& f) {
            auto new_id = _next_id;

            value_type v{};
            v.id = new_id;
            f(v);
            auto insert_result = _indices.emplace( v );

            if( !insert_result.second ) {
               BOOST_THROW_EXCEPTION( std::logic_error("could not insert object, most likely a uniqueness constraint was violated") );
            }

            ++_next_id;
            _indices.set_next_id( _next_id );
            on_create( *insert_result.first );
            return *insert_result.first;
         }

         template<typename Modifier>
         void modify( const value_type& obj, Modifier&& m ) {
            on_modify( obj );
            auto ok = _indices.modify( _indices.iterator_to( obj ), m );
            if( !ok ) BOOST_THROW_EXCEPTION( std::logic_error( "Could not modify object, most likely a uniqueness constraint was violated" ) );
         }

         void remove( const value_type& obj ) {
            on_remove( obj );
            _indices.erase( _indices.iterator_to( obj ) );
         }

         template< typename ByIndex, typename IterType >
         IterType erase( IterType objI ) {
            on_remove( *objI );
            return _indices.template mutable_get< ByIndex >().erase( objI );
         }

         template<typename CompatibleKey>
         const value_type* find( CompatibleKey&& key )const {
            auto itr = _indices.find( std::forward<CompatibleKey>(key) );
            if( itr != _indices.end() ) return &*itr;
            return nullptr;
         }

         template<typename CompatibleKey>
         const value_type& get( CompatibleKey&& key )const {
            auto ptr = find( key );
            if( !ptr ) BOOST_THROW_EXCEPTION( std::out_of_range("key not found") );
            return *ptr;
         }

         index_type& mutable_indices() { return _indices; }

         const index_type& indices()const { return _indices; }

         void clear() { _indices.clear(); }

         void open( const bfs::path& p, const boost::any& o )
         {
            _indices.open( p, o );
            _revision = _indices.revision();
            typename value_type::id_type next_id = 0;
            if( _indices.get_metadata( "next_id", next_id ) )
            {
               _next_id = next_id;
            }
         }

         void close()
         {
            _indices.put_metadata( "next_id", _next_id );
            _indices.close();
         }

         void wipe( const bfs::path& dir ) { _indices.wipe( dir ); }

         void flush() { _indices.flush(); }

         size_t get_cache_usage() const { return _indices.get_cache_usage(); }

         size_t get_cache_size() const { return _indices.get_cache_size(); }

         void dump_lb_call_counts() { _indices.dump_lb_call_counts(); }

         void trim_cache() { _indices.trim_cache(); }

         void begin_bulk_load() { _indices.begin_bulk_load(); }

         void end_bulk_load() { _indices.end_bulk_load(); }

         void flush_bulk_load() { _indices.flush_bulk_load(); }

         class session {
            public:
               session( session&& mv )
               :_index(mv._index),_apply(mv._apply){ mv._apply = false; }

               ~session() {
                  if( _apply ) {
                     _index.undo();
                  }
               }

               /** leaves the UNDO state on the stack when session goes out of scope */
               void push()   { _apply = false; }
               /** combines this session with the prior session */
               void squash() { if( _apply ) _index.squash(); _apply = false; }
               void undo()   { if( _apply ) _index.undo();  _apply = false; }

               session& operator = ( session&& mv ) {
                  if( this == &mv ) return *this;
                  if( _apply ) _index.undo();
                  _apply = mv._apply;
                  mv._apply = false;
                  return *this;
               }

               int64_t revision()const { return _revision; }

            private:
               friend class generic_index;

               session( generic_index& idx, int64_t revision )
               :_index(idx),_revision(revision) {
                  if( revision == -1 )
                     _apply = false;
               }

               generic_index& _index;
               bool           _apply = true;
               int64_t        _revision = 0;
         };

         // TODO: This function needs some work to make it consistent on failure.
         session start_undo_session()
         {
            ++_revision;
            _indices.set_revision( _revision );
            assert( _indices.revision() == _revision );
            _stack.emplace_back( );
            _stack.back().old_next_id = _next_id;
            _stack.back().revision = _revision;
            return session( *this, _revision );
         }

         const index_type& indicies()const { return _indices; }


         /**
          *  Restores the state to how it was prior to the current session discarding all changes
          *  made between the last revision and the current revision.
          */
         void undo() {
            if( !enabled() ) return;

            const auto& head = _stack.back();

            for( auto& item : head.old_values ) {
               bool ok = false;
               auto itr = _indices.find( item.second.id );
               if( itr != _indices.end() )
               {
                  ok = _indices.modify( itr, [&]( value_type& v ) {
                     v = std::move( item.second );
                  });
               }
               else
               {
                  ok = _indices.emplace( std::move( item.second ) ).second;
               }

               if( !ok ) BOOST_THROW_EXCEPTION( std::logic_error( "Could not modify object, most likely a uniqueness constraint was violated" ) );
            }

            for( const auto& id : head.new_ids )
            {
               _indices.erase( _indices.find( id ) );
            }
            _next_id = head.old_next_id;
            _indices.set_next_id( _next_id );

            for( auto& item : head.removed_values ) {
               bool ok = _indices.emplace( std::move( item.second ) ).second;
               if( !ok ) BOOST_THROW_EXCEPTION( std::logic_error( "Could not restore object, most likely a uniqueness constraint was violated" ) );
            }

            _stack.pop_back();
            --_revision;
            _indices.set_revision( _revision );
            assert( _indices.revision() == _revision );
         }

         /**
          *  This method works similar to git squash, it merges the change set from the two most
          *  recent revision numbers into one revision number (reducing the head revision number)
          *
          *  This method does not change the state of the index, only the state of the undo buffer.
          */
         void squash()
         {
            if( !enabled() ) return;
            if( _stack.size() == 1 ) {
               _stack.pop_front();
               return;
            }

            auto& state = _stack.back();
            auto& prev_state = _stack[_stack.size()-2];

            // An object's relationship to a state can be:
            // in new_ids            : new
            // in old_values (was=X) : upd(was=X)
            // in removed (was=X)    : del(was=X)
            // not in any of above   : nop
            //
            // When merging A=prev_state and B=state we have a 4x4 matrix of all possibilities:
            //
            //                   |--------------------- B ----------------------|
            //
            //                +------------+------------+------------+------------+
            //                | new        | upd(was=Y) | del(was=Y) | nop        |
            //   +------------+------------+------------+------------+------------+
            // / | new        | N/A        | new       A| nop       C| new       A|
            // | +------------+------------+------------+------------+------------+
            // | | upd(was=X) | N/A        | upd(was=X)A| del(was=X)C| upd(was=X)A|
            // A +------------+------------+------------+------------+------------+
            // | | del(was=X) | N/A        | N/A        | N/A        | del(was=X)A|
            // | +------------+------------+------------+------------+------------+
            // \ | nop        | new       B| upd(was=Y)B| del(was=Y)B| nop      AB|
            //   +------------+------------+------------+------------+------------+
            //
            // Each entry was composed by labelling what should occur in the given case.
            //
            // Type A means the composition of states contains the same entry as the first of the two merged states for that object.
            // Type B means the composition of states contains the same entry as the second of the two merged states for that object.
            // Type C means the composition of states contains an entry different from either of the merged states for that object.
            // Type N/A means the composition of states violates causal timing.
            // Type AB means both type A and type B simultaneously.
            //
            // The merge() operation is defined as modifying prev_state in-place to be the state object which represents the composition of
            // state A and B.
            //
            // Type A (and AB) can be implemented as a no-op; prev_state already contains the correct value for the merged state.
            // Type B (and AB) can be implemented by copying from state to prev_state.
            // Type C needs special case-by-case logic.
            // Type N/A can be ignored or assert(false) as it can only occur if prev_state and state have illegal values
            // (a serious logic error which should never happen).
            //

            // We can only be outside type A/AB (the nop path) if B is not nop, so it suffices to iterate through B's three containers.

            for( const auto& item : state.old_values )
            {
               if( prev_state.new_ids.find( item.second.id ) != prev_state.new_ids.end() )
               {
                  // new+upd -> new, type A
                  continue;
               }
               if( prev_state.old_values.find( item.second.id ) != prev_state.old_values.end() )
               {
                  // upd(was=X) + upd(was=Y) -> upd(was=X), type A
                  continue;
               }
               // del+upd -> N/A
               assert( prev_state.removed_values.find(item.second.id) == prev_state.removed_values.end() );
               // nop+upd(was=Y) -> upd(was=Y), type B
               prev_state.old_values.emplace( std::move(item) );
            }

            // *+new, but we assume the N/A cases don't happen, leaving type B nop+new -> new
            for( const auto& id : state.new_ids )
               prev_state.new_ids.insert(id);

            // *+del
            for( auto& obj : state.removed_values )
            {
               if( prev_state.new_ids.find(obj.second.id) != prev_state.new_ids.end() )
               {
                  // new + del -> nop (type C)
                  prev_state.new_ids.erase(obj.second.id);
                  continue;
               }
               auto it = prev_state.old_values.find(obj.second.id);
               if( it != prev_state.old_values.end() )
               {
                  // upd(was=X) + del(was=Y) -> del(was=X)
                  prev_state.removed_values.emplace( std::move(*it) );
                  prev_state.old_values.erase(obj.second.id);
                  continue;
               }
               // del + del -> N/A
               assert( prev_state.removed_values.find( obj.second.id ) == prev_state.removed_values.end() );
               // nop + del(was=Y) -> del(was=Y)
               prev_state.removed_values.emplace( std::move(obj) ); //[obj.second->id] = std::move(obj.second);
            }

            _stack.pop_back();
            --_revision;
            _indices.set_revision( _revision );
            assert( _indices.revision() == _revision );
         }

         /**
          * Discards all undo history prior to revision
          */
         void commit( int64_t revision )
         {
            while( _stack.size() && _stack[0].revision <= revision )
            {
               _stack.pop_front();
            }
         }

         /**
          * Unwinds all undo states
          */
         void undo_all()
         {
            while( enabled() )
               undo();
         }

         int64_t revision()const { return _revision; }

         void set_revision( int64_t revision )
         {
            if( _stack.size() != 0 ) BOOST_THROW_EXCEPTION( std::logic_error("cannot set revision while there is an existing undo stack") );
            _revision = revision;
            _indices.set_revision( _revision );
            assert( _indices.revision() == _revision );
         }

         int64_t next_id()const { return _next_id._id; }
         void set_next_id( int64_t next_id ) { _next_id = typename value_type::id_type( next_id ); }

      private:
         bool enabled()const { return _stack.size(); }

         void on_modify( const value_type& v ) {
            if( !enabled() ) return;

            auto& head = _stack.back();

            if( head.new_ids.find( v.id ) != head.new_ids.end() )
               return;

            auto itr = head.old_values.find( v.id );
            if( itr != head.old_values.end() )
               return;

            head.old_values.emplace( std::pair< typename value_type::id_type, const value_type& >( v.id, v ) );
         }

         void on_remove( const value_type& v ) {
            if( !enabled() ) return;

            auto& head = _stack.back();
            if( head.new_ids.count(v.id) ) {
               head.new_ids.erase( v.id );
               return;
            }

            auto itr = head.old_values.find( v.id );
            if( itr != head.old_values.end() ) {
               head.removed_values.emplace( std::move( *itr ) );
               head.old_values.erase( v.id );
               return;
            }

            if( head.removed_values.count( v.id ) )
               return;

            head.removed_values.emplace( std::pair< typename value_type::id_type, const value_type& >( v.id, v ) );
         }

         void on_create( const value_type& v ) {
            if( !enabled() ) return;
            auto& head = _stack.back();

            head.new_ids.insert( v.id );
         }

         boost::container::deque< undo_state_type> _stack;

         /**
          *  Each new session increments the revision, a squash will decrement the revision by combining
          *  the two most recent revisions into one revision.
          *
          *  Commit will discard all revisions prior to the committed revision.
          */
         int64_t                         _revision = 0;
         typename value_type::id_type    _next_id = 0;
         index_type                      _indices;
         uint32_t                        _size_of_value_type = 0;
         uint32_t                        _size_of_this = 0;
   };

   class abstract_session {
      public:
         virtual ~abstract_session(){};
         virtual void push()             = 0;
         virtual void squash()           = 0;
         virtual void undo()             = 0;
         virtual int64_t revision()const  = 0;
   };

   template<typename SessionType>
   class session_impl : public abstract_session
   {
      public:
         session_impl( SessionType&& s ):_session( std::move( s ) ){}

         virtual void push() override  { _session.push();  }
         virtual void squash() override{ _session.squash(); }
         virtual void undo() override  { _session.undo();  }
         virtual int64_t revision()const override  { return _session.revision();  }
      private:
         SessionType _session;
   };

   class index_extension
   {
      public:
         index_extension() {}
         virtual ~index_extension() {}
   };

   typedef std::vector< std::shared_ptr< index_extension > > index_extensions;

   class abstract_index
   {
      public:
         typedef helpers::index_statistic_info statistic_info;

         abstract_index( void* i ):_idx_ptr(i){}
         virtual ~abstract_index(){}

         virtual unique_ptr<abstract_session> start_undo_session() = 0;

         virtual void    undo()const = 0;
         virtual void    squash()const = 0;
         virtual void    commit( int64_t revision )const = 0;
         virtual void    undo_all()const = 0;
         virtual uint32_t type_id()const  = 0;

         virtual int64_t revision()const = 0;
         virtual void    set_revision( int64_t revision ) = 0;
         virtual int64_t next_id()const = 0;
         virtual void    set_next_id( int64_t next_id ) = 0;

         virtual statistic_info get_statistics(bool onlyStaticInfo) const = 0;
         virtual size_t size() const = 0;
         virtual void clear() = 0;
         virtual void open( const bfs::path&, const boost::any& ) = 0;
         virtual void close() = 0;
         virtual void wipe( const bfs::path& dir ) = 0;
         virtual void flush() = 0;
         virtual size_t get_cache_usage() const = 0;
         virtual size_t get_cache_size() const = 0;
         virtual void dump_lb_call_counts() = 0;
         virtual void trim_cache() = 0;
         virtual void print_stats() const = 0;
         virtual void begin_bulk_load() = 0;
         virtual void end_bulk_load() = 0;
         virtual void flush_bulk_load() = 0;

         void add_index_extension( std::shared_ptr< index_extension > ext )  { _extensions.push_back( ext ); }
         const index_extensions& get_index_extensions()const  { return _extensions; }
         void* get()const { return _idx_ptr; }
      protected:
         void*              _idx_ptr;
      private:
         index_extensions   _extensions;
   };

   template<typename BaseIndex>
   class index_impl : public abstract_index {
      public:
         using abstract_index::statistic_info;

         index_impl( BaseIndex& base ):abstract_index( &base ),_base(base){}

         ~index_impl()
         {
            delete (BaseIndex*) abstract_index::_idx_ptr;
         }

         virtual unique_ptr<abstract_session> start_undo_session() override {
            return unique_ptr<abstract_session>(new session_impl<typename BaseIndex::session>( _base.start_undo_session() ) );
         }

         virtual void     undo()const  override { _base.undo(); }
         virtual void     squash()const  override { _base.squash(); }
         virtual void     commit( int64_t revision )const  override { _base.commit(revision); }
         virtual void     undo_all() const override {_base.undo_all(); }
         virtual uint32_t type_id()const override { return BaseIndex::value_type::type_id; }

         virtual int64_t  revision()const  override { return _base.revision(); }
         virtual void     set_revision( int64_t revision ) override { _base.set_revision( revision ); }
         virtual int64_t  next_id()const override { return _base.next_id(); }
         virtual void     set_next_id( int64_t next_id ) override { _base.set_next_id( next_id ); }

         virtual statistic_info get_statistics(bool onlyStaticInfo) const override final
         {
            typedef typename BaseIndex::index_type index_type;
            helpers::index_statistic_provider<index_type> provider;
            return provider.gather_statistics(_base.indices(), onlyStaticInfo);
         }

         virtual size_t size() const override final
         {
            return _base.indicies().size();
         }

         virtual void clear() override final
         {
            _base.clear();
         }

         virtual void open( const bfs::path& p, const boost::any& o ) override final
         {
            _base.open( p, o );
         }

         virtual void close() override final
         {
            _base.close();
         }

         virtual void wipe( const bfs::path& dir ) override final
         {
            _base.wipe( dir );
         }

         virtual void flush() override final
         {
            _base.flush();
         }

         virtual size_t get_cache_usage() const override final
         {
            return _base.get_cache_usage();
         }

         virtual size_t get_cache_size() const override final
         {
            return _base.get_cache_size();
         }

         virtual void dump_lb_call_counts() override final
         {
            _base.dump_lb_call_counts();
         }

         virtual void trim_cache() override final
         {
            _base.trim_cache();
         }

         virtual void print_stats() const override final
         {
            _base.indicies().print_stats();
         }

         virtual void begin_bulk_load() override final
         {
            _base.mutable_indices().begin_bulk_load();
         }

         virtual void end_bulk_load() override final
         {
            _base.mutable_indices().end_bulk_load();
         }

         virtual void flush_bulk_load() override final
         {
            _base.mutable_indices().flush_bulk_load();
         }

      private:
         BaseIndex& _base;
   };

   template<typename IndexType>
   class index : public index_impl<IndexType> {
      public:
         index( IndexType& i ):index_impl<IndexType>( i ){}
   };

   /**
    *  This class
    */
   class database
   {
      private:
         class abstract_index_type
         {
            public:
               abstract_index_type() {}
               virtual ~abstract_index_type() {}

               virtual void add_index( database& db ) = 0;
         };

         template< typename IndexType >
         class index_type_impl : public abstract_index_type
         {
            virtual void add_index( database& db ) override
            {
               db.add_index_helper< IndexType >();
            }
         };

      public:
         void open( const bfs::path& dir, uint32_t flags = 0, size_t shared_file_size = 0, const boost::any& database_cfg = nullptr );
         void close();
         void flush();
         size_t get_cache_usage() const;
         size_t get_cache_size() const;
         void dump_lb_call_counts();
         void trim_cache();
         void wipe( const bfs::path& dir );
         void resize( size_t new_shared_file_size );

         void require_lock_fail( const char* method, const char* lock_type, const char* tname )const;

         void require_read_lock( const char* method, const char* tname )const
         {
            if( _read_lock_count <= 0 && _write_lock_count <= 0)
               require_lock_fail(method, "read", tname);
         }

         void require_write_lock( const char* method, const char* tname )
         {
            if( _write_lock_count <= 0 )
               require_lock_fail(method, "write", tname);
         }

         struct session {
            public:
               session( session&& s )
                  : _index_sessions( std::move(s._index_sessions) ),
                    _revision( s._revision ),
                    _session_incrementer( s._session_incrementer )
               {}

               session( vector<std::unique_ptr<abstract_session>>&& s, std::atomic<int32_t>& session_count )
                  : _index_sessions( std::move(s) ), _session_incrementer( session_count )
               {
                  if( _index_sessions.size() )
                     _revision = _index_sessions[0]->revision();
               }

               ~session() {
                  undo();
               }

               void push()
               {
                  for( auto& i : _index_sessions ) i->push();
                  _index_sessions.clear();
               }

               void squash()
               {
                  for( auto& i : _index_sessions ) i->squash();
                  _index_sessions.clear();
               }

               void undo()
               {
                  for( auto& i : _index_sessions ) i->undo();
                  _index_sessions.clear();
               }

               int64_t revision()const { return _revision; }

            private:
               friend class database;

               vector< std::unique_ptr<abstract_session> > _index_sessions;
               int64_t _revision = -1;
               int_incrementer _session_incrementer;
         };

         session start_undo_session();

         int64_t revision()const {
             if( _index_list.size() == 0 ) return -1;
             return _index_list[0]->revision();
         }

         void undo();
         void squash();
         void commit( int64_t revision );
         void undo_all();


         void set_revision( int64_t revision )
         {
             CHAINBASE_REQUIRE_WRITE_LOCK( "set_revision", int64_t );
             for( const auto& i : _index_list ) i->set_revision( revision );
         }

         void print_stats()
         {
            for( const auto& i : _index_list )  i->print_stats();
         }

         template<typename MultiIndexType>
         void add_index()
         {
            _index_types.push_back( unique_ptr< abstract_index_type >( new index_type_impl< MultiIndexType >() ) );
            _index_types.back()->add_index( *this );
         }

         template<typename MultiIndexType>
         bool has_index()const
         {
            CHAINBASE_REQUIRE_READ_LOCK("get_index", typename MultiIndexType::value_type);
            typedef generic_index<MultiIndexType> index_type;
            return _index_map.size() > index_type::value_type::type_id && _index_map[index_type::value_type::type_id];
         }

         template<typename MultiIndexType>
         const generic_index<MultiIndexType>& get_index()const
         {
            CHAINBASE_REQUIRE_READ_LOCK("get_index", typename MultiIndexType::value_type);
            typedef generic_index<MultiIndexType> index_type;
            typedef index_type*                   index_type_ptr;

            if( !has_index< MultiIndexType >() )
            {
               std::string type_name = boost::core::demangle( typeid( typename index_type::value_type ).name() );
               BOOST_THROW_EXCEPTION( std::runtime_error( "unable to find index for " + type_name + " in database" ) );
            }

            return *index_type_ptr( _index_map[index_type::value_type::type_id]->get() );
         }

         template<typename MultiIndexType>
         void add_index_extension( std::shared_ptr< index_extension > ext )
         {
            typedef generic_index<MultiIndexType> index_type;

            if( !has_index< MultiIndexType >() )
            {
               std::string type_name = boost::core::demangle( typeid( typename index_type::value_type ).name() );
               BOOST_THROW_EXCEPTION( std::runtime_error( "unable to find index for " + type_name + " in database" ) );
            }

            _index_map[index_type::value_type::type_id]->add_index_extension( ext );
         }

         template<typename MultiIndexType, typename ByIndex>
         auto get_index()const -> decltype( ((generic_index<MultiIndexType>*)( nullptr ))->indicies().template get<ByIndex>() )
         {
            CHAINBASE_REQUIRE_READ_LOCK("get_index", typename MultiIndexType::value_type);
            typedef generic_index<MultiIndexType> index_type;
            typedef index_type*                   index_type_ptr;

            if( !has_index< MultiIndexType >() )
            {
               std::string type_name = boost::core::demangle( typeid( typename index_type::value_type ).name() );
               BOOST_THROW_EXCEPTION( std::runtime_error( "unable to find index for " + type_name + " in database" ) );
            }

            return index_type_ptr( _index_map[index_type::value_type::type_id]->get() )->indicies().template get<ByIndex>();
         }

         template<typename MultiIndexType>
         generic_index<MultiIndexType>& get_mutable_index()
         {
            CHAINBASE_REQUIRE_WRITE_LOCK("get_mutable_index", typename MultiIndexType::value_type);
            typedef generic_index<MultiIndexType> index_type;
            typedef index_type*                   index_type_ptr;

            if( !has_index< MultiIndexType >() )
            {
               std::string type_name = boost::core::demangle( typeid( typename index_type::value_type ).name() );
               BOOST_THROW_EXCEPTION( std::runtime_error( "unable to find index for " + type_name + " in database" ) );
            }

            return *index_type_ptr( _index_map[index_type::value_type::type_id]->get() );
         }

         template< typename ObjectType, typename IndexedByType, typename CompatibleKey >
         const ObjectType* find( CompatibleKey&& key )const
         {
             CHAINBASE_REQUIRE_READ_LOCK("find", ObjectType);
             typedef typename get_index_type< ObjectType >::type index_type;
             const auto& idx = get_index< index_type >().indicies().template get< IndexedByType >();
             auto itr = idx.find( std::forward< CompatibleKey >( key ) );
             if( itr == idx.end() ) return nullptr;
             return &*itr;
         }

         template< typename ObjectType >
         const ObjectType* find( oid< ObjectType > key = oid< ObjectType >() ) const
         {
             CHAINBASE_REQUIRE_READ_LOCK("find", ObjectType);
             typedef typename get_index_type< ObjectType >::type index_type;
             const auto& idx = get_index< index_type >().indices();
             auto itr = idx.find( key );
             if( itr == idx.end() ) return nullptr;
             return &*itr;
         }

         template< typename ObjectType, typename IndexedByType, typename CompatibleKey >
         const ObjectType& get( CompatibleKey&& key )const
         {
             CHAINBASE_REQUIRE_READ_LOCK("get", ObjectType);
             auto obj = find< ObjectType, IndexedByType >( std::forward< CompatibleKey >( key ) );
             if( !obj )
             {
                BOOST_THROW_EXCEPTION( std::out_of_range( "unknown key" ) );
             }
             return *obj;
         }

         template< typename ObjectType >
         const ObjectType& get( const oid< ObjectType >& key = oid< ObjectType >() )const
         {
             CHAINBASE_REQUIRE_READ_LOCK("get", ObjectType);
             auto obj = find< ObjectType >( key );
             if( !obj ) BOOST_THROW_EXCEPTION( std::out_of_range( "unknown key") );
             return *obj;
         }

         template<typename ObjectType, typename Modifier>
         void modify( const ObjectType& obj, Modifier&& m )
         {
             CHAINBASE_REQUIRE_WRITE_LOCK("modify", ObjectType);
             typedef typename get_index_type<ObjectType>::type index_type;
             get_mutable_index<index_type>().modify( obj, m );
         }

         template<typename ObjectType>
         void remove( const ObjectType& obj )
         {
             CHAINBASE_REQUIRE_WRITE_LOCK("remove", ObjectType);
             typedef typename get_index_type<ObjectType>::type index_type;
             return get_mutable_index<index_type>().remove( obj );
         }

         template<typename ObjectType>
         const ObjectType& create( const std::function <void (ObjectType&)> f)
         {
             CHAINBASE_REQUIRE_WRITE_LOCK("create", ObjectType);
             typedef typename get_index_type<ObjectType>::type index_type;
             return get_mutable_index<index_type>().emplace(f);
         }

         template< typename ObjectType >
         size_t count()const
         {
            typedef typename get_index_type<ObjectType>::type index_type;
            return get_index< index_type >().indices().size();
         }

         auto lock_read() {
            return counted_shared_lock(_write_lock_count, _mutex);
         }

         auto lock_write() {
            return counted_lock(_write_lock_count, _mutex);
         }

         template <class Function, class... Args>
         auto with_read_lock(Function&& func, Args&&... args) -> decltype(func(args...)) {
            auto _lock = lock_read();

            return func(args...);
         };

         template <class Function, class... Args>
         auto with_write_lock(Function&& func, Args&&... args) -> decltype(func(args...)) {
            auto _lock = lock_write();

            return func(args...);
         }

         template< typename Lambda >
         void bulk_load( Lambda&& callback )
         {
            for( auto& item : _index_list )
            {
               item->begin_bulk_load();
            }

            callback();

            for( auto& item : _index_list )
            {
               item->end_bulk_load();
            }
         }

         void flush_bulk_load()
         {
            for( auto& item : _index_list )
            {
               item->flush_bulk_load();
            }
         }

         template< typename IndexExtensionType, typename Lambda >
         void for_each_index_extension( Lambda&& callback )const
         {
            for( const abstract_index* idx : _index_list )
            {
               const index_extensions& exts = idx->get_index_extensions();
               for( const std::shared_ptr< index_extension >& e : exts )
               {
                  std::shared_ptr< IndexExtensionType > e2 = std::dynamic_pointer_cast< IndexExtensionType >( e );
                  if( e2 )
                     callback( e2 );
               }
            }
         }

         typedef vector<abstract_index*> abstract_index_cntr_t;

         const abstract_index_cntr_t& get_abstract_index_cntr() const
            { return _index_list; }

      private:
         template<typename MultiIndexType>
         void add_index_helper() {
            const uint16_t type_id = generic_index<MultiIndexType>::value_type::type_id;
            typedef generic_index<MultiIndexType>          index_type;

            std::string type_name = boost::core::demangle( typeid( typename index_type::value_type ).name() );

            if( !( _index_map.size() <= type_id || _index_map[ type_id ] == nullptr ) ) {
               BOOST_THROW_EXCEPTION( std::logic_error( type_name + "::type_id is already in use" ) );
            }

            index_type* idx_ptr =  nullptr;
            idx_ptr = new index_type( );
            idx_ptr->validate();

            if( type_id >= _index_map.size() )
               _index_map.resize( type_id + 1 );

            auto new_index = new index<index_type>( *idx_ptr );

            _index_map[ type_id ].reset( new_index );
            _index_list.push_back( new_index );

            if( _is_open ) new_index->open( _data_dir, _database_cfg );
         }

         /**
          * This is a sparse list of known indicies kept to accelerate creation of undo sessions
          */
         abstract_index_cntr_t                                       _index_list;

         /**
          * This is a full map (size 2^16) of all possible index designed for constant time lookup
          */
         vector<unique_ptr<abstract_index>>                          _index_map;

         vector<unique_ptr<abstract_index_type>>                     _index_types;

         bfs::path                                                   _data_dir;

         boost::shared_mutex                                         _mutex;
         std::atomic<int32_t>                                        _read_lock_count = {0};
         std::atomic<int32_t>                                        _write_lock_count = {0};

         bool                                                        _is_open = false;

         std::atomic<int32_t>                                        _undo_session_count = {0};
         size_t                                                      _file_size = 0;
         boost::any                                                  _database_cfg = nullptr;
   };

}  // namepsace chainbase

