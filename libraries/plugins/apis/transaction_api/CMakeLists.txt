file(GLOB HEADERS "include/xgt/plugins/transaction_api_plugin/*.hpp")
add_library( transaction_api_plugin
             transaction_api.cpp
             transaction_api_plugin.cpp
             ${HEADERS} )

target_link_libraries( transaction_api_plugin
   chain_plugin
   json_rpc_plugin
   p2p_plugin
   database_api_plugin
   block_api_plugin
   transaction_status_plugin
   xgt_utilities
   xgt_chain
   appbase )
target_include_directories( transaction_api_plugin PUBLIC include )
