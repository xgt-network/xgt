file(GLOB HEADERS "include/xgt/plugins/chain/*.hpp")

add_library( chain_plugin
             chain_plugin.cpp
             statefile/load_state.cpp
             statefile/save_state.cpp
             statefile/statefile.cpp
             ${HEADERS} )

target_link_libraries( chain_plugin xgt_chain appbase xgt_utilities xgt_protocol xgt_chain )
target_include_directories( chain_plugin PUBLIC include )