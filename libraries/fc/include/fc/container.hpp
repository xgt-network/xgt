#pragma once

#include <fc/variant.hpp>
#include <boost/container/deque.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/map.hpp>
#include <boost/container/set.hpp>
#include <boost/container/vector.hpp>
#include <fc/crypto/hex.hpp>
#include <fc/io/raw_fwd.hpp>

// boost::interprocess::flat_map is an alias to boost::container::flat_map
// so it uses the reflection/serialization for flat_map and does not have any here
// BUT we add typename for flat_map so it works with allocators

namespace fc {

    template<typename... T >
    void to_variant( const boost::container::deque< T... >& t, fc::variant& v ) {
      std::vector<variant> vars(t.size());
      for( size_t i = 0; i < t.size(); ++i ) {
         vars[i] = t[i];
      }
      v = std::move(vars);
    }

    template<typename T, typename... A>
    void from_variant( const fc::variant& v, boost::container::deque< T, A... >& d ) {
      const variants& vars = v.get_array();
      d.clear();
      d.resize( vars.size() );
      for( uint32_t i = 0; i < vars.size(); ++i ) {
         from_variant( vars[i], d[i] );
      }
    }

    //boost::container::map == boost::map
    template<typename K, typename V, typename... T >
    void to_variant( const boost::container::map< K, V, T... >& var, fc::variant& vo ) {
       std::vector< variant > vars(var.size());
       size_t i = 0;
       for( auto itr = var.begin(); itr != var.end(); ++itr, ++i )
          vars[i] = fc::variant(*itr);
       vo = vars;
    }
/*
    template<typename K, typename V, typename... A>
    void from_variant( const variant& var,  boost::container::map<K, V, A...>& vo )
    {
       const variants& vars = var.get_array();
       vo.clear();
       for( auto itr = vars.begin(); itr != vars.end(); ++itr )
          vo.insert( itr->as< std::pair<K,V> >() ); Not safe for interprocess. Needs allocator
    }
*/

    template<typename... T >
    void to_variant( const boost::container::vector< T... >& t, fc::variant& v ) {
      std::vector<variant> vars(t.size());
      for( size_t i = 0; i < t.size(); ++i ) {
         vars[i] = t[i];
      }
      v = std::move(vars);
    }

    template<typename T, typename... A>
    void from_variant( const fc::variant& v, boost::container::vector< T, A... >& d ) {
      const variants& vars = v.get_array();
      d.clear();
      d.resize( vars.size() );
      for( uint32_t i = 0; i < vars.size(); ++i ) {
         from_variant( vars[i], d[i] );
      }
    }

    template<typename... T >
    void to_variant( const boost::container::set< T... >& t, fc::variant& v ) {
      std::vector<variant> vars;
      vars.reserve(t.size());
      for( const auto& item : t ) {
         vars.emplace_back( item );
      }
      v = std::move(vars);
    }

/*
    template<typename T, typename... A>
    void from_variant( const fc::variant& v, boost::container::set< T, A... >& d ) {
      const variants& vars = v.get_array();
      d.clear();
      d.reserve( vars.size() );
      for( uint32_t i = 0; i < vars.size(); ++i ) {
         from_variant( vars[i], d[i] ); Not safe for interprocess. Needs allocator
      }
    }
*/

    template<typename... A>
    void to_variant( const boost::container::vector<char, A...>& t, fc::variant& v )
    {
        if( t.size() )
            v = variant(fc::to_hex(t.data(), t.size()));
        else
            v = "";
    }

    template<typename... A>
    void from_variant( const fc::variant& v, boost::container::vector<char, A...>& d )
    {
         auto str = v.as_string();
         d.resize( str.size() / 2 );
         if( d.size() )
         {
            size_t r = fc::from_hex( str, d.data(), d.size() );
            FC_ASSERT( r == d.size() );
         }
    //   std::string b64 = base64_decode( var.as_string() );
    //   vo = std::vector<char>( b64.c_str(), b64.c_str() + b64.size() );
    }

   namespace raw {

       template<typename Stream, typename T, typename... A>
       inline void pack( Stream& s, const boost::container::vector<T,A...>& value ) {
         pack( s, unsigned_int((uint32_t)value.size()) );
         auto itr = value.begin();
         auto end = value.end();
         while( itr != end ) {
           fc::raw::pack( s, *itr );
           ++itr;
         }
       }
       template<typename Stream, typename T, typename... A>
       inline void unpack( Stream& s, boost::container::vector<T,A...>& value, uint32_t depth = 0 ) {
         depth++;
         FC_ASSERT( depth <= MAX_RECURSION_DEPTH );
         unsigned_int size;
         unpack( s, size, depth );
         value.clear();
         value.resize(size.value);
         for ( size_t i = 0; i < size.value; i++ )
         {
            fc::raw::unpack(s, value[i], depth);
         }
       }
   }

template< typename E >
struct get_typename< boost::container::deque< E > >
{
   static const char* name()
   {
      static std::string n = std::string("boost::container::deque<") + get_typename<E>::name() + ">";
      return n.c_str();
   }
};

template< typename K, typename V, typename Comp >
struct get_typename< boost::container::flat_map< K, V, Comp > >
{
   static const char* name()
   {
      static std::string n = std::string("boost::container::flat_map<")
         + get_typename<K>::name() + std::string(",")
         + get_typename<V>::name() + std::string(">");
      return n.c_str();
   }
};

template< typename K, typename V >
struct get_typename< boost::container::map< K, V > >
{
   static const char* name()
   {
      static std::string n = std::string("boost::container::map<") + get_typename<K>::name() + ", " + get_typename<V>::name() + ">";
      return n.c_str();
   }
};

template< typename E >
struct get_typename< boost::container::set< E > >
{
   static const char* name()
   {
      static std::string n = std::string("boost::container::set<") + get_typename<E>::name() + ">";
      return n.c_str();
   }
};

template< typename E >
struct get_typename< boost::container::vector< E > >
{
   static const char* name()
   {
      static std::string n = std::string("boost::container::vector<") + get_typename<E>::name() + ">";
      return n.c_str();
   }
};

}
