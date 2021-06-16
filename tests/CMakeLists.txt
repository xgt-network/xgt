add_subdirectory(db_fixture)
add_subdirectory(bmic_objects)

find_package( Gperftools QUIET )
if( GPERFTOOLS_FOUND )
    message( STATUS "Found gperftools; compiling tests with TCMalloc")
    list( APPEND PLATFORM_SPECIFIC_LIBS tcmalloc )
endif()

INCLUDE(CTest)

ENABLE_TESTING()

file(GLOB UNIT_TESTS "tests/*.cpp")

include(BoostTestTargets.cmake)

function(parse_unit_tests RESULT)
   set(SOURCES)
	foreach(_element ${ARGN})
      list(APPEND SOURCES "${_element}")
	endforeach()

   set(tests)

   foreach(src ${SOURCES})
      file(READ ${src} thefile)
      #string(REGEX MATCH "" test_suite "${thefile}" )
      string(REGEX MATCH "BOOST_FIXTURE_TEST_SUITE\\([A-Za-z0-9_, ]*\\)" test_suite "${thefile}" )

      if( NOT (test_suite STREQUAL "") )
         string(SUBSTRING "${test_suite}" 25 -1 test_suite)
         string(FIND "${test_suite}" "," comma_loc )
         string(SUBSTRING "${test_suite}" 0 ${comma_loc} test_suite)
         string(STRIP "${test_suite}" test_suite)

         string( REGEX MATCHALL "BOOST_AUTO_TEST_CASE\\([A-Za-z0-9_, ]*\\)" cases "${thefile}" )

         foreach( test_case ${cases} )
            string(SUBSTRING "${test_case}" 22 -1 test_case)
            string(FIND "${test_case}" ")" paren_loc )
            string(SUBSTRING "${test_case}" 0 ${paren_loc} test_case)
            string(STRIP "${test_case}" test_case)

            list(APPEND tests "${test_suite}/${test_case}")
         endforeach()
      endif()
   endforeach()

   set(${RESULT} ${tests} PARENT_SCOPE)
endfunction()

parse_unit_tests(TEST_CASES ${UNIT_TESTS})
#string (REPLACE ";" "\n" TEST_CASES "${TEST_CASES}")
#message(STATUS "${TEST_CASES}")

add_boost_test( chain_test
   SOURCES ${UNIT_TESTS}
   TESTS
   #    automated_action_tests/push_pending_required_actions
   #    automated_action_tests/push_pending_optional_actions
   #    automated_action_tests/full_block
   #    automated_action_tests/pending_required_execution
   #    automated_action_tests/unexpected_required_action
   #    automated_action_tests/missing_required_action
   #    automated_action_tests/optional_action_expiration
   #    automated_action_tests/unexpected_optional_action
   #    automated_action_tests/reject_optional_action
   #    basic_tests/parse_size_test
   #    basic_tests/valid_name_test

   basic_tests/merkle_root

   #    operation_tests/account_create_validate
   #    operation_tests/account_create_authorities
   #    operation_tests/account_create_apply
   #    operation_tests/account_update_validate
   #    operation_tests/account_update_authorities
   #    operation_tests/account_update_apply
   #    operation_tests/comment_validate
   #    operation_tests/comment_authorities
   #    operation_tests/comment_apply
   #    operation_tests/comment_delete_apply
   #    operation_tests/vote_validate
   #    operation_tests/vote_authorities
   #    operation_tests/vote_apply
   #    operation_tests/transfer_validate
   #    operation_tests/transfer_authorities
   #    operation_tests/signature_stripping
   #    operation_tests/transfer_apply
   #    operation_tests/transfer_to_vesting_validate
   #    operation_tests/transfer_to_vesting_authorities
   #    operation_tests/transfer_to_vesting_apply
   #    operation_tests/withdraw_vesting_validate
   #    operation_tests/withdraw_vesting_authorities
   #    operation_tests/withdraw_vesting_apply
   #    operation_tests/witness_update_validate
   #    operation_tests/witness_update_authorities
   #    operation_tests/witness_update_apply
   #    operation_tests/account_witness_vote_validate
   #    operation_tests/account_witness_vote_authorities
   #    operation_tests/account_witness_vote_apply
   #    operation_tests/account_witness_proxy_validate
   #    operation_tests/account_witness_proxy_authorities
   #    operation_tests/account_witness_proxy_apply
   #    operation_tests/custom_authorities
   #    operation_tests/custom_json_authorities
   #    operation_tests/custom_binary_authorities
   #    operation_tests/feed_publish_validate
   #    operation_tests/feed_publish_authorities
   #    operation_tests/feed_publish_apply
   #    operation_tests/convert_validate
   #    operation_tests/convert_authorities
   #    operation_tests/convert_apply
   #    operation_tests/fixture_convert_checks_balance
   #    operation_tests/limit_order_create_validate
   #    operation_tests/limit_order_create_authorities
   #    operation_tests/limit_order_create_apply
   #    operation_tests/limit_order_create2_authorities
   #    operation_tests/limit_order_create2_apply
   #    operation_tests/limit_order_cancel_validate
   #    operation_tests/limit_order_cancel_authorities
   #    operation_tests/limit_order_cancel_apply
   #    operation_tests/pow_validate
   #    operation_tests/pow_authorities
   #    operation_tests/pow_apply
   #    operation_tests/account_recovery
   #    operation_tests/change_recovery_account
   #    operation_tests/escrow_transfer_validate
   #    operation_tests/escrow_transfer_authorities
   #    operation_tests/escrow_transfer_apply
   #    operation_tests/escrow_approve_validate
   #    operation_tests/escrow_approve_authorities
   #    operation_tests/escrow_approve_apply
   #    operation_tests/escrow_dispute_validate
   #    operation_tests/escrow_dispute_authorities
   #    operation_tests/escrow_dispute_apply
   #    operation_tests/escrow_release_validate
   #    operation_tests/escrow_release_authorities
   #    operation_tests/escrow_release_apply
   #    operation_tests/transfer_to_savings_validate
   #    operation_tests/transfer_to_savings_authorities
   #    operation_tests/transfer_to_savings_apply
   #    operation_tests/transfer_from_savings_validate
   #    operation_tests/transfer_from_savings_authorities
   #    operation_tests/transfer_from_savings_apply
   #    operation_tests/cancel_transfer_from_savings_validate
   #    operation_tests/cancel_transfer_from_savings_authorities
   #    operation_tests/cancel_transfer_from_savings_apply
   #    operation_tests/decline_voting_rights_authorities
   #    operation_tests/decline_voting_rights_apply
   #    operation_tests/claim_reward_balance_validate
   #    operation_tests/claim_reward_balance_authorities
   #    operation_tests/account_create_with_delegation_authorities
   #    operation_tests/account_create_with_delegation_apply
   #    operation_tests/claim_reward_balance_apply
   #    operation_tests/delegate_vesting_shares_validate
   #    operation_tests/delegate_vesting_shares_authorities
   #    operation_tests/delegate_vesting_shares_apply
   #    operation_tests/issue_971_vesting_removal
   #    operation_tests/comment_beneficiaries_validate
   #    operation_tests/comment_beneficiaries_apply
   #    operation_tests/witness_set_properties_validate
   #    operation_tests/witness_set_properties_authorities
   #    operation_tests/witness_set_properties_apply
   #    operation_tests/claim_account_validate
   #    operation_tests/claim_account_authorities
   #    operation_tests/claim_account_apply
   #    operation_tests/create_claimed_account_validate
   #    operation_tests/create_claimed_account_authorities
   #    operation_tests/create_claimed_account_apply
   #    operation_tests/account_auth_tests
   #    operation_tests/account_update2_validate
   #    operation_tests/account_update2_authorities
   #    operation_tests/account_update2_apply
   #    operation_time_tests/comment_payout_equalize
   #    operation_time_tests/comment_payout_dust
   #    operation_time_tests/recent_claims_decay
   #    operation_time_tests/vesting_withdrawals
   #    operation_time_tests/vesting_withdraw_route
   #    operation_time_tests/feed_publish_mean
   #    operation_time_tests/convert_delay
   #    operation_time_tests/xgt_inflation
   #    operation_time_tests/sbd_interest
   #    operation_time_tests/post_rate_limit
   #    operation_time_tests/comment_freeze
   #    operation_time_tests/sbd_stability
   #    operation_time_tests/sbd_price_feed_limit
   #    operation_time_tests/clear_null_account
   #    operation_time_tests/generate_account_subsidies
   #    operation_time_tests/account_subsidy_witness_limits
   #    serialization_tests/serialization_raw_test
   #    serialization_tests/serialization_json_test
   #    serialization_tests/legacy_asset_test
   #    serialization_tests/asset_test
   #    serialization_tests/asset_raw_test
   #    serialization_tests/json_tests
   #    serialization_tests/extended_private_key_type_test
   #    serialization_tests/extended_public_key_type_test
   #    serialization_tests/version_test
   #    serialization_tests/hardfork_version_test
   #    serialization_tests/min_block_size
   #    serialization_tests/legacy_signed_transaction
   #    serialization_tests/static_variant_json_test
   #    serialization_tests/legacy_operation_test
   #    serialization_tests/asset_symbol_type_test
   #    serialization_tests/unpack_clear_test
   #    serialization_tests/unpack_recursion_test
   #    xtt_operation_tests/xtt_limit_order_create_authorities
   #    xtt_operation_tests/xtt_limit_order_create2_authorities
   #    xtt_operation_tests/xtt_limit_order_create_apply
   #    xtt_operation_tests/xtt_limit_order_cancel_authorities
   #    xtt_operation_tests/xtt_limit_order_cancel_apply
   #    xtt_operation_tests/xtt_limit_order_create2_apply
   #    xtt_operation_tests/claim_reward_balance2_validate
   #    xtt_operation_tests/claim_reward_balance2_authorities
   #    xtt_operation_tests/claim_reward_balance2_apply
   #    xtt_operation_tests/xtt_transfer_to_vesting_validate
   #    xtt_operation_tests/xtt_transfer_to_vesting_apply
   #    xtt_operation_tests/xtt_withdraw_vesting_validate
   #    xtt_operation_tests/xtt_withdraw_vesting_apply
   #    xtt_operation_tests/xtt_create_validate
   #    xtt_operation_tests/xtt_create_authorities
   #    xtt_operation_tests/xtt_create_duplicate
   #    xtt_operation_tests/xtt_create_duplicate_differing_decimals
   #    xtt_operation_tests/xtt_create_duplicate_different_users
   #    xtt_operation_tests/xtt_create_with_xgt_funds
   #    xtt_operation_tests/xtt_create_with_sbd_funds
   #    xtt_operation_tests/xtt_create_with_invalid_nai
   #    xtt_operation_tests/xtt_creation_fee_test
   #    xtt_operation_tests/xtt_create_reset
   #    xtt_operation_tests/xtt_nai_pool_removal
   #    xtt_operation_tests/xtt_nai_pool_count
   #    xtt_operation_tests/xtt_setup_emissions_validate
   #    xtt_operation_tests/xtt_setup_emissions_authorities
   #    xtt_operation_tests/xtt_setup_emissions_apply
   #    xtt_operation_tests/set_setup_parameters_validate
   #    xtt_operation_tests/set_setup_parameters_authorities
   #    xtt_operation_tests/set_setup_parameters_apply
   #    xtt_operation_tests/xtt_set_runtime_parameters_validate
   #    xtt_operation_tests/xtt_set_runtime_parameters_authorities
   #    xtt_operation_tests/xtt_set_runtime_parameters_apply
   #    xtt_operation_tests/xtt_contribute_validate
   #    xtt_operation_tests/xtt_contribute_apply
   #    xtt_operation_tests/xtt_transfer_validate
   #    xtt_operation_tests/xtt_transfer_apply
   #    xtt_operation_tests/xtt_setup_validate
   #    xtt_operation_tests/xtt_setup_authorities
   #    xtt_operation_tests/xtt_setup_apply
   #    xtt_operation_tests/xtt_setup_ico_tier_validate
   #    xtt_operation_tests/xtt_setup_ico_tier_authorities
   #    xtt_operation_tests/xtt_setup_ico_tier_apply
   #    xtt_operation_tests/comment_votable_assets_validate
   #    xtt_operation_tests/comment_votable_assets_apply
   #    xtt_operation_tests/xtt_delegate_vesting_shares_validate
   #    xtt_operation_tests/xtt_delegate_vesting_shares_apply
   #    xtt_operation_tests/vote2_authorities
   #    xtt_operation_tests/vote2_apply
   #    xtt_operation_time_tests/xtt_refunds
   #    xtt_operation_time_tests/xtt_ico_payouts
   #    xtt_operation_time_tests/xtt_ico_payouts_special_destinations
   #    xtt_operation_time_tests/xtt_without_ico
   #    xtt_operation_time_tests/xtt_vesting_withdrawals
   #    xtt_operation_time_tests/recent_claims_decay
   #    xtt_operation_time_tests/xtt_rewards
   #    xtt_operation_time_tests/xtt_token_emissions
   #    xtt_operation_time_tests/xtt_action_delay
   #    xtt_tests/xtt_transfer_validate
   #    xtt_tests/xtt_transfer_apply
   #    xtt_tests/asset_symbol_vesting_methods
   #    xtt_tests/vesting_xtt_creation
   #    xtt_tests/xtt_founder_vesting
   #    xtt_tests/setup_validate
   #    xtt_tests/setup_authorities
   #    xtt_tests/setup_apply
   #    xtt_tests/xtt_create_apply
   #    xtt_tests/tick_pricing_rules_validation
   #    xtt_tests/tick_pricing_rules
   #    xtt_tests/price_as_decimal_and_real
   #    xtt_tests/token_emission_timing
   #    undo_tests/undo_basic
   #    undo_tests/undo_object_disapear
   #    undo_tests/undo_key_collision
   #    undo_tests/undo_different_indexes
   #    undo_tests/undo_generate_blocks
)

target_link_libraries( chain_test db_fixture chainbase xgt_chain xgt_protocol wallet_history_plugin witness_plugin debug_node_plugin fc ${PLATFORM_SPECIFIC_LIBS} )

file(GLOB PLUGIN_TESTS "plugin_tests/*.cpp")

parse_unit_tests(TEST_CASES ${PLUGIN_TESTS})

#message(STATUS "${TEST_CASES}")

add_boost_test( plugin_test
   SOURCES ${PLUGIN_TESTS}
   TESTS
   json_rpc/basic_validation
   json_rpc/syntax_validation
   json_rpc/misc_validation
   json_rpc/positive_validation
   json_rpc/semantics_validation
   transaction_status/transaction_status_test
)

target_link_libraries( plugin_test db_fixture xgt_chain xgt_protocol wallet_history_plugin witness_plugin debug_node_plugin transaction_status_plugin transaction_status_api_plugin fc ${PLATFORM_SPECIFIC_LIBS} )

if(MSVC)
  set_source_files_properties( tests/serialization_tests.cpp PROPERTIES COMPILE_FLAGS "/bigobj" )
endif(MSVC)

#add_subdirectory( generate_empty_blocks )
