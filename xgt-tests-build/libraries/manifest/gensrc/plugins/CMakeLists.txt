
add_library( xgt_plugins
   mf_plugins.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/include/xgt/manifest/plugins.hpp
)

target_link_libraries( xgt_plugins appbase fc

   wallet_by_key_plugin

   wallet_history_rocksdb_plugin

   transaction_status_plugin

   chain_plugin

   webserver_plugin

   chain_api_plugin

   transaction_api_plugin

   wallet_by_key_api_plugin

   contract_api_plugin

   wallet_history_api_plugin

   database_api_plugin

   block_api_plugin

   debug_node_plugin

   p2p_plugin

   wallet_history_plugin

   witness_plugin

)

target_include_directories( xgt_plugins
   PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
   )

INSTALL( TARGETS
   xgt_plugins

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)