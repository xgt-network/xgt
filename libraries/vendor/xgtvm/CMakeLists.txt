cmake_minimum_required(VERSION 2.8.9)
project(Xgtvm)

find_package( Boost REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

# Library
add_library(xgtvm programs/xgtvm.cpp libraries/machine.cpp libraries/machine.hpp)
target_link_libraries( xgtvm LINK_PUBLIC ${Boost_LIBRARIES} )
target_include_directories(xgtvm PRIVATE libraries)
# Binary
add_executable(xgtvmd programs/xgtvm.cpp libraries/machine.cpp libraries/machine.hpp)
target_link_libraries( xgtvmd LINK_PUBLIC ${Boost_LIBRARIES} )
target_include_directories(xgtvmd PRIVATE libraries)
# Tests
add_executable(xgtvm_tests programs/xgtvm_tests.cpp libraries/machine.cpp libraries/machine.hpp)
target_link_libraries( xgtvm_tests LINK_PUBLIC ${Boost_LIBRARIES} )
target_include_directories(xgtvm_tests PRIVATE libraries)
