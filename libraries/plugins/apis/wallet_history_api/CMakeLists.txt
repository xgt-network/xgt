file(GLOB HEADERS "include/xgt/plugins/wallet_history_api/*.hpp")

add_library( wallet_history_api_plugin
             wallet_history_api_plugin.cpp
             wallet_history_api.cpp
             ${HEADERS}
           )

target_link_libraries( wallet_history_api_plugin wallet_history_plugin wallet_history_rocksdb_plugin json_rpc_plugin xgt_chain chain_plugin )
target_include_directories( wallet_history_api_plugin PUBLIC include )
