file(GLOB HEADERS "include/xgt/plugins/wallet_history/*.hpp")

add_library( wallet_history_plugin
             wallet_history_plugin.cpp
             ${HEADERS}
           )

target_link_libraries( wallet_history_plugin chain_plugin xgt_chain xgt_protocol xgt_utilities )
target_include_directories( wallet_history_plugin
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

if( CLANG_TIDY_EXE )
   set_target_properties(
      wallet_history_plugin PROPERTIES
      CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
   )
endif( CLANG_TIDY_EXE )

install( TARGETS
   wallet_history_plugin

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
