#pragma once

#include <random>

#include <xgt/plugins/json_rpc/utility.hpp>

#include <xgt/plugins/database_api/database_api_args.hpp>
#include <xgt/plugins/database_api/database_api_objects.hpp>

#define DATABASE_API_SINGLE_QUERY_LIMIT 1000

namespace xgt { namespace plugins { namespace database_api {

class database_api_impl;

class database_api
{
   public:
      database_api();
      ~database_api();

      DECLARE_API(

         /////////////
         // Globals //
         /////////////

         /**
         * @brief Retrieve compile-time constants
         */
         (get_config)

         /**
          * @brief Return version information and chain_id of running node
          */
         (get_version)

         /**
         * @brief Retrieve the current @ref dynamic_global_property_object
         */
         (get_dynamic_global_properties)
         (get_hardfork_properties)

         ///////////////
         // Witnesses //
         ///////////////
         (list_witnesses)
         (find_witnesses)

         //////////////
         // Accounts //
         //////////////

         /**
         * @brief List accounts ordered by specified key
         *
         */
         (list_wallets)

         /**
         * @brief Find accounts by primary key (account name)
         */
         (find_wallets)
         (list_escrows)
         (find_escrows)

         //////////////
         // Comments //
         //////////////
         (list_comments)
         (find_comments)
         (list_votes)
         (find_votes)

         ////////////////////////////
         // Authority / validation //
         ////////////////////////////

         /// @brief Get a hexdump of the serialized binary form of a transaction
         (get_transaction_hex)

         /**
         *  This API will take a partially signed transaction and a set of public keys that the recovery has the ability to sign for
         *  and return the minimal subset of public keys that should add signatures to the transaction.
         */
         (get_required_signatures)

         /**
         *  This method will return the set of all public keys that could possibly sign for a given transaction.  This call can
         *  be used by wallets to filter their set of public keys to just the relevant subset prior to calling @ref get_required_signatures
         *  to get the minimum subset.
         */
         (get_potential_signatures)

         /**
         * @return true of the @ref trx has all of the required signatures, otherwise throws an exception
         */
         (verify_authority)

         /**
         * @return true if the signers have enough authority to authorize an account
         */
         (verify_account_authority)

         /*
          * This is a general purpose API that checks signatures against accounts for an arbitrary sha256 hash
          * using the existing authority structures in Xgt
          */
         (verify_signatures)

         /**
         * @return array of Numeric Asset Identifier (NAI) available to be used for new XTT to be created.
         */
         (get_nai_pool)
         (get_xtt_balances)

         (list_xtt_contributions)
         (find_xtt_contributions)

         (list_xtt_tokens)
         (find_xtt_tokens)

         (list_xtt_token_balances)
         (find_xtt_token_balances)
      )

   private:
      std::unique_ptr< database_api_impl > my;
};

} } } //xgt::plugins::database_api

