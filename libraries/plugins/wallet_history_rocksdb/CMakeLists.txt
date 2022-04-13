file(GLOB HEADERS "include/xgt/plugins/wallet_history_rocksdb/*.hpp")

add_library( wallet_history_rocksdb_plugin wallet_history_rocksdb_plugin.cpp ${HEADERS})

target_link_libraries( wallet_history_rocksdb_plugin PRIVATE rocksdb chain_plugin xgt_chain xgt_protocol json_rpc_plugin rocksdb xgt_utilities)

target_include_directories( wallet_history_rocksdb_plugin
   PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
          "${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/rocksdb/include"
)
