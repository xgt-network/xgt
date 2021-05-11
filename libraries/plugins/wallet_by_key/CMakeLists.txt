file(GLOB HEADERS "include/xgt/plugins/wallet_by_key/*.hpp")

add_library( wallet_by_key_plugin
             wallet_by_key_plugin.cpp
             ${HEADERS}
           )

target_link_libraries( wallet_by_key_plugin chain_plugin xgt_chain xgt_protocol )
target_include_directories( wallet_by_key_plugin
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

if( CLANG_TIDY_EXE )
   set_target_properties(
      wallet_by_key_plugin PROPERTIES
      CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
   )
endif( CLANG_TIDY_EXE )

install( TARGETS
   wallet_by_key_plugin

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
