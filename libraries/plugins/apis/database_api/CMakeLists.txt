file(GLOB HEADERS "include/xgt/plugins/database_api/*.hpp")

add_library( database_api_plugin
             database_api.cpp
             database_api_plugin.cpp
             ${HEADERS}
           )

target_link_libraries( database_api_plugin chain_plugin json_rpc_plugin xgt_chain xgt_utilities )
target_include_directories( database_api_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
