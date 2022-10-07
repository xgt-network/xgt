#include <xgt/chain/xgt_fwd.hpp>

#include <appbase/application.hpp>

#include <xgt/plugins/database_api/database_api.hpp>
#include <xgt/plugins/database_api/database_api_plugin.hpp>

#include <xgt/protocol/get_config.hpp>
#include <xgt/protocol/exceptions.hpp>
#include <xgt/protocol/transaction_util.hpp>

#include <xgt/chain/util/xtt_token.hpp>

#include <xgt/utilities/git_revision.hpp>

#include <fc/git_revision.hpp>

namespace xgt { namespace plugins { namespace database_api {



class database_api_impl
{
   public:
      database_api_impl();
      ~database_api_impl();

      DECLARE_API_IMPL
      (
         (get_config)
         (get_version)
         (get_dynamic_global_properties)
         (get_hardfork_properties)
         (list_witnesses)
         (find_witnesses)
         (list_wallets)
         (find_wallets)
         (list_escrows)
         (find_escrows)
         (list_comments)
         (find_comments)
         (list_votes)
         (find_votes)
         (get_transaction_hex)
         (get_required_signatures)
         (get_potential_signatures)
         (verify_authority)
         (verify_account_authority)
         (verify_signatures)
         (get_nai_pool)
         (get_xtt_balances)
         (list_xtt_contributions)
         (find_xtt_contributions)
         (list_xtt_tokens)
         (find_xtt_tokens)
         (list_xtt_token_balances)
         (find_xtt_token_balances)
      )

      template< typename ResultType >
      static ResultType on_push_default( const ResultType& r ) { return r; }

      template< typename ValueType >
      static bool filter_default( const ValueType& r ) { return true; }

      template<typename IndexType, typename OrderType, typename StartType, typename ResultType, typename OnPushType, typename FilterType>
      void iterate_results(
         StartType start,
         std::vector<ResultType>& result,
         uint32_t limit,
         OnPushType&& on_push,
         FilterType&& filter,
         order_direction_type direction = ascending )
      {
         const auto& idx = _db.get_index< IndexType, OrderType >();
         if( direction == ascending )
         {
            auto itr = idx.lower_bound( start );
            auto end = idx.end();

            while( result.size() < limit && itr != end )
            {
               if( filter( *itr ) )
                  result.push_back( on_push( *itr ) );

               ++itr;
            }
         }
         else if( direction == descending )
         {
            auto itr = boost::make_reverse_iterator( idx.lower_bound( start ) );
            auto end = idx.rend();

            while( result.size() < limit && itr != end )
            {
               if( filter( *itr ) )
                  result.push_back( on_push( *itr ) );

               ++itr;
            }
         }
      }

      chain::database& _db;
};

//////////////////////////////////////////////////////////////////////
//                                                                  //
// Constructors                                                     //
//                                                                  //
//////////////////////////////////////////////////////////////////////

database_api::database_api()
   : my( new database_api_impl() )
{
   JSON_RPC_REGISTER_API( XGT_DATABASE_API_PLUGIN_NAME );
}

database_api::~database_api() {}

database_api_impl::database_api_impl()
   : _db( appbase::app().get_plugin< xgt::plugins::chain::chain_plugin >().db() ) {}

database_api_impl::~database_api_impl() {}

//////////////////////////////////////////////////////////////////////
//                                                                  //
// Globals                                                          //
//                                                                  //
//////////////////////////////////////////////////////////////////////


DEFINE_API_IMPL( database_api_impl, get_config )
{
   return xgt::protocol::get_config();
}

DEFINE_API_IMPL( database_api_impl, get_version )
{
   return get_version_return
   (
      fc::string( xgt::utilities:git_revision_description)
      fc::string( XGT_BLOCKCHAIN_VERSION ),
      fc::string( xgt::utilities::git_revision_sha ),
      fc::string( fc::git_revision_sha ),
      _db.get_chain_id()
   );
}

DEFINE_API_IMPL( database_api_impl, get_dynamic_global_properties )
{
   return _db.get_dynamic_global_properties();
}

DEFINE_API_IMPL( database_api_impl, get_hardfork_properties )
{
   return _db.get_hardfork_property_object();
}


//////////////////////////////////////////////////////////////////////
//                                                                  //
// Witnesses                                                        //
//                                                                  //
//////////////////////////////////////////////////////////////////////

DEFINE_API_IMPL( database_api_impl, list_witnesses )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_witnesses_return result;
   result.witnesses.reserve( args.limit );

   switch( args.order )
   {
      case( by_name ):
      {
         iterate_results< chain::witness_index, chain::by_name >(
            args.start.as< protocol::wallet_name_type >(),
            result.witnesses,
            args.limit,
            [&]( const witness_object& w ){ return api_witness_object( w ); },
            &database_api_impl::filter_default< witness_object > );
         break;
      }
      case( by_vote_name ):
      {
         auto key = args.start.as< std::pair< share_type, wallet_name_type > >();
         iterate_results< chain::witness_index, chain::by_vote_name >(
            boost::make_tuple( key.first, key.second ),
            result.witnesses,
            args.limit,
            [&]( const witness_object& w ){ return api_witness_object( w ); },
            &database_api_impl::filter_default< witness_object > );
         break;
      }
      case( by_schedule_time ):
      {
         auto key = args.start.as< std::pair< fc::uint128, wallet_name_type > >();
         auto wit_id = _db.get< chain::witness_object, chain::by_name >( key.second ).id;
         iterate_results< chain::witness_index, chain::by_schedule_time >(
            boost::make_tuple( key.first, wit_id ),
            result.witnesses,
            args.limit,
            [&]( const witness_object& w ){ return api_witness_object( w ); },
            &database_api_impl::filter_default< witness_object > );
         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_witnesses )
{
   FC_ASSERT( args.miners.size() <= DATABASE_API_SINGLE_QUERY_LIMIT );

   find_witnesses_return result;

   for( auto& o : args.miners )
   {
      auto witness = _db.find< chain::witness_object, chain::by_name >( o );

      if( witness != nullptr )
         result.miners.push_back( api_witness_object( *witness ) );
   }

   return result;
}


//////////////////////////////////////////////////////////////////////
//                                                                  //
// Accounts                                                         //
//                                                                  //
//////////////////////////////////////////////////////////////////////

/* Accounts */

DEFINE_API_IMPL( database_api_impl, list_wallets )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_wallets_return result;
   result.wallets.reserve( args.limit );

   switch( args.order )
   {
      case( by_name ):
      {
         iterate_results< chain::wallet_index, chain::by_name >(
            args.start.as< protocol::wallet_name_type >(),
            result.wallets,
            args.limit,
            [&]( const wallet_object& a ){ return api_wallet_object( a, _db ); },
            &database_api_impl::filter_default< wallet_object > );
         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_wallets )
{
   find_wallets_return result;
   FC_ASSERT( args.wallets.size() <= DATABASE_API_SINGLE_QUERY_LIMIT );

   for( auto& a : args.wallets )
   {
      auto acct = _db.find< chain::wallet_object, chain::by_name >( a );
      if( acct != nullptr )
         result.wallets.push_back( api_wallet_object( *acct, _db ) );
   }

   return result;
}


/* Escrows */

DEFINE_API_IMPL( database_api_impl, list_escrows )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_escrows_return result;
   result.escrows.reserve( args.limit );

   switch( args.order )
   {
      case( by_from_id ):
      {
         auto key = args.start.as< std::pair< wallet_name_type, uint32_t > >();
         iterate_results< chain::escrow_index, chain::by_from_id >(
            boost::make_tuple( key.first, key.second ),
            result.escrows,
            args.limit,
            &database_api_impl::on_push_default< escrow_object >,
            &database_api_impl::filter_default< escrow_object > );
         break;
      }
      case( by_ratification_deadline ):
      {
         auto key = args.start.as< std::vector< fc::variant > >();
         FC_ASSERT( key.size() == 3, "by_ratification_deadline start requires 3 values. (bool, time_point_sec, escrow_id_type)" );
         iterate_results< chain::escrow_index, chain::by_ratification_deadline >(
            boost::make_tuple( key[0].as< bool >(), key[1].as< fc::time_point_sec >(), key[2].as< escrow_id_type >() ),
            result.escrows,
            args.limit,
            &database_api_impl::on_push_default< escrow_object >,
            &database_api_impl::filter_default< escrow_object > );
         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_escrows )
{
   find_escrows_return result;

   const auto& escrow_idx = _db.get_index< chain::escrow_index, chain::by_from_id >();
   auto itr = escrow_idx.lower_bound( args.from );

   while( itr != escrow_idx.end() && itr->from == args.from && result.escrows.size() <= DATABASE_API_SINGLE_QUERY_LIMIT )
   {
      result.escrows.push_back( *itr );
      ++itr;
   }

   return result;
}


//////////////////////////////////////////////////////////////////////
//                                                                  //
// Comments                                                         //
//                                                                  //
//////////////////////////////////////////////////////////////////////

/* Comments */

DEFINE_API_IMPL( database_api_impl, list_comments )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_comments_return result;
   result.comments.reserve( args.limit );

   switch( args.order )
   {
      case( by_permlink ):
      {
         auto key = args.start.as< std::pair< wallet_name_type, string > >();
         iterate_results< chain::comment_index, chain::by_permlink >(
            boost::make_tuple( key.first, key.second ),
            result.comments,
            args.limit,
            [&]( const comment_object& c ){ return api_comment_object( c, _db ); },
            &database_api_impl::filter_default< comment_object > );
         break;
      }
      case( by_root ):
      {
         auto key = args.start.as< vector< fc::variant > >();
         FC_ASSERT( key.size() == 4, "by_root start requires 4 values. (wallet_name_type, string, wallet_name_type, string)" );

         auto root_author = key[0].as< wallet_name_type >();
         auto root_permlink = key[1].as< string >();
         comment_id_type root_id;

         if( root_author != wallet_name_type() || root_permlink.size() )
         {
            auto root = _db.find< chain::comment_object, chain::by_permlink >( boost::make_tuple( root_author, root_permlink ) );
            FC_ASSERT( root != nullptr, "Could not find comment ${a}/${p}.", ("a", root_author)("p", root_permlink) );
            root_id = root->id;
         }

         auto child_author = key[2].as< wallet_name_type >();
         auto child_permlink = key[3].as< string >();
         comment_id_type child_id;

         if( child_author != wallet_name_type() || child_permlink.size() )
         {
            auto child = _db.find< chain::comment_object, chain::by_permlink >( boost::make_tuple( child_author, child_permlink ) );
            FC_ASSERT( child != nullptr, "Could not find comment ${a}/${p}.", ("a", child_author)("p", child_permlink) );
            child_id = child->id;
         }

         iterate_results< chain::comment_index, chain::by_root >(
            boost::make_tuple( root_id, child_id ),
            result.comments,
            args.limit,
            [&]( const comment_object& c ){ return api_comment_object( c, _db ); },
            &database_api_impl::filter_default< comment_object > );
         break;
      }
      case( by_parent ):
      {
         auto key = args.start.as< vector< fc::variant > >();
         FC_ASSERT( key.size() == 4, "by_parent start requires 4 values. (wallet_name_type, string, wallet_name_type, string)" );

         auto child_author = key[2].as< wallet_name_type >();
         auto child_permlink = key[3].as< string >();
         comment_id_type child_id;

         if( child_author != wallet_name_type() || child_permlink.size() )
         {
            auto child = _db.find< chain::comment_object, chain::by_permlink >( boost::make_tuple( child_author, child_permlink ) );
            FC_ASSERT( child != nullptr, "Could not find comment ${a}/${p}.", ("a", child_author)("p", child_permlink) );
            child_id = child->id;
         }

         iterate_results< chain::comment_index, chain::by_parent >(
            boost::make_tuple( key[0].as< wallet_name_type >(), key[1].as< string >(), child_id ),
            result.comments,
            args.limit,
            [&]( const comment_object& c ){ return api_comment_object( c, _db ); },
            &database_api_impl::filter_default< comment_object > );
         break;
      }
#ifndef IS_LOW_MEM
      case( by_last_update ):
      {
         auto key = args.start.as< vector< fc::variant > >();
         FC_ASSERT( key.size() == 4, "by_last_update start requires 4 values. (wallet_name_type, time_point_sec, wallet_name_type, string)" );

         auto child_author = key[2].as< wallet_name_type >();
         auto child_permlink = key[3].as< string >();
         comment_id_type child_id;

         if( child_author != wallet_name_type() || child_permlink.size() )
         {
            auto child = _db.find< chain::comment_object, chain::by_permlink >( boost::make_tuple( child_author, child_permlink ) );
            FC_ASSERT( child != nullptr, "Could not find comment ${a}/${p}.", ("a", child_author)("p", child_permlink) );
            child_id = child->id;
         }

         iterate_results< chain::comment_index, chain::by_last_update >(
            boost::make_tuple( key[0].as< wallet_name_type >(), key[1].as< fc::time_point_sec >(), child_id ),
            result.comments,
            args.limit,
            [&]( const comment_object& c ){ return api_comment_object( c, _db ); },
            &database_api_impl::filter_default< comment_object > );
         break;
      }
      case( by_author_last_update ):
      {
         auto key = args.start.as< vector< fc::variant > >();
         FC_ASSERT( key.size() == 4, "by_author_last_update start requires 4 values. (wallet_name_type, time_point_sec, wallet_name_type, string)" );

         auto author = key[2].as< wallet_name_type >();
         auto permlink = key[3].as< string >();
         comment_id_type comment_id;

         if( author != wallet_name_type() || permlink.size() )
         {
            auto comment = _db.find< chain::comment_object, chain::by_permlink >( boost::make_tuple( author, permlink ) );
            FC_ASSERT( comment != nullptr, "Could not find comment ${a}/${p}.", ("a", author)("p", permlink) );
            comment_id = comment->id;
         }

         iterate_results< chain::comment_index, chain::by_last_update >(
            boost::make_tuple( key[0].as< wallet_name_type >(), key[1].as< fc::time_point_sec >(), comment_id ),
            result.comments,
            args.limit,
            [&]( const comment_object& c ){ return api_comment_object( c, _db ); },
            &database_api_impl::filter_default< comment_object > );
         break;
      }
#endif
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_comments )
{
   FC_ASSERT( args.comments.size() <= DATABASE_API_SINGLE_QUERY_LIMIT );
   find_comments_return result;
   result.comments.reserve( args.comments.size() );

   for( auto& key: args.comments )
   {
      auto comment = _db.find< chain::comment_object, chain::by_permlink >( boost::make_tuple( key.first, key.second ) );

      if( comment != nullptr )
         result.comments.push_back( api_comment_object( *comment, _db ) );
   }

   return result;
}

//====================================================last_votes_misc====================================================

namespace last_votes_misc
{

   //====================================================votes_impl====================================================
   template< sort_order_type SORTORDERTYPE >
   void votes_impl( database_api_impl& _impl, vector< api_comment_vote_object >& c, uint32_t limit, vector< fc::variant >& key, fc::time_point_sec& timestamp, uint64_t weight )
   {
      if( SORTORDERTYPE == by_comment_voter_symbol )
         FC_ASSERT( key.size() == 3 || key.size() == 4, "by_comment_voter_symbol start requires 3-4 values. (wallet_name_type, string, wallet_name_type, asset_symbol_type)" );
      else
         FC_ASSERT( key.size() == 3 || key.size() == 4, "by_voter_comment_symbol start requires 3-4 values. (wallet_name_type, wallet_name_type, string)" );

      wallet_name_type voter;
      wallet_name_type author;
      string permlink;

      wallet_id_type voter_id;
      comment_id_type comment_id;

      if( SORTORDERTYPE == by_comment_voter_symbol )
      {
         author = key[0].as< wallet_name_type >();
         permlink = key[1].as< string >();
         voter = key[ 2 ].as< wallet_name_type >();
      }
      else
      {
         voter = key[0].as< wallet_name_type >();
         author = key[ 1 ].as< wallet_name_type >();
         permlink = key[ 2 ].as< string >();
      }

      if( voter != wallet_name_type() )
      {
         auto account = _impl._db.find< chain::wallet_object, chain::by_name >( voter );
         FC_ASSERT( account != nullptr, "Could not find voter ${v}.", ("v", voter ) );
         voter_id = account->id;
      }

      if( author != wallet_name_type() || permlink.size() )
      {
         auto comment = _impl._db.find< chain::comment_object, chain::by_permlink >( boost::make_tuple( author, permlink ) );
         FC_ASSERT( comment != nullptr, "Could not find comment ${a}/${p}.", ("a", author)("p", permlink) );
         comment_id = comment->id;
      }

      asset_symbol_type start_symbol = XGT_SYMBOL;

      if( key.size() == 4 )
      {
         start_symbol = key[3].as< asset_symbol_type >();
      }

      if( SORTORDERTYPE == by_comment_voter_symbol )
      {
         _impl.iterate_results< chain::comment_vote_index, chain::by_comment_voter_symbol >(
         boost::make_tuple( comment_id, voter_id ),
         c,
         limit,
         [&]( const comment_vote_object& cv ){ return api_comment_vote_object( cv, _impl._db ); },
         &database_api_impl::filter_default< comment_vote_object > );
      }
      else if( SORTORDERTYPE == by_voter_comment )
      {
         _impl.iterate_results< chain::comment_vote_index, chain::by_voter_comment_symbol >(
         boost::make_tuple( voter_id, comment_id ),
         c,
         limit,
         [&]( const comment_vote_object& cv ){ return api_comment_vote_object( cv, _impl._db ); },
         &database_api_impl::filter_default< comment_vote_object > );
      }
   }

}//namespace last_votes_misc

//====================================================last_votes_misc====================================================

/* Votes */

DEFINE_API_IMPL( database_api_impl, list_votes )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   auto key = args.start.as< vector< fc::variant > >();

   list_votes_return result;
   result.votes.reserve( args.limit );

   switch( args.order )
   {
      case( by_comment_voter ):
      case( by_comment_voter_symbol ):
      {
         static fc::time_point_sec t( -1 );
         last_votes_misc::votes_impl< by_comment_voter_symbol >( *this, result.votes, args.limit, key, t, 0 );
         break;
      }
      case( by_voter_comment ):
      case( by_voter_comment_symbol ):
      {
         static fc::time_point_sec t( -1 );
         last_votes_misc::votes_impl< by_voter_comment_symbol >( *this, result.votes, args.limit, key, t, 0 );
         break;
      }
      case( by_comment_symbol_voter ):
      {
         auto key = args.start.as< vector< fc::variant > >();
         FC_ASSERT( key.size() == 3 || key.size() == 4, "by_comment_symbol_voter start requires 3-4 values. (wallet_name_type, string, asset_symbol_type, wallet_name_type)" );

         auto author = key[0].as< wallet_name_type >();
         auto permlink = key[1].as< string >();
         comment_id_type comment_id;

         if( author != wallet_name_type() || permlink.size() )
         {
            auto comment = _db.find< chain::comment_object, chain::by_permlink >( boost::make_tuple( author, permlink ) );
            FC_ASSERT( comment != nullptr, "Could not find comment ${a}/${p}.", ("a", author)("p", permlink) );
            comment_id = comment->id;
         }

         asset_symbol_type start_symbol = key[2].as< asset_symbol_type >();
         wallet_id_type voter_id;

         if( key.size() == 4 )
         {
            auto voter = key[3].as< wallet_name_type >();

            if( voter != wallet_name_type() )
            {
               auto account = _db.find< chain::wallet_object, chain::by_name >( voter );
               FC_ASSERT( account != nullptr, "Could not find voter ${v}.", ("v", voter ) );
               voter_id = account->id;
            }
         }

         iterate_results< chain::comment_vote_index, chain::by_comment_symbol_voter >(
            boost::make_tuple( comment_id, start_symbol, voter_id ),
            result.votes,
            args.limit,
            [&]( const comment_vote_object& cv ){ return api_comment_vote_object( cv, _db ); },
            &database_api_impl::filter_default< comment_vote_object > );
         break;
      }
      case( by_voter_symbol_comment ):
      {
         auto key = args.start.as< vector< fc::variant > >();
         FC_ASSERT( key.size() == 2 || key.size() == 4, "by_voter_symbol_comment start requires 2 or 4 values. (wallet_name_type, asset_symbol_type, wallet_name_type, string)" );

         wallet_id_type voter_id;
         auto voter = key[0].as< wallet_name_type >();

         if( voter != wallet_name_type() )
         {
            auto account = _db.find< chain::wallet_object, chain::by_name >( voter );
            FC_ASSERT( account != nullptr, "Could not find voter ${v}.", ("v", voter ) );
            voter_id = account->id;
         }

         asset_symbol_type start_symbol = key[1].as< asset_symbol_type >();

         comment_id_type comment_id;

         if( key.size() == 4 )
         {
            auto author = key[2].as< wallet_name_type >();
            auto permlink = key[3].as< string >();

            if( author != wallet_name_type() || permlink.size() )
            {
               auto comment = _db.find< chain::comment_object, chain::by_permlink >( boost::make_tuple( author, permlink ) );
               FC_ASSERT( comment != nullptr, "Could not find comment ${a}/${p}.", ("a", author)("p", permlink) );
               comment_id = comment->id;
            }
         }

         iterate_results< chain::comment_vote_index, chain::by_voter_symbol_comment >(
            boost::make_tuple( voter_id, start_symbol, comment_id ),
            result.votes,
            args.limit,
            [&]( const comment_vote_object& cv ){ return api_comment_vote_object( cv, _db ); },
            &database_api_impl::filter_default< comment_vote_object > );
         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_votes )
{
   find_votes_return result;

   auto comment = _db.find< chain::comment_object, chain::by_permlink >( boost::make_tuple( args.author, args.permlink, args.symbol ) );
   FC_ASSERT( comment != nullptr, "Could not find comment ${a}/${p}", ("a", args.author)("p", args.permlink ) );

   const auto& vote_idx = _db.get_index< chain::comment_vote_index, chain::by_comment_voter_symbol >();
   auto itr = vote_idx.lower_bound( comment->id );

   while( itr != vote_idx.end() && itr->comment == comment->id && result.votes.size() <= DATABASE_API_SINGLE_QUERY_LIMIT )
   {
      result.votes.push_back( api_comment_vote_object( *itr, _db ) );
      ++itr;
   }

   return result;
}


//////////////////////////////////////////////////////////////////////
//                                                                  //
// Authority / Validation                                           //
//                                                                  //
//////////////////////////////////////////////////////////////////////

DEFINE_API_IMPL( database_api_impl, get_transaction_hex )
{
   return get_transaction_hex_return( { fc::to_hex( fc::raw::pack_to_vector( args.trx ) ) } );
}

DEFINE_API_IMPL( database_api_impl, get_required_signatures )
{
   get_required_signatures_return result;
   result.keys = args.trx.get_required_signatures( _db.get_chain_id(),
                                                   args.available_keys,
                                                   [&]( string wallet_name ){ return authority( _db.get< chain::account_authority_object, chain::by_account >( wallet_name ).money ); },
                                                   [&]( string wallet_name ){ return authority( _db.get< chain::account_authority_object, chain::by_account >( wallet_name ).recovery ); },
                                                   [&]( string wallet_name ){ return authority( _db.get< chain::account_authority_object, chain::by_account >( wallet_name ).social ); },
                                                   XGT_MAX_SIG_CHECK_DEPTH,
                                                   fc::ecc::canonical_signature_type::bip_0062 );

   return result;
}

DEFINE_API_IMPL( database_api_impl, get_potential_signatures )
{
   get_potential_signatures_return result;
   args.trx.get_required_signatures(
      _db.get_chain_id(),
      flat_set< public_key_type >(),
      [&]( wallet_name_type wallet_name )
      {
         const auto& auth = _db.get< chain::account_authority_object, chain::by_account >( wallet_name ).money;
         for( const auto& k : auth.get_keys() )
            result.keys.insert( k );
         return authority( auth );
      },
      [&]( wallet_name_type wallet_name )
      {
         const auto& auth = _db.get< chain::account_authority_object, chain::by_account >( wallet_name ).recovery;
         for( const auto& k : auth.get_keys() )
            result.keys.insert( k );
         return authority( auth );
      },
      [&]( wallet_name_type wallet_name )
      {
         const auto& auth = _db.get< chain::account_authority_object, chain::by_account >( wallet_name ).social;
         for( const auto& k : auth.get_keys() )
            result.keys.insert( k );
         return authority( auth );
      },
      XGT_MAX_SIG_CHECK_DEPTH,
      fc::ecc::canonical_signature_type::bip_0062
   );

   return result;
}

DEFINE_API_IMPL( database_api_impl, verify_authority )
{
   if( args.auth.valid() )
   {
         args.trx.verify_authority(_db.get_chain_id(),
                              [&]( string wallet_name ){ return *(args.auth); },
                              [&]( string wallet_name ){ return *(args.auth); },
                              [&]( string wallet_name ){ return *(args.auth); },
                              XGT_MAX_SIG_CHECK_DEPTH,
                              XGT_MAX_AUTHORITY_MEMBERSHIP,
                              XGT_MAX_SIG_CHECK_WALLETS,
                              fc::ecc::canonical_signature_type::bip_0062);
   }
   else
   {
         args.trx.verify_authority(_db.get_chain_id(),
                              [&]( string wallet_name ){ return authority( _db.get< chain::account_authority_object, chain::by_account >( wallet_name ).money    ); },
                              [&]( string wallet_name ){ return authority( _db.get< chain::account_authority_object, chain::by_account >( wallet_name ).recovery ); },
                              [&]( string wallet_name ){ return authority( _db.get< chain::account_authority_object, chain::by_account >( wallet_name ).social   ); },
                              XGT_MAX_SIG_CHECK_DEPTH,
                              XGT_MAX_AUTHORITY_MEMBERSHIP,
                              XGT_MAX_SIG_CHECK_WALLETS,
                              fc::ecc::canonical_signature_type::bip_0062);
   }
   return verify_authority_return( { true } );
}

// TODO: This is broken. By the look of is, it has been since BitShares. verify_authority always
// returns false because the TX is not signed.
DEFINE_API_IMPL( database_api_impl, verify_account_authority )
{
   auto account = _db.find< chain::wallet_object, chain::by_name >( args.account );
   FC_ASSERT( account != nullptr, "no such account" );

   /// reuse trx.verify_authority by creating a dummy transfer
   verify_authority_args vap;
   transfer_operation op;
   op.from = account->name;
   vap.trx.operations.emplace_back( op );

   return verify_authority( vap );
}

DEFINE_API_IMPL( database_api_impl, verify_signatures )
{
   // get_signature_keys can throw for dup sigs. Allow this to throw.
   flat_set< public_key_type > sig_keys;
   for( const auto&  sig : args.signatures )
   {
      XGT_ASSERT(
         sig_keys.insert( fc::ecc::public_key( sig, args.hash ) ).second,
         protocol::tx_duplicate_sig,
         "Duplicate Signature detected" );
   }

   verify_signatures_return result;
   result.valid = true;

   // verify authority throws on failure, catch and return false
   try
   {
      xgt::protocol::verify_authority< verify_signatures_args >(
         { args },
         sig_keys,
         [this]( const string& name ) { return authority( _db.get< chain::account_authority_object, chain::by_account >( name ).recovery ); },
         [this]( const string& name ) { return authority( _db.get< chain::account_authority_object, chain::by_account >( name ).money ); },
         [this]( const string& name ) { return authority( _db.get< chain::account_authority_object, chain::by_account >( name ).social ); },
         XGT_MAX_SIG_CHECK_DEPTH );
   }
   catch( fc::exception& ) { result.valid = false; }

   return result;
}

//////////////////////////////////////////////////////////////////////
//                                                                  //
// XTT                                                              //
//                                                                  //
//////////////////////////////////////////////////////////////////////

DEFINE_API_IMPL( database_api_impl, get_nai_pool )
{
   get_nai_pool_return result;
   result.nai_pool = _db.get< nai_pool_object >().pool();
   return result;
}

DEFINE_API_IMPL( database_api_impl, get_xtt_balances )
{
   auto acc_syms = args[0].as< vector< std::pair< wallet_name_type, string > > >();
   get_xtt_balances_return result;
   result.balances.reserve( acc_syms.size() );

   for( auto& acc_sym : acc_syms )
   {
      auto first = acc_sym.first;
      auto second = asset_symbol_type::from_nai_string( acc_sym.second.c_str(), 0 );
      auto itr = _db.find< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( first, second ) );
      if( itr != nullptr ) result.balances.push_back( api_xtt_account_balance_object( *itr, _db ) );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, list_xtt_contributions )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_xtt_contributions_return result;
   result.contributions.reserve( args.limit );

   switch( args.order )
   {
      case( by_symbol_contributor ):
      {
         auto key = args.start.get_array();
         FC_ASSERT( key.size() == 0 || key.size() == 3, "The parameter 'start' must be an empty array or consist of asset_symbol_type, contributor and contribution_id" );

         boost::tuple< asset_symbol_type, wallet_name_type, uint32_t > start;
         if ( key.size() == 0 )
            start = boost::make_tuple( asset_symbol_type(), wallet_name_type(), 0 );
         else
            start = boost::make_tuple( key[ 0 ].as< asset_symbol_type >(), key[ 1 ].as< wallet_name_type >(), key[ 2 ].as< uint32_t >() );

         iterate_results< chain::xtt_contribution_index, chain::by_symbol_contributor >(
            start,
            result.contributions,
            args.limit,
            &database_api_impl::on_push_default< chain::xtt_contribution_object >,
            &database_api_impl::filter_default< chain::xtt_contribution_object > );
         break;
      }
      case( by_symbol_id ):
      {
         auto key = args.start.get_array();
         FC_ASSERT( key.size() == 0 || key.size() == 2, "The parameter 'start' must be an empty array or consist of asset_symbol_type and id" );

         boost::tuple< asset_symbol_type, xtt_contribution_object_id_type > start;
         if ( key.size() == 0 )
            start = boost::make_tuple( asset_symbol_type(), 0 );
         else
            start = boost::make_tuple( key[ 0 ].as< asset_symbol_type >(), key[ 1 ].as< xtt_contribution_object_id_type >() );

         iterate_results< chain::xtt_contribution_index, chain::by_symbol_id >(
            start,
            result.contributions,
            args.limit,
            &database_api_impl::on_push_default< chain::xtt_contribution_object >,
            &database_api_impl::filter_default< chain::xtt_contribution_object > );
         break;
      }
#ifndef IS_LOW_MEM
      case ( by_contributor ):
      {
         auto key = args.start.get_array();
         FC_ASSERT( key.size() == 0 || key.size() == 3, "The parameter 'start' must be an empty array or consist of contributor, asset_symbol_type and contribution_id" );

         boost::tuple< wallet_name_type, asset_symbol_type, uint32_t > start;
         if ( key.size() == 0 )
            start = boost::make_tuple( wallet_name_type(), asset_symbol_type(), 0 );
         else
            start = boost::make_tuple( key[ 0 ].as< wallet_name_type >(), key[ 1 ].as< asset_symbol_type >(), key[ 2 ].as< uint32_t >() );

         iterate_results< chain::xtt_contribution_index, chain::by_contributor >(
            start,
            result.contributions,
            args.limit,
            &database_api_impl::on_push_default< chain::xtt_contribution_object >,
            &database_api_impl::filter_default< chain::xtt_contribution_object > );
         break;
      }
#endif
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_xtt_contributions )
{
   find_xtt_contributions_return result;

   const auto& idx = _db.get_index< chain::xtt_contribution_index, chain::by_symbol_contributor >();

   for( auto& symbol_contributor : args.symbol_contributors )
   {
      auto itr = idx.lower_bound( boost::make_tuple( symbol_contributor.first, symbol_contributor.second, 0 ) );
      while( itr != idx.end() && itr->symbol == symbol_contributor.first && itr->contributor == symbol_contributor.second && result.contributions.size() <= DATABASE_API_SINGLE_QUERY_LIMIT )
      {
         result.contributions.push_back( *itr );
         ++itr;
      }
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, list_xtt_tokens )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_xtt_tokens_return result;
   result.tokens.reserve( args.limit );

   switch( args.order )
   {
      case( by_symbol ):
      {
         asset_symbol_type start;

         if( args.start.get_object().size() > 0 )
         {
            start = asset_symbol_type::from_asset_num( args.start.as< asset_symbol_type >().get_stripped_precision_xtt_num() );
         }

         iterate_results< chain::xtt_token_index, chain::by_symbol >(
            start,
            result.tokens,
            args.limit,
            [&]( const xtt_token_object& t ) { return api_xtt_token_object( t, _db ); },
            &database_api_impl::filter_default< chain::xtt_token_object > );

         break;
      }
      case( by_control_account ):
      {
         boost::tuple< wallet_name_type, asset_symbol_type > start;

         if( args.start.is_string() )
         {
            start = boost::make_tuple( args.start.as< wallet_name_type >(), asset_symbol_type() );
         }
         else
         {
            auto key = args.start.get_array();
            FC_ASSERT( key.size() == 2, "The parameter 'start' must be an account name or an array containing an account name and an asset symbol" );

            start = boost::make_tuple( key[0].as< wallet_name_type >(), key[1].as< asset_symbol_type >() );
         }

         iterate_results< chain::xtt_token_index, chain::by_control_account >(
            start,
            result.tokens,
            args.limit,
            [&]( const xtt_token_object& t ) { return api_xtt_token_object( t, _db ); },
            &database_api_impl::filter_default< chain::xtt_token_object > );

         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_xtt_tokens )
{
   FC_ASSERT( args.symbols.size() <= DATABASE_API_SINGLE_QUERY_LIMIT );

   find_xtt_tokens_return result;
   result.tokens.reserve( args.symbols.size() );

   for( auto& symbol : args.symbols )
   {
      const auto token = chain::util::xtt::find_token( _db, symbol, symbol.decimals() == 0 );
      if( token != nullptr )
      {
         result.tokens.push_back( api_xtt_token_object( *token, _db ) );
      }
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_xtt_token_balances )
{
   FC_ASSERT( args.account_symbols.size() <= DATABASE_API_SINGLE_QUERY_LIMIT );
   find_xtt_token_balances_return result;
   result.balances.reserve( args.account_symbols.size() );

   for( auto& acc_sym : args.account_symbols )
   {
      auto itr = _db.find< account_regular_balance_object, by_name_liquid_symbol >( boost::make_tuple( acc_sym.first, acc_sym.second ) );
      if( itr != nullptr ) result.balances.push_back( api_xtt_account_balance_object( *itr, _db ) );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, list_xtt_token_balances )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );
   list_xtt_token_balances_return result;
   result.balances.reserve( args.limit );

   switch( args.order )
   {
      case( by_account_symbol ):
      {
         auto key = args.start.get_array();
         FC_ASSERT( key.size() == 2, "The parameter 'start' must consist of wallet_name_type and asset_symbol_type" );

         boost::tuple< wallet_name_type, asset_symbol_type > start;
         start = boost::make_tuple(
            key[ 0 ].as< wallet_name_type >(),
            asset_symbol_type::from_asset_num( key[ 1 ].as< asset_symbol_type >().get_stripped_precision_xtt_num() ) );

         iterate_results< chain::account_regular_balance_index, chain::by_name_liquid_symbol >(
            start,
            result.balances,
            args.limit,
            [&]( const chain::account_regular_balance_object& b ){ return api_xtt_account_balance_object( b, _db ); },
            &database_api_impl::filter_default< chain::account_regular_balance_object > );
         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_LOCKLESS_APIS( database_api, (get_config)(get_version) )

DEFINE_READ_APIS( database_api,
   (get_dynamic_global_properties)
   (get_hardfork_properties)
   (list_witnesses)
   (find_witnesses)
   (list_wallets)
   (find_wallets)
   (list_escrows)
   (find_escrows)
   (list_comments)
   (find_comments)
   (list_votes)
   (find_votes)
   (get_transaction_hex)
   (get_required_signatures)
   (get_potential_signatures)
   (verify_authority)
   (verify_account_authority)
   (verify_signatures)
   (get_nai_pool)
   (get_xtt_balances)
   (list_xtt_contributions)
   (find_xtt_contributions)
   (list_xtt_tokens)
   (find_xtt_tokens)
   (list_xtt_token_balances)
   (find_xtt_token_balances)
)

} } } // xgt::plugins::database_api
