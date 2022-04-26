project( ChainBase )

FIND_PACKAGE(Boost 1.57 QUIET REQUIRED COMPONENTS filesystem)

file(GLOB HEADERS "include/chainbase/*.hpp" "include/chainbase/util/*.hpp")
add_library( chainbase src/chainbase.cpp ${HEADERS} )
target_link_libraries( chainbase  Boost::filesystem )

if(Backtrace_STATIC)
    target_link_libraries( chainbase ${Backtrace_STATIC} )
endif()

target_include_directories( chainbase PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(test)