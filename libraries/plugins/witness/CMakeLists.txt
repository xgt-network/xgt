file(GLOB HEADERS "include/xgt/plugins/witness/*.hpp")

add_library( witness_plugin
             witness_plugin.cpp
             block_producer.cpp
             ${HEADERS}
           )

target_link_libraries( witness_plugin PRIVATE p2p_plugin chain_plugin appbase xgt_chain xgt_utilities)
target_include_directories( witness_plugin PUBLIC include )
