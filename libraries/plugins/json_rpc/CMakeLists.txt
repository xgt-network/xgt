file(GLOB HEADERS "include/xgt/plugins/json_rpc/*.hpp")

add_library( json_rpc_plugin json_rpc_plugin.cpp ${HEADERS} )

target_link_libraries( json_rpc_plugin PRIVATE chain_plugin chainbase appbase fc xgt_chain )
target_include_directories( json_rpc_plugin PUBLIC include )
