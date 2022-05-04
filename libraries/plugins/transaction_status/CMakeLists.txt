file(GLOB HEADERS "include/xgt/plugins/transaction_status/*.hpp")

add_library( transaction_status_plugin transaction_status_plugin.cpp ${HEADERS})

target_link_libraries( transaction_status_plugin PRIVATE chain_plugin xgt_chain )
target_include_directories( transaction_status_plugin PUBLIC include )

