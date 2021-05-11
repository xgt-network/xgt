#pragma once

#include <fc/uint128.hpp>
#include <fc/io/raw_fwd.hpp>

#include <boost/endian/conversion.hpp>
#include <boost/utility/identity_type.hpp>

#include <xgt/protocol/types_fwd.hpp>

// These overloads need to be defined before the implementation in fixed_string
namespace fc
{
   /**
    * Endian-reversible pair class.
    */

   template< typename T >
   T endian_reverse( const T& x )
   {  return boost::endian::endian_reverse(x);  }

   template<>
   inline uint128 endian_reverse( const uint128& u )
   {  return uint128( boost::endian::endian_reverse( u.hi ), boost::endian::endian_reverse( u.lo ) );  }

   template< typename... A >
   struct erpair
   {
      erpair() {}
      erpair(const A&... a) : data(std::make_tuple(a...)) {};
      erpair(std::tuple< A... > p) : data(p) {};

      friend bool operator < ( const erpair& a, const erpair& b )
      { return a.data < b.data; }
      friend bool operator <= ( const erpair& a, const erpair& b )
      { return a.data <= b.data; }
      friend bool operator > ( const erpair& a, const erpair& b )
      { return a.data > b.data; }
      friend bool operator >= ( const erpair& a, const erpair& b )
      { return a.data >= b.data; }
      friend bool operator == ( const erpair& a, const erpair& b )
      { return a.data == b.data; }
      friend bool operator != ( const erpair& a, const erpair& b )
      { return a.data != b.data; }

      std::tuple< A... > get_data() const
      { return data; }

      std::tuple< A... > data{};
   };

   template< typename... A >
   erpair< A... > make_erpair( const A&... a )
   {  return erpair< A... >(a...); }

   template<typename Tuple, size_t... I>
   auto call_endian_reverse_with_tuple(const Tuple &t, std::index_sequence< I... >)
   {
      return Tuple( endian_reverse(std::get<I>(t))... );
   }

   template< typename... A >
   erpair< A... > endian_reverse( const erpair< A... >& p )
   {
      static constexpr auto size = std::tuple_size< std::tuple< A... > >::value;
      std::tuple< A... > t = p.get_data();
      return erpair< A... >( call_endian_reverse_with_tuple(t, std::make_index_sequence<size>{}) );
   }
}

namespace xgt { namespace protocol {

/**
 * This class is an in-place memory allocation of a fixed length character string.
 *
 * The string will serialize the same way as std::string for variant and raw formats.
 */

template< typename _Storage >
class fixed_string_impl
{
   public:
      typedef _Storage Storage;

      fixed_string_impl() { memset( (char*)&data, 0, sizeof(data) ); }
      fixed_string_impl( const fixed_string_impl& c ) : data( c.data ){}
      fixed_string_impl( const char* str ) : fixed_string_impl( std::string( str ) ) {}
      fixed_string_impl( const std::string& str )
      {
         Storage d;
         memset( (char*)&d, 0, sizeof(d) );
         if( str.size() <= sizeof(d) )
            memcpy( (char*)&d, str.c_str(), str.size() );
         else
            memcpy( (char*)&d, str.c_str(), sizeof(d) );

         data = boost::endian::big_to_native( d );
      }

      operator std::string()const
      {
         Storage d = boost::endian::native_to_big( data );
         size_t s = strnlen( (const char*)&d, sizeof(d) );
         const char* self = (const char*)&d;

         return std::string( self, self + s );
      }

      uint32_t size()const
      {
         Storage d = boost::endian::native_to_big( data );

         return strnlen( (const char*)&d, sizeof(d) );
      }

      uint32_t length()const { return size(); }

      fixed_string_impl& operator = ( const fixed_string_impl& str )
      {
         data = str.data;
         return *this;
      }

      fixed_string_impl& operator = ( const char* str )
      {
         *this = fixed_string_impl( str );
         return *this;
      }

      fixed_string_impl& operator = ( const std::string& str )
      {
         *this = fixed_string_impl( str );
         return *this;
      }

      friend std::string operator + ( const fixed_string_impl& a, const std::string& b ) { return std::string( a ) + b; }
      friend std::string operator + ( const std::string& a, const fixed_string_impl& b ){ return a + std::string( b ); }
      friend bool operator < ( const fixed_string_impl& a, const fixed_string_impl& b ) { return a.data < b.data; }
      friend bool operator <= ( const fixed_string_impl& a, const fixed_string_impl& b ) { return a.data <= b.data; }
      friend bool operator > ( const fixed_string_impl& a, const fixed_string_impl& b ) { return a.data > b.data; }
      friend bool operator >= ( const fixed_string_impl& a, const fixed_string_impl& b ) { return a.data >= b.data; }
      friend bool operator == ( const fixed_string_impl& a, const fixed_string_impl& b ) { return a.data == b.data; }
      friend bool operator != ( const fixed_string_impl& a, const fixed_string_impl& b ) { return a.data != b.data; }

      Storage data;
};

// These storage types work with memory layout and should be used instead of a custom template.
template< size_t N > struct fixed_string_impl_for_size;
template< typename T > struct fixed_string_size_for_impl;

//
// The main purpose of this macro is to auto-generate the boilerplate
// for different fixed_string sizes that have different backing types.
//
// This "boilerplate" is simply templates which allow us to go from
// size -> type and from type -> size at compile time.
//
// We want to write this one-liner:
//
//     XGT_DEFINE_FIXED_STRING_IMPL( 32, fc::erpair< fc::uint128_t, fc::uint128_t > )
//
// Unfortunately, since the preprocessor doesn't do syntactic parsing,
// this would be regarded as a 3-argument call (since there are three commas,
// and the PP doesn't "know" enough C++ syntax to "understand" the internal
// comma separating the two arguments to erpair "shouldn't" be taken
// as delimiting separate macro arguments).
//
// Fortunately, there is a solution that involves using parentheses to "quote"
// the argument, then using a dummy function type to "unquote" it.
// This solution is explained here:
//
//     https://stackoverflow.com/questions/13842468/comma-in-c-c-macro
//
// We don't have to code this preprocessor hack ourselves, as a battle-tested,
// widely-compatible implementation is available in the Boost Identity Type
// library.  Which allows our one-liner to be:
//
//     XGT_DEFINE_FIXED_STRING_IMPL( 32, BOOST_IDENTITY_TYPE((fc::erpair< fc::uint128_t, fc::uint128_t >)) )
//
#define XGT_DEFINE_FIXED_STRING_IMPL( SIZE, STORAGE_TYPE )         \
template<>                                                         \
struct fixed_string_impl_for_size< SIZE >                          \
{                                                                  \
   typedef xgt::protocol::fixed_string_impl< STORAGE_TYPE > t;   \
};                                                                 \
                                                                   \
template<>                                                         \
struct fixed_string_size_for_impl< STORAGE_TYPE >                  \
{                                                                  \
   static const size_t size = SIZE;                                \
};

XGT_DEFINE_FIXED_STRING_IMPL( 8, uint64_t)
XGT_DEFINE_FIXED_STRING_IMPL( 16, BOOST_IDENTITY_TYPE((fc::uint128_t)) )
//XGT_DEFINE_FIXED_STRING_IMPL( 24, BOOST_IDENTITY_TYPE((fc::erpair< fc::uint128_t, uint64_t >)) )
XGT_DEFINE_FIXED_STRING_IMPL( 32, BOOST_IDENTITY_TYPE((fc::erpair< fc::uint128_t, fc::uint128_t >)) )
XGT_DEFINE_FIXED_STRING_IMPL( 256, BOOST_IDENTITY_TYPE((
   fc::erpair<
      fc::uint128_t, fc::uint128_t, fc::uint128_t, fc::uint128_t,
      fc::uint128_t, fc::uint128_t, fc::uint128_t, fc::uint128_t,
      fc::uint128_t, fc::uint128_t, fc::uint128_t, fc::uint128_t,
      fc::uint128_t, fc::uint128_t, fc::uint128_t, fc::uint128_t
   >
)) )

template< size_t N >
using fixed_string = typename fixed_string_impl_for_size<N>::t;

} } // xgt::protocol

namespace fc { namespace raw {

template< typename Stream, typename Storage >
inline void pack( Stream& s, const xgt::protocol::fixed_string_impl< Storage >& u )
{
   pack( s, std::string( u ) );
}

template< typename Stream, typename Storage >
inline void unpack( Stream& s, xgt::protocol::fixed_string_impl< Storage >& u, uint32_t depth )
{
   depth++;
   std::string str;
   unpack( s, str, depth );
   u = str;
}

} // raw

template< typename Storage >
void to_variant(   const xgt::protocol::fixed_string_impl< Storage >& s, variant& v )
{
   v = std::string( s );
}

template< typename Storage >
void from_variant( const variant& v, xgt::protocol::fixed_string_impl< Storage >& s )
{
   s = v.as_string();
}

template< typename Storage >
struct get_typename< xgt::protocol::fixed_string_impl< Storage > >
{
   static const char* name()
   {
      static const std::string n =
         std::string("xgt::protocol::fixed_string<") +
         std::to_string( xgt::protocol::fixed_string_size_for_impl<Storage>::size ) +
         std::string(">");
      return n.c_str();
   }
};

} // fc
