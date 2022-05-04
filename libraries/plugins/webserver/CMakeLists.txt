file(GLOB HEADERS "include/xgt/plugins/webserver/*.hpp")

add_library( webserver_plugin
             webserver_plugin.cpp
             ${HEADERS} )

target_link_libraries( webserver_plugin PRIVATE json_rpc_plugin chain_plugin appbase fc xgt_chain )
target_include_directories( webserver_plugin PUBLIC include )
