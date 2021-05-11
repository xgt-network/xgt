file(GLOB HEADERS "include/xgt/chain/*.hpp" "include/xgt/chain/util/*.hpp" "include/xgt/chain/xtt_objects/*.hpp")

## SORT .cpp by most likely to change / break compile
add_library( xgt_chain

             # As database takes the longest to compile, start it first
             database.cpp
             index.cpp

             machine.cpp

             xtt_evaluator.cpp

             xtt_objects/nai_pool.cpp
             xgt_evaluator.cpp
             xgt_objects.cpp
             required_action_evaluator.cpp
             optional_action_evaluator.cpp

             fork_database.cpp

             shared_authority.cpp
             block_log.cpp

             generic_custom_operation_interpreter.cpp

             util/rd.cpp
             util/impacted.cpp
             util/advanced_benchmark_dumper.cpp
             util/xtt_token.cpp

             ${HEADERS}
           )

target_link_libraries( xgt_chain xgt_protocol fc chainbase xgt_schema appbase mira
                       ${PATCH_MERGE_LIB} )
target_include_directories( xgt_chain
                            PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_BINARY_DIR}/include" )

if( CLANG_TIDY_EXE )
   set_target_properties(
      xgt_chain PROPERTIES
      CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
   )
endif( CLANG_TIDY_EXE )

if(MSVC)
  set_source_files_properties( database.cpp PROPERTIES COMPILE_FLAGS "/bigobj" )
endif(MSVC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wno-error=shadow -Wno-error=deprecated-copy -Wno-error=pessimizing-move -Wno-error=sign-compare")

INSTALL( TARGETS
   xgt_chain

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
INSTALL( FILES ${HEADERS} DESTINATION "include/xgt/chain" )
