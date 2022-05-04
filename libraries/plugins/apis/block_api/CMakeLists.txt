file(GLOB HEADERS "include/xgt/plugins/block_api/*.hpp")

add_library( block_api_plugin
             block_api.cpp
             block_api_plugin.cpp
             ${HEADERS}
           )

target_link_libraries( block_api_plugin xgt_chain chain_plugin json_rpc_plugin fc )
target_include_directories( block_api_plugin PUBLIC include )
