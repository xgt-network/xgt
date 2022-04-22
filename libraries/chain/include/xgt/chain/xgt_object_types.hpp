#pragma once

#include <xgt/chain/xgt_fwd.hpp>

#include <chainbase/chainbase.hpp>

#include <xgt/protocol/types.hpp>
#include <xgt/protocol/authority.hpp>

#include <xgt/chain/buffer_type.hpp>

#include <xgt/chain/multi_index_types.hpp>

#define XGT_STD_ALLOCATOR_CONSTRUCTOR( object_type )   \
   public:                                             \
      object_type () = default;


#define XGT_OBJECT_ID_TYPE( object ) typedef oid< object ## _object > object ## _id_type;

namespace xgt {

namespace protocol {

struct asset;
struct price;

}

namespace chain {

using chainbase::object;
using chainbase::oid;

using xgt::protocol::block_id_type;
using xgt::protocol::transaction_id_type;
using xgt::protocol::chain_id_type;
using xgt::protocol::wallet_name_type;
using xgt::protocol::share_type;

using std::string;

inline std::string to_string( const std::string& str ) { return std::string( str.begin(), str.end() ); }
inline void from_string( std::string& out, const string& in ){ out.assign( in.begin(), in.end() ); }

struct by_id;
struct by_name;

enum object_type
{
   dynamic_global_property_object_type,
   wallet_object_type,
   account_metadata_object_type,
   account_authority_object_type,
   contract_object_type,
   contract_log_object_type,
   contract_receipt_object_type,
   contract_storage_object_type,
   witness_object_type,
   transaction_object_type,
   block_summary_object_type,
   witness_schedule_object_type,
   comment_object_type,
   comment_content_object_type,
   comment_vote_object_type,
   witness_vote_object_type,
   feed_history_object_type,
   convert_request_object_type,
   operation_object_type,
   account_history_object_type,
   hardfork_property_object_type,
   withdraw_vesting_route_object_type,
   recovery_authority_history_object_type,
   account_recovery_request_object_type,
   change_recovery_account_request_object_type,
   escrow_object_type,
   savings_withdraw_object_type,
   decline_voting_rights_request_object_type,
   block_stats_object_type,
   vesting_delegation_object_type,
   vesting_delegation_expiration_object_type,
   pending_required_action_object_type,
   pending_optional_action_object_type,
   proposal_object_type,
   proposal_vote_object_type,
   // XTT objects
   xtt_token_object_type,
   account_regular_balance_object_type,
   nai_pool_object_type,
   xtt_token_emissions_object_type,
   xtt_contribution_object_type,
   xtt_ico_object_type,
   xtt_ico_tier_object_type
};

class dynamic_global_property_object;
class wallet_object;
class account_metadata_object;
class account_authority_object;
class contract_object;
class contract_storage_object;
class witness_object;
class transaction_object;
class block_summary_object;
class witness_schedule_object;
class comment_object;
class comment_content_object;
class comment_vote_object;
class witness_vote_object;
class feed_history_object;
class convert_request_object;
class operation_object;
class account_history_object;
class hardfork_property_object;
class withdraw_vesting_route_object;
class recovery_authority_history_object;
class account_recovery_request_object;
class change_recovery_account_request_object;
class escrow_object;
class savings_withdraw_object;
class decline_voting_rights_request_object;
class block_stats_object;
class vesting_delegation_object;
class vesting_delegation_expiration_object;
class pending_required_action_object;
class pending_optional_action_object;

class xtt_token_object;
class account_regular_balance_object;
class nai_pool_object;
class xtt_token_emissions_object;
class xtt_contribution_object;
class xtt_ico_object;
class xtt_ico_tier_object;

class proposal_object;
class proposal_vote_object;

typedef oid< dynamic_global_property_object         > dynamic_global_property_id_type;
typedef oid< wallet_object                          > wallet_id_type;
typedef oid< account_metadata_object                > account_metadata_id_type;
typedef oid< account_authority_object               > account_authority_id_type;
typedef oid< contract_object                        > contract_id_type;
typedef oid< contract_storage_object                > contract_storage_id_type;
typedef oid< witness_object                         > witness_id_type;
typedef oid< transaction_object                     > transaction_object_id_type;
typedef oid< block_summary_object                   > block_summary_id_type;
typedef oid< witness_schedule_object                > witness_schedule_id_type;
typedef oid< comment_object                         > comment_id_type;
typedef oid< comment_content_object                 > comment_content_id_type;
typedef oid< comment_vote_object                    > comment_vote_id_type;
typedef oid< witness_vote_object                    > witness_vote_id_type;
typedef oid< feed_history_object                    > feed_history_id_type;
typedef oid< convert_request_object                 > convert_request_id_type;
typedef oid< operation_object                       > operation_id_type;
typedef oid< account_history_object                 > account_history_id_type;
typedef oid< hardfork_property_object               > hardfork_property_id_type;
typedef oid< withdraw_vesting_route_object          > withdraw_vesting_route_id_type;
typedef oid< recovery_authority_history_object      > recovery_authority_history_id_type;
typedef oid< account_recovery_request_object        > account_recovery_request_id_type;
typedef oid< change_recovery_account_request_object > change_recovery_account_request_id_type;
typedef oid< escrow_object                          > escrow_id_type;
typedef oid< savings_withdraw_object                > savings_withdraw_id_type;
typedef oid< decline_voting_rights_request_object   > decline_voting_rights_request_id_type;
typedef oid< block_stats_object                     > block_stats_id_type;
typedef oid< vesting_delegation_object              > vesting_delegation_id_type;
typedef oid< vesting_delegation_expiration_object   > vesting_delegation_expiration_id_type;
typedef oid< pending_required_action_object         > pending_required_action_id_type;
typedef oid< pending_optional_action_object         > pending_optional_action_id_type;

typedef oid< xtt_token_object                       > xtt_token_id_type;
typedef oid< account_regular_balance_object         > account_regular_balance_id_type;
typedef oid< nai_pool_object                        > nai_pool_id_type;
typedef oid< xtt_token_emissions_object             > xtt_token_emissions_object_id_type;
typedef oid< xtt_contribution_object                > xtt_contribution_object_id_type;
typedef oid< xtt_ico_object                         > xtt_ico_object_id_type;
typedef oid< xtt_ico_tier_object                    > xtt_ico_tier_object_id_type;

typedef oid< proposal_object > proposal_id_type;
typedef oid< proposal_vote_object > proposal_vote_id_type;

enum bandwidth_type
{
   post,    ///< Rate limiting posting reward eligibility over time
   forum,   ///< Rate limiting for all forum related actins
   market   ///< Rate limiting for all other actions
};

} } //xgt::chain

namespace mira {

template< typename T > struct is_static_length< chainbase::oid< T > > : public boost::true_type {};
template< typename T > struct is_static_length< fc::fixed_string< T > > : public boost::true_type {};
template<> struct is_static_length< xgt::protocol::wallet_name_type > : public boost::true_type {};
template<> struct is_static_length< xgt::protocol::asset_symbol_type > : public boost::true_type {};
template<> struct is_static_length< xgt::protocol::asset > : public boost::true_type {};
template<> struct is_static_length< xgt::protocol::price > : public boost::true_type {};

} // mira

namespace fc
{
class variant;

template<typename T>
void to_variant( const chainbase::oid<T>& var,  variant& vo )
{
   vo = var._id;
}

template<typename T>
void from_variant( const variant& vo, chainbase::oid<T>& var )
{
   var._id = vo.as_int64();
}

template< typename T >
struct get_typename< chainbase::oid< T > >
{
   static const char* name()
   {
      static std::string n = std::string( "chainbase::oid<" ) + get_typename< T >::name() + ">";
      return n.c_str();
   }
};

namespace raw
{

template<typename Stream, typename T>
void pack( Stream& s, const chainbase::oid<T>& id )
{
   s.write( (const char*)&id._id, sizeof(id._id) );
}

template<typename Stream, typename T>
void unpack( Stream& s, chainbase::oid<T>& id, uint32_t )
{
   s.read( (char*)&id._id, sizeof(id._id));
}

template< typename Stream, typename E, typename A >
void pack( Stream& s, const boost::container::deque<E, A>& dq )
{
   // This could be optimized
   std::vector<E> temp;
   std::copy( dq.begin(), dq.end(), std::back_inserter(temp) );
   pack( s, temp );
}

template< typename Stream, typename E, typename A >
void unpack( Stream& s, boost::container::deque<E, A>& dq, uint32_t depth )
{
   depth++;
   FC_ASSERT( depth <= MAX_RECURSION_DEPTH );
   // This could be optimized
   std::vector<E> temp;
   unpack( s, temp, depth );
   dq.clear();
   std::copy( temp.begin(), temp.end(), std::back_inserter(dq) );
}

template< typename Stream, typename K, typename V, typename C, typename A >
void pack( Stream& s, const boost::container::flat_map< K, V, C, A >& value )
{
   fc::raw::pack( s, unsigned_int((uint32_t)value.size()) );
   auto itr = value.begin();
   auto end = value.end();
   while( itr != end )
   {
      fc::raw::pack( s, *itr );
      ++itr;
   }
}

template< typename Stream, typename K, typename V, typename C, typename A >
void unpack( Stream& s, boost::container::flat_map< K, V, C, A >& value, uint32_t depth )
{
   depth++;
   FC_ASSERT( depth <= MAX_RECURSION_DEPTH );
   unsigned_int size;
   unpack( s, size, depth );
   value.clear();
   FC_ASSERT( size.value*(sizeof(K)+sizeof(V)) < MAX_ARRAY_ALLOC_SIZE );
   for( uint32_t i = 0; i < size.value; ++i )
   {
      std::pair<K,V> tmp;
      fc::raw::unpack( s, tmp, depth );
      value.insert( std::move(tmp) );
   }
}


}



} // namespace fc::raw

FC_REFLECT_ENUM( xgt::chain::object_type,
                 (dynamic_global_property_object_type)
                 (wallet_object_type)
                 (account_metadata_object_type)
                 (account_authority_object_type)
                 (contract_object_type)
                 (contract_log_object_type)
                 (contract_receipt_object_type)
                 (contract_storage_object_type)
                 (witness_object_type)
                 (transaction_object_type)
                 (block_summary_object_type)
                 (witness_schedule_object_type)
                 (comment_object_type)
                 (comment_content_object_type)
                 (comment_vote_object_type)
                 (witness_vote_object_type)
                 (feed_history_object_type)
                 (convert_request_object_type)
                 (operation_object_type)
                 (account_history_object_type)
                 (hardfork_property_object_type)
                 (withdraw_vesting_route_object_type)
                 (recovery_authority_history_object_type)
                 (account_recovery_request_object_type)
                 (change_recovery_account_request_object_type)
                 (escrow_object_type)
                 (savings_withdraw_object_type)
                 (decline_voting_rights_request_object_type)
                 (block_stats_object_type)
                 (vesting_delegation_object_type)
                 (vesting_delegation_expiration_object_type)
                 (pending_required_action_object_type)
                 (pending_optional_action_object_type)
                 (proposal_object_type)
                 (proposal_vote_object_type)
                 (xtt_token_object_type)
                 (account_regular_balance_object_type)
                 (nai_pool_object_type)
                 (xtt_token_emissions_object_type)
                 (xtt_contribution_object_type)
                 (xtt_ico_object_type)
                 (xtt_ico_tier_object_type)
               )

FC_REFLECT_ENUM( xgt::chain::bandwidth_type, (post)(forum)(market) )
