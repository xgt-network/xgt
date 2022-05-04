file(GLOB HEADERS "include/xgt/plugins/debug_node/*.hpp")

add_library( debug_node_plugin ${HEADERS} debug_node_plugin.cpp)

target_link_libraries( debug_node_plugin PRIVATE chain_plugin xgt_utilities witness_plugin xgt_chain )
target_include_directories( debug_node_plugin PUBLIC include )
