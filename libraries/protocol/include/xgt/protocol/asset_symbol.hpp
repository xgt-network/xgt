#pragma once

#include <fc/io/raw.hpp>
#include <xgt/protocol/types_fwd.hpp>

#define XGT_ASSET_SYMBOL_PRECISION_BITS    4
#define XGT_ASSET_CONTROL_BITS             1
#define XGT_NAI_SHIFT                      ( XGT_ASSET_SYMBOL_PRECISION_BITS + XGT_ASSET_CONTROL_BITS )
#define XTT_MAX_NAI                        99999999
#define XTT_MIN_NAI                        1
#define XTT_MIN_NON_RESERVED_NAI           10000000
#define XGT_ASSET_SYMBOL_NAI_LENGTH        10
#define XGT_ASSET_SYMBOL_NAI_STRING_LENGTH ( XGT_ASSET_SYMBOL_NAI_LENGTH + 2 )
#define XTT_MAX_NAI_POOL_COUNT             10
#define XTT_MAX_NAI_GENERATION_TRIES       100

#define XGT_PRECISION_XGT   (8)

// One's place is used for check digit, which means NAI 0-9 all have NAI data of 0 which is invalid
// This space is safe to use because it would alwasys result in failure to convert from NAI
#define XGT_NAI_XGT   (2)

#define XGT_ASSET_NUM_XGT \
  (uint32_t(((XTT_MAX_NAI + XGT_NAI_XGT) << XGT_NAI_SHIFT) | XGT_PRECISION_XGT))

#define XGT_SYMBOL_U64    (uint64_t('X') | (uint64_t('G') << 8) | (uint64_t('T') << 16))
#define XGT_SYMBOL_STR    "XGT"

#define XGT_SYMBOL_SER    (uint64_t(3) | (XGT_SYMBOL_U64 << 8)) ///< XGT with 3 digits of precision

#define XGT_ASSET_MAX_DECIMALS 12

#define XTT_ASSET_NUM_PRECISION_MASK   0xF
#define XTT_ASSET_NUM_CONTROL_MASK     0x10
#define XTT_ASSET_NUM_VESTING_MASK     0x20

#define ASSET_SYMBOL_NAI_KEY      "nai"
#define ASSET_SYMBOL_DECIMALS_KEY "precision"

namespace xgt { namespace protocol {

class asset_symbol_type
{
   public:
      enum asset_symbol_space
      {
         legacy_space = 1,
         xtt_nai_space = 2
      };

      explicit operator uint32_t() { return to_nai(); }

      // buf must have space for XGT_ASSET_SYMBOL_MAX_LENGTH+1
      static asset_symbol_type from_string( const std::string& str );
      static asset_symbol_type from_nai_string( const char* buf, uint8_t decimal_places );
      static asset_symbol_type from_asset_num( uint32_t asset_num )
      {   asset_symbol_type result;   result.asset_num = asset_num;   return result;   }
      static uint32_t asset_num_from_nai( uint32_t nai, uint8_t decimal_places );
      static asset_symbol_type from_nai( uint32_t nai, uint8_t decimal_places )
      {   return from_asset_num( asset_num_from_nai( nai, decimal_places ) );          }
      static uint8_t damm_checksum_8digit(uint32_t value);

      std::string to_string()const;

      void to_nai_string( char* buf )const;
      std::string to_nai_string()const
      {
         char buf[ XGT_ASSET_SYMBOL_NAI_STRING_LENGTH ];
         to_nai_string( buf );
         return std::string( buf );
      }

      uint32_t to_nai()const;

      asset_symbol_type get_liquid_symbol() const;

      /**
       * Returns asset_num stripped of precision holding bits.
       * \warning checking that it's XTT symbol is caller responsibility.
       */
      uint32_t get_stripped_precision_xtt_num() const
      {
         return asset_num & ~( XTT_ASSET_NUM_PRECISION_MASK );
      }

      asset_symbol_space space()const;
      uint8_t decimals()const
      {  return uint8_t( asset_num & XTT_ASSET_NUM_PRECISION_MASK );    }
      void validate()const;

      friend bool operator == ( const asset_symbol_type& a, const asset_symbol_type& b )
      {  return (a.asset_num == b.asset_num);   }
      friend bool operator != ( const asset_symbol_type& a, const asset_symbol_type& b )
      {  return (a.asset_num != b.asset_num);   }
      friend bool operator <  ( const asset_symbol_type& a, const asset_symbol_type& b )
      {  return (a.asset_num <  b.asset_num);   }
      friend bool operator >  ( const asset_symbol_type& a, const asset_symbol_type& b )
      {  return (a.asset_num >  b.asset_num);   }
      friend bool operator <= ( const asset_symbol_type& a, const asset_symbol_type& b )
      {  return (a.asset_num <= b.asset_num);   }
      friend bool operator >= ( const asset_symbol_type& a, const asset_symbol_type& b )
      {  return (a.asset_num >= b.asset_num);   }

      uint32_t asset_num = 0;
};

} } // xgt::protocol

FC_REFLECT(xgt::protocol::asset_symbol_type, (asset_num))

namespace fc { namespace raw {

// Legacy serialization of assets
// 0000pppp aaaaaaaa bbbbbbbb cccccccc dddddddd eeeeeeee ffffffff 00000000
// Symbol = abcdef
//
// NAI serialization of assets
// aaa1pppp bbbbbbbb cccccccc dddddddd
// NAI = (MSB to LSB) dddddddd cccccccc bbbbbbbb aaa
//
// NAI internal storage of legacy assets

template< typename Stream >
inline void pack( Stream& s, const xgt::protocol::asset_symbol_type& sym )
{
   switch( sym.space() )
   {
      case xgt::protocol::asset_symbol_type::legacy_space:
      {
         uint64_t ser = 0;
         switch( sym.asset_num )
         {
            case XGT_ASSET_NUM_XGT:
               ser = XGT_SYMBOL_SER;
               break;
            default:
               FC_ASSERT( false, "Cannot serialize unknown asset symbol" );
         }
         pack( s, ser );
         break;
      }
      case xgt::protocol::asset_symbol_type::xtt_nai_space:
         pack( s, sym.asset_num );
         break;
      default:
         FC_ASSERT( false, "Cannot serialize unknown asset symbol" );
   }
}

template< typename Stream >
inline void unpack( Stream& s, xgt::protocol::asset_symbol_type& sym, uint32_t )
{
   uint64_t ser = 0;
   s.read( (char*) &ser, 4 );

   switch( ser )
   {
      case XGT_SYMBOL_SER & 0xFFFFFFFF:
         s.read( ((char*) &ser)+4, 4 );
         FC_ASSERT( ser == XGT_SYMBOL_SER, "invalid asset bits" );
         sym.asset_num = XGT_ASSET_NUM_XGT;
         break;
      default:
         sym.asset_num = uint32_t( ser );
   }
   sym.validate();
}

} // fc::raw

inline void to_variant( const xgt::protocol::asset_symbol_type& sym, fc::variant& var )
{
   try
   {
      mutable_variant_object o;
         o( ASSET_SYMBOL_NAI_KEY, sym.to_nai_string() )
          ( ASSET_SYMBOL_DECIMALS_KEY, sym.decimals() );
      var = std::move( o );
   } FC_CAPTURE_AND_RETHROW()
}

inline void from_variant( const fc::variant& var, xgt::protocol::asset_symbol_type& sym )
{
   using xgt::protocol::asset_symbol_type;

   try
   {
      FC_ASSERT( var.is_object(), "Asset symbol is expected to be an object." );

      auto& o = var.get_object();

      auto nai = o.find( ASSET_SYMBOL_NAI_KEY );
      FC_ASSERT( nai != o.end(), "Expected key '${key}'.", ("key", ASSET_SYMBOL_NAI_KEY) );
      FC_ASSERT( nai->value().is_string(), "Expected a string type for value '${key}'.", ("key", ASSET_SYMBOL_NAI_KEY) );

      uint8_t precision = 0;
      auto decimals = o.find( ASSET_SYMBOL_DECIMALS_KEY );
      if( decimals != o.end() )
      {
         FC_ASSERT( decimals->value().is_uint64(), "Expected an unsigned integer type for value '${key}'.", ("key", ASSET_SYMBOL_DECIMALS_KEY) );
         FC_ASSERT( decimals->value().as_uint64() <= XGT_ASSET_MAX_DECIMALS,
            "Expected decimals to be less than or equal to ${num}", ("num", XGT_ASSET_MAX_DECIMALS) );

         precision = decimals->value().as< uint8_t >();
      }

      sym = asset_symbol_type::from_nai_string( nai->value().as_string().c_str(), precision );
   } FC_CAPTURE_AND_RETHROW()
}

} // fc
