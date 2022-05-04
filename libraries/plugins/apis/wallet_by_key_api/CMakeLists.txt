file(GLOB HEADERS "include/xgt/plugins/wallet_by_key_api/*.hpp")

add_library( wallet_by_key_api_plugin
             wallet_by_key_api_plugin.cpp
             wallet_by_key_api.cpp
             ${HEADERS}
           )

target_link_libraries( wallet_by_key_api_plugin chain_plugin wallet_by_key_plugin json_rpc_plugin xgt_chain )
target_include_directories( wallet_by_key_api_plugin PUBLIC include )
