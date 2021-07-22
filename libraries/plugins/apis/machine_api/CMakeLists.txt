file(GLOB HEADERS "include/xgt/plugins/machine_api/*.hpp")
add_library( machine_api_plugin
             machine_api_plugin.cpp
             machine_api.cpp
             ${HEADERS}
           )

target_link_libraries( machine_api_plugin chain_plugin json_rpc_plugin xgtvm )
target_include_directories( machine_api_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/../../../vendor/xgtvm/libraries" )

if( CLANG_TIDY_EXE )
   set_target_properties(
      machine_api_plugin PROPERTIES
      CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
   )
endif( CLANG_TIDY_EXE )

install( TARGETS
   machine_api_plugin

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
