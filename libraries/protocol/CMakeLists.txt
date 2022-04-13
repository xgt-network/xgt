file(GLOB HEADERS "include/xgt/protocol/*.hpp")
file(GLOB HF_PARTS "hardfork.d/*.hpp")


set( hardfork_hpp_file "${CMAKE_CURRENT_BINARY_DIR}/include/xgt/protocol/hardfork.hpp" )
add_custom_command(
   COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}/../../programs/build_helpers
   python3 -m xgt_build_helpers.cat_parts
   "${CMAKE_CURRENT_SOURCE_DIR}/hardfork.d"
   ${hardfork_hpp_file}
   OUTPUT ${hardfork_hpp_file}
   DEPENDS ${HF_PARTS}
)

## SORT .cpp by most likely to change / break compile
add_library( xgt_protocol

             xtt_operations.cpp
             xtt_util.cpp
             xgt_operations.cpp
             xgt_required_actions.cpp
             xgt_optional_actions.cpp
             types.cpp
             schema_types.cpp
             authority.cpp
             operations.cpp
             sign_state.cpp
             transaction.cpp
             block.cpp
             asset.cpp
             version.cpp
             get_config.cpp

             ${HEADERS}
             ${hardfork_hpp_file}
           )

target_link_libraries( xgt_protocol xgt_schema fc )
target_include_directories( xgt_protocol
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_BINARY_DIR}/include" )
