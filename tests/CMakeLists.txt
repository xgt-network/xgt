add_subdirectory(db_fixture)

find_package(Boost QUIET REQUIRED COMPONENTS unit_test_framework)

file(GLOB UNIT_TESTS "tests/*.cpp")

add_executable( chain_test ${UNIT_TESTS})
add_test(NAME chain_test COMMAND chain_test)

target_link_libraries( chain_test db_fixture chainbase xgt_chain xgt_protocol chain_plugin wallet_history_plugin witness_plugin debug_node_plugin fc Boost::unit_test_framework)

file(GLOB PLUGIN_TESTS "plugin_tests/*.cpp")

add_executable( plugin_test ${PLUGIN_TESTS})
add_test(NAME plugin_test COMMAND plugin_test)

target_link_libraries( plugin_test db_fixture xgt_chain xgt_protocol wallet_history_plugin witness_plugin debug_node_plugin transaction_status_plugin xgt_utilities xgt_plugins Boost::unit_test_framework)
