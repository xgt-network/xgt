file(GLOB HEADERS "include/xgt/plugins/wallet_by_key/*.hpp")

add_library( wallet_by_key_plugin wallet_by_key_plugin.cpp ${HEADERS})

target_link_libraries( wallet_by_key_plugin PRIVATE chain_plugin xgt_chain xgt_protocol )
target_include_directories( wallet_by_key_plugin PUBLIC include )

