project( MIRA )
cmake_minimum_required( VERSION 2.8.12 )

file(GLOB_RECURSE sources src/*.cpp include/mira/*.hpp)
add_library(mira ${sources})

FIND_PACKAGE(Boost 1.58 QUIET REQUIRED COMPONENTS filesystem)
target_link_libraries( mira Boost::filesystem rocksdb fc )

target_include_directories( mira
   PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
          "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/rocksdb/include"
)

add_subdirectory( test )