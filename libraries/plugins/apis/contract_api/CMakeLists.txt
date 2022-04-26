file( GLOB HEADERS "include/xgt/plugins/contract_api/*.hpp" )
set( SOURCES contract_api.cpp contract_api_plugin.cpp )

add_library( contract_api_plugin ${SOURCES} ${HEADERS} )

target_link_libraries( contract_api_plugin chain_plugin json_rpc_plugin xgt_chain )
target_include_directories( contract_api_plugin PUBLIC include )
