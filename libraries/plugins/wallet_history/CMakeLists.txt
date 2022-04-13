file(GLOB HEADERS "include/xgt/plugins/wallet_history/*.hpp")

add_library( wallet_history_plugin wallet_history_plugin.cpp ${HEADERS})

target_link_libraries( wallet_history_plugin PRIVATE chain_plugin xgt_chain xgt_protocol xgt_utilities )
target_include_directories( wallet_history_plugin PUBLIC include )

