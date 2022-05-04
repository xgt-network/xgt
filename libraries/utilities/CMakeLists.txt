list( APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/libraries/fc/GitVersionGen" )
include( GetGitRevisionDescription )
get_git_head_revision(GIT_REFSPEC XGT_GIT_REVISION_SHA)
get_git_unix_timestamp(XGT_GIT_REVISION_UNIX_TIMESTAMP)
git_describe(XGT_GIT_REVISION_DESCRIPTION --tags)
if(NOT XGT_GIT_REVISION_DESCRIPTION)
    set(XGT_GIT_REVISION_DESCRIPTION "unknown")
endif(NOT XGT_GIT_REVISION_DESCRIPTION)

file(GLOB HEADERS "include/xgt/utilities/*.hpp")

set(sources
   benchmark_dumper.cpp
   key_conversion.cpp
   string_escape.cpp
   tempdir.cpp
   logging_config.cpp
   database_configuration.cpp
   ${HEADERS})

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/git_revision.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/git_revision.cpp" @ONLY)
list(APPEND sources "${CMAKE_CURRENT_BINARY_DIR}/git_revision.cpp")

add_library( xgt_utilities
             ${sources}
             ${HEADERS} )
target_link_libraries( xgt_utilities PUBLIC fc )
target_include_directories( xgt_utilities
                            PUBLIC include )
