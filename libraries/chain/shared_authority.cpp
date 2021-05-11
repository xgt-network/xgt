#include <xgt/chain/shared_authority.hpp>

namespace xgt { namespace chain {

shared_authority::operator authority()const
{
   authority result;

   result.wallet_auths.reserve( wallet_auths.size() );
   for( const auto& item : wallet_auths )
      result.wallet_auths.insert( item );

   result.key_auths.reserve( key_auths.size() );
   for( const auto& item : key_auths )
      result.key_auths.insert( item );

   result.weight_threshold = weight_threshold;

   return result;
}

shared_authority& shared_authority::operator=( const authority& a )
{
   clear();

   for( const auto& item : a.wallet_auths )
      wallet_auths.insert( item );

   for( const auto& item : a.key_auths )
      key_auths.insert( item );

   weight_threshold = a.weight_threshold;

   return *this;
}

void shared_authority::add_authority( const public_key_type& k, weight_type w )
{
   key_auths[k] = w;
}

void shared_authority::add_authority( const wallet_name_type& k, weight_type w )
{
   wallet_auths[k] = w;
}

vector<public_key_type> shared_authority::get_keys()const
{
   vector<public_key_type> result;
   result.reserve( key_auths.size() );
   for( const auto& k : key_auths )
      result.push_back(k.first);
   return result;
}

bool shared_authority::is_impossible()const
{
   uint64_t auth_weights = 0;
   for( const auto& item : wallet_auths ) auth_weights += item.second;
   for( const auto& item : key_auths ) auth_weights += item.second;
   return auth_weights < weight_threshold;
}

uint32_t shared_authority::num_auths()const { return wallet_auths.size() + key_auths.size(); }

void shared_authority::clear() { wallet_auths.clear(); key_auths.clear(); }

void shared_authority::validate()const
{
   for( const auto& item : wallet_auths )
   {
      FC_ASSERT( protocol::is_valid_wallet_name( item.first ) );
   }
}

bool operator == ( const shared_authority& a, const shared_authority& b )
{
   return ( a.weight_threshold == b.weight_threshold ) &&
            ( a.wallet_auths == b.wallet_auths ) &&
            ( a.key_auths == b.key_auths );
}

bool operator == ( const authority& a, const shared_authority& b )
{
   return a == authority( b );
}

bool operator == ( const shared_authority& a, const authority& b )
{
   return authority( a ) == b;
}

} } // xgt::chain
