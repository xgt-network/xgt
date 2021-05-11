#pragma once

#include <xgt/protocol/types.hpp>

#include <xgt/protocol/operation_util.hpp>
#include <xgt/protocol/xgt_operations.hpp>
#include <xgt/protocol/xgt_virtual_operations.hpp>
#include <xgt/protocol/xtt_operations.hpp>

namespace xgt { namespace protocol {

   /** NOTE: do not change the order of any operations prior to the virtual operations
    * or it will trigger a hardfork.
    */
   typedef fc::static_variant<
            comment_operation,

            transfer_operation,

            wallet_create_operation,
            wallet_update_operation,

            witness_update_operation,

            custom_operation,

            report_over_production_operation,

            delete_comment_operation,
            custom_json_operation,
            comment_options_operation,
            request_wallet_recovery_operation,
            recover_wallet_operation,
            change_recovery_wallet_operation,
            escrow_transfer_operation,
            escrow_dispute_operation,
            escrow_release_operation,
            pow_operation,
            escrow_approve_operation,

            vote_operation,

            /// XTT operations
            xtt_setup_operation,
            xtt_setup_ico_tier_operation,
            xtt_set_setup_parameters_operation,
            xtt_set_runtime_parameters_operation,
            xtt_create_operation,
            xtt_contribute_operation,

            /// virtual operations below this point
            shutdown_witness_operation,
            hardfork_operation,
            clear_null_wallet_balance_operation,

            contract_create_operation,
            contract_invoke_operation
         > operation;

   /*void operation_get_required_authorities( const operation& op,
                                            flat_set<string>& money,
                                            flat_set<string>& recovery,
                                            flat_set<string>& social,
                                            vector<authority>&  other );

   void operation_validate( const operation& op );*/

   bool is_virtual_operation( const operation& op );
   bool is_wallet_update_operation( const operation& op );
   bool is_pow_operation( const operation& op );

} } // xgt::protocol

/*namespace fc {
    void to_variant( const xgt::protocol::operation& var,  fc::variant& vo );
    void from_variant( const fc::variant& var,  xgt::protocol::operation& vo );
}*/

XGT_DECLARE_OPERATION_TYPE( xgt::protocol::operation )
FC_REFLECT_TYPENAME( xgt::protocol::operation )
