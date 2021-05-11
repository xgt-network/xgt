file(GLOB HEADERS "include/xgt/plugins/transaction_api_plugin/*.hpp")
add_library( transaction_api_plugin
             transaction_api.cpp
             transaction_api_plugin.cpp
             ${HEADERS} )

target_link_libraries( transaction_api_plugin
   chain_plugin
   json_rpc_plugin
   p2p_plugin
   database_api_plugin
   block_api_plugin
   transaction_status_plugin
   xgt_utilities
   appbase )
target_include_directories( transaction_api_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

if( CLANG_TIDY_EXE )
   set_target_properties(
      transaction_api_plugin PROPERTIES
      CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
   )
endif( CLANG_TIDY_EXE )

install( TARGETS
   transaction_api_plugin

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
install( FILES ${HEADERS} DESTINATION "include/xgt/transaction_api_plugin" )
