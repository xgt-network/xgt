#include <xgt/protocol/asset.hpp>
#include <xgt/protocol/validation.hpp>
#include <fc/io/json.hpp>

#include <boost/rational.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#define ASSET_AMOUNT_KEY     "amount"
#define ASSET_PRECISION_KEY  "precision"
#define ASSET_NAI_KEY        "nai"

/*

The bounds on asset serialization are as follows:

index : field
0     : decimals
1..6  : symbol
   7  : \0
*/

namespace xgt { namespace protocol {

std::string asset_symbol_type::to_string()const
{
   return fc::json::to_string( fc::variant( *this ) );
}

asset_symbol_type asset_symbol_type::from_string( const std::string& str )
{
   return fc::json::from_string( str ).as< asset_symbol_type >();
}

void asset_symbol_type::to_nai_string( char* buf )const
{
   static_assert( XGT_ASSET_SYMBOL_NAI_STRING_LENGTH >= 12, "This code will overflow a short buffer" );
   uint32_t x = to_nai();
   buf[11] = '\0';
   buf[10] = ((x%10)+'0');  x /= 10;
   buf[ 9] = ((x%10)+'0');  x /= 10;
   buf[ 8] = ((x%10)+'0');  x /= 10;
   buf[ 7] = ((x%10)+'0');  x /= 10;
   buf[ 6] = ((x%10)+'0');  x /= 10;
   buf[ 5] = ((x%10)+'0');  x /= 10;
   buf[ 4] = ((x%10)+'0');  x /= 10;
   buf[ 3] = ((x%10)+'0');  x /= 10;
   buf[ 2] = ((x   )+'0');
   buf[ 1] = '@';
   buf[ 0] = '@';
}

asset_symbol_type asset_symbol_type::from_nai_string( const char* p, uint8_t decimal_places )
{
   try
   {
      FC_ASSERT( p != nullptr, "NAI string cannot be a null" );
      FC_ASSERT( std::strlen( p ) == XGT_ASSET_SYMBOL_NAI_STRING_LENGTH - 1, "Incorrect NAI string length" );
      FC_ASSERT( p[0] == '@' && p[1] == '@', "Invalid NAI string prefix" );
      uint32_t nai = boost::lexical_cast< uint32_t >( p + 2 );
      return asset_symbol_type::from_nai( nai, decimal_places );
   } FC_CAPTURE_AND_RETHROW();
}

// Highly optimized implementation of Damm algorithm
// https://en.wikipedia.org/wiki/Damm_algorithm
uint8_t asset_symbol_type::damm_checksum_8digit(uint32_t value)
{
   FC_ASSERT( value < 100000000 );

   const uint8_t t[] = {
       0, 30, 10, 70, 50, 90, 80, 60, 40, 20,
      70,  0, 90, 20, 10, 50, 40, 80, 60, 30,
      40, 20,  0, 60, 80, 70, 10, 30, 50, 90,
      10, 70, 50,  0, 90, 80, 30, 40, 20, 60,
      60, 10, 20, 30,  0, 40, 50, 90, 70, 80,
      30, 60, 70, 40, 20,  0, 90, 50, 80, 10,
      50, 80, 60, 90, 70, 20,  0, 10, 30, 40,
      80, 90, 40, 50, 30, 60, 20,  0, 10, 70,
      90, 40, 30, 80, 60, 10, 70, 20,  0, 50,
      20, 50, 80, 10, 40, 30, 60, 70, 90, 0
   };

   uint32_t q0 = value/10;
   uint32_t d0 = value%10;
   uint32_t q1 = q0/10;
   uint32_t d1 = q0%10;
   uint32_t q2 = q1/10;
   uint32_t d2 = q1%10;
   uint32_t q3 = q2/10;
   uint32_t d3 = q2%10;
   uint32_t q4 = q3/10;
   uint32_t d4 = q3%10;
   uint32_t q5 = q4/10;
   uint32_t d5 = q4%10;
   uint32_t d6 = q5%10;
   uint32_t d7 = q5/10;

   uint8_t x = t[d7];
   x = t[x+d6];
   x = t[x+d5];
   x = t[x+d4];
   x = t[x+d3];
   x = t[x+d2];
   x = t[x+d1];
   x = t[x+d0];
   return x/10;
}

uint32_t asset_symbol_type::asset_num_from_nai( uint32_t nai, uint8_t decimal_places )
{
   // Can be replaced with some clever bitshifting
   uint32_t nai_check_digit = nai % 10;
   uint32_t nai_data_digits = nai / 10;

   FC_ASSERT( (nai_data_digits >= XTT_MIN_NAI) & (nai_data_digits <= XTT_MAX_NAI), "NAI out of range" );
   FC_ASSERT( nai_check_digit == damm_checksum_8digit(nai_data_digits), "Invalid check digit" );

   switch( nai_data_digits )
   {
      case XGT_NAI_XGT:
         FC_ASSERT( decimal_places == XGT_PRECISION_XGT );
         return XGT_ASSET_NUM_XGT;
      default:
         FC_ASSERT( decimal_places <= XGT_ASSET_MAX_DECIMALS, "Invalid decimal_places" );
         return (nai_data_digits << XGT_NAI_SHIFT) | XTT_ASSET_NUM_CONTROL_MASK | decimal_places;
   }
}

uint32_t asset_symbol_type::to_nai()const
{
   uint32_t nai_data_digits = 0;

   // Can be replaced with some clever bitshifting
   switch( asset_num )
   {
      case XGT_ASSET_NUM_XGT:
         nai_data_digits = XGT_NAI_XGT;
         break;
      default:
         FC_ASSERT( space() == xtt_nai_space );
         nai_data_digits = (asset_num >> XGT_NAI_SHIFT);
   }

   uint32_t nai_check_digit = damm_checksum_8digit(nai_data_digits);
   return nai_data_digits * 10 + nai_check_digit;
}

asset_symbol_type asset_symbol_type::get_liquid_symbol()const
{
   switch( space() )
   {
      case legacy_space:
      {
         switch( asset_num )
         {
            case XGT_ASSET_NUM_XGT:
               return XGT_SYMBOL;
            default:
               FC_ASSERT( false, "Unknown asset symbol" );
         }
      }
      case xtt_nai_space:
      {
         return from_asset_num( asset_num & ~XTT_ASSET_NUM_VESTING_MASK );
      }
      default:
         FC_ASSERT( false, "Unknown asset_symbol" );
   }
}

asset_symbol_type::asset_symbol_space asset_symbol_type::space()const
{
   asset_symbol_type::asset_symbol_space s = legacy_space;
   switch( asset_num )
   {
      case XGT_ASSET_NUM_XGT:
         s = legacy_space;
         break;
      default:
         s = xtt_nai_space;
   }
   return s;
}

void asset_symbol_type::validate()const
{
   switch( asset_num )
   {
      case XGT_ASSET_NUM_XGT:
         break;
      default:
      {
         uint32_t nai_data_digits = (asset_num >> XGT_NAI_SHIFT);
         uint32_t nai_1bit = (asset_num & XTT_ASSET_NUM_CONTROL_MASK);
         uint32_t nai_decimal_places = (asset_num & XTT_ASSET_NUM_PRECISION_MASK);
         FC_ASSERT( (nai_data_digits >= XTT_MIN_NAI) &
                    (nai_data_digits <= XTT_MAX_NAI) &
                    (nai_1bit == XTT_ASSET_NUM_CONTROL_MASK) &
                    (nai_decimal_places <= XGT_ASSET_MAX_DECIMALS),
                    "Cannot determine space for asset ${n}", ("n", asset_num) );
      }
   }
   // this assert is duplicated by above code in all cases
   // FC_ASSERT( decimals() <= XGT_ASSET_MAX_DECIMALS );
}

void asset::validate()const
{
   symbol.validate();
   FC_ASSERT( amount.value >= 0 );
   FC_ASSERT( amount.value <= XGT_MAX_SATOSHIS );
}

#define BQ(a) \
   std::tie( a.base.symbol, a.quote.symbol )

#define DEFINE_PRICE_COMPARISON_OPERATOR( op ) \
bool operator op ( const price& a, const price& b ) \
{ \
   if( BQ(a) != BQ(b) ) \
      return BQ(a) op BQ(b); \
   \
   const uint128_t amult = uint128_t( b.quote.amount.value ) * a.base.amount.value; \
   const uint128_t bmult = uint128_t( a.quote.amount.value ) * b.base.amount.value; \
   \
   return amult op bmult;  \
}

DEFINE_PRICE_COMPARISON_OPERATOR( == )
DEFINE_PRICE_COMPARISON_OPERATOR( != )
DEFINE_PRICE_COMPARISON_OPERATOR( <  )
DEFINE_PRICE_COMPARISON_OPERATOR( <= )
DEFINE_PRICE_COMPARISON_OPERATOR( >  )
DEFINE_PRICE_COMPARISON_OPERATOR( >= )

      asset operator * ( const asset& a, const price& b )
      {
         if( a.symbol == b.base.symbol )
         {
            FC_ASSERT( b.base.amount.value > 0 );
            uint128_t result = (uint128_t(a.amount.value) * b.quote.amount.value)/b.base.amount.value;
            FC_ASSERT( result.hi == 0 );
            return asset( result.to_uint64(), b.quote.symbol );
         }
         else if( a.symbol == b.quote.symbol )
         {
            FC_ASSERT( b.quote.amount.value > 0 );
            uint128_t result = (uint128_t(a.amount.value) * b.base.amount.value)/b.quote.amount.value;
            FC_ASSERT( result.hi == 0 );
            return asset( result.to_uint64(), b.base.symbol );
         }
         FC_THROW_EXCEPTION( fc::assert_exception, "invalid asset * price", ("asset",a)("price",b) );
      }

      price operator / ( const asset& base, const asset& quote )
      { try {
         FC_ASSERT( base.symbol != quote.symbol );
         return price{ base, quote };
      } FC_CAPTURE_AND_RETHROW( (base)(quote) ) }

      price price::max( asset_symbol_type base, asset_symbol_type quote ) { return asset( share_type(XGT_MAX_SATOSHIS), base ) / asset( share_type(1), quote); }
      price price::min( asset_symbol_type base, asset_symbol_type quote ) { return asset( 1, base ) / asset( XGT_MAX_SATOSHIS, quote); }

      bool price::is_null() const { return *this == price(); }

      void price::validate() const
      { try {
         FC_ASSERT( base.amount > share_type(0) );
         FC_ASSERT( quote.amount > share_type(0) );
         FC_ASSERT( base.symbol != quote.symbol );
      } FC_CAPTURE_AND_RETHROW( (base)(quote) ) }

      std::string price::as_decimal() const
      { try {
         std::string decimal_price;

         double approx_real_price;
         if ( base.symbol == XGT_SYMBOL && quote.symbol.space() == asset_symbol_type::xtt_nai_space )
            approx_real_price = static_cast< double >( base.amount.value ) / static_cast< double >( quote.amount.value );
         else
            approx_real_price = static_cast< double >( quote.amount.value ) / static_cast< double >( base.amount.value );
         decimal_price = boost::lexical_cast< std::string >( approx_real_price );
         decimal_price += "?";

         return decimal_price;
      } FC_CAPTURE_AND_RETHROW( (base)(quote) ) }

      double price::as_real() const
      { try {
         auto decimal_price = as_decimal();
         if ( decimal_price.back() == '?' )
            decimal_price.pop_back();
         return boost::lexical_cast< double >( decimal_price );
      } FC_CAPTURE_AND_RETHROW( (base)(quote) ) }


} } // xgt::protocol

namespace fc {
   void to_variant( const xgt::protocol::asset& var, fc::variant& vo )
   {
      try
      {
         variant v = mutable_variant_object( ASSET_AMOUNT_KEY, boost::lexical_cast< std::string >( var.amount.value ) )
                                           ( ASSET_PRECISION_KEY, uint64_t( var.symbol.decimals() ) )
                                           ( ASSET_NAI_KEY, var.symbol.to_nai_string() );
         vo = v;
      } FC_CAPTURE_AND_RETHROW()
   }

   void from_variant( const fc::variant& var, xgt::protocol::asset& vo )
   {
      try
      {
         FC_ASSERT( var.is_object(), "Asset has to be treated as object." );

         const auto& v_object = var.get_object();

         FC_ASSERT( v_object.contains( ASSET_AMOUNT_KEY ), "Amount field doesn't exist." );
         FC_ASSERT( v_object[ ASSET_AMOUNT_KEY ].is_string(), "Expected a string type for value '${key}'.", ("key", ASSET_AMOUNT_KEY) );
         vo.amount = boost::lexical_cast< int64_t >( v_object[ ASSET_AMOUNT_KEY ].as< std::string >() );
         FC_ASSERT( vo.amount >= 0, "Asset amount cannot be negative" );

         FC_ASSERT( v_object.contains( ASSET_PRECISION_KEY ), "Precision field doesn't exist." );
         FC_ASSERT( v_object[ ASSET_PRECISION_KEY ].is_uint64(), "Expected an unsigned integer type for value '${key}'.", ("key", ASSET_PRECISION_KEY) );

         FC_ASSERT( v_object.contains( ASSET_NAI_KEY ), "NAI field doesn't exist." );
         FC_ASSERT( v_object[ ASSET_NAI_KEY ].is_string(), "Expected a string type for value '${key}'.", ("key", ASSET_NAI_KEY) );

         vo.symbol = xgt::protocol::asset_symbol_type::from_nai_string( v_object[ ASSET_NAI_KEY ].as< std::string >().c_str(), v_object[ ASSET_PRECISION_KEY ].as< uint8_t >() );
      } FC_CAPTURE_AND_RETHROW()
   }
}
