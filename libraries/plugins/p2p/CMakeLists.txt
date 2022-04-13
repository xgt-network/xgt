file(GLOB HEADERS "include/xgt/plugins/p2p/*.hpp")

add_library( p2p_plugin p2p_plugin.cpp ${HEADERS})

target_link_libraries( p2p_plugin PRIVATE chain_plugin appbase xgt_chain graphene_net )
target_include_directories( p2p_plugin PUBLIC include )
