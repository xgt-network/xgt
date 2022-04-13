file( GLOB HEADERS "include/xgt/plugins/chain_api/*.hpp" )
set( SOURCES chain_api.cpp chain_api_plugin.cpp )

add_library( chain_api_plugin ${SOURCES} ${HEADERS} )

target_link_libraries( chain_api_plugin chain_plugin json_rpc_plugin xgt_chain )
target_include_directories( chain_api_plugin PUBLIC include )