file( GLOB HEADERS "include/xgt/plugins/contract_api/*.hpp" )
set( SOURCES contract_api.cpp contract_api_plugin.cpp )

add_library( contract_api_plugin ${SOURCES} ${HEADERS} )

target_link_libraries( contract_api_plugin
                       chain_plugin
                       json_rpc_plugin
                       xgtvm
                     )
target_include_directories( contract_api_plugin
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
                                   "${CMAKE_CURRENT_SOURCE_DIR}/../../../vendor/xgtvm/libraries"
                          )

if( CLANG_TIDY_EXE )
   set_target_properties(
      contract_api_plugin PROPERTIES
      CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
   )
endif( CLANG_TIDY_EXE )

install( TARGETS
   contract_api_plugin

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
