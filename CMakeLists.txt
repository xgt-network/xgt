cmake_minimum_required( VERSION 3.12 )
project( Xgt )
enable_testing()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# http://stackoverflow.com/a/18369825
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
        message(FATAL_ERROR "GCC version must be at least 4.8!")
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.3)
        message(FATAL_ERROR "Clang version must be at least 3.3!")
    endif()
endif()

list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules" )

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libraries/fc/CMakeModules" )
list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libraries/fc/GitVersionGen" )
include( GetGitRevisionDescription )
get_git_head_revision( GIT_REFSPEC GIT_SHA2 )

OPTION( BUILD_XGT_TESTNET "Build source for test network (ON OR OFF)" OFF )
MESSAGE( STATUS "BUILD_XGT_TESTNET: ${BUILD_XGT_TESTNET}" )
if( BUILD_XGT_TESTNET )
  add_compile_definitions("IS_TEST_NET")
endif()

OPTION( XGT_MAX_VOTED_WITNESSES "Number of max voted witnesses" OFF )
MESSAGE( STATUS "XGT_MAX_VOTED_WITNESSES: ${XGT_MAX_VOTED_WITNESSES}" )
if( XGT_MAX_VOTED_WITNESSES )
  add_compile_definitions("XGT_MAX_VOTED_WITNESSES=${XGT_MAX_VOTED_WITNESSES}")
endif()

OPTION( XGT_MAX_MINER_WITNESSES "Number of max miner witnesses" OFF )
MESSAGE( STATUS "XGT_MAX_MINER_WITNESSES: ${XGT_MAX_MINER_WITNESSES}" )
if( XGT_MAX_MINER_WITNESSES )
  add_compile_definitions("XGT_MAX_MINER_WITNESSES=${XGT_MAX_MINER_WITNESSES}")
endif()

OPTION( ENABLE_MIRA "Build source with MIRA (ON OR OFF)" ON )
MESSAGE( STATUS "ENABLE_MIRA: ${ENABLE_MIRA}" )
if( ENABLE_MIRA )
  add_compile_definitions("ENABLE_MIRA")
endif()

OPTION( LOW_MEMORY_NODE "Build source for low memory node (ON OR OFF)" OFF )
MESSAGE( STATUS "LOW_MEMORY_NODE: ${LOW_MEMORY_NODE}" )
if( LOW_MEMORY_NODE )
  add_compile_definitions("IS_LOW_MEM")
endif()

OPTION( SKIP_BY_TX_ID "Skip ordering operation history by transaction id (ON or OFF)" OFF )
MESSAGE( STATUS "SKIP_BY_TX_ID: ${SKIP_BY_TX_ID}" )
if( SKIP_BY_TX_ID )
  add_compile_definitions("SKIP_BY_TX_ID")
endif()

OPTION( XGT_STATIC_BUILD "Build xgtd as a static library (ON or OFF)" OFF )
MESSAGE( STATUS "XGT_STATIC_BUILD: ${XGT_STATIC_BUILD}" )

OPTION( XGT_OPT_NATIVE "Optimize for native architecture" OFF )
if ( XGT_OPT_NATIVE )
  add_compile_options(-march=native)
endif()
MESSAGE( STATUS "XGT_OPT_NATIVE: ${XGT_OPT_NATIVE}" )

OPTION(FAIL_ON_WARNINGS "Fail on compiler warnings" OFF)
MESSAGE( STATUS "FAIL_ON_WARNINGS: ${FAIL_ON_WARNINGS}" )

SET( Boost_USE_STATIC_LIBS ON CACHE STRING "ON or OFF" )

# The practice of declaring the Bind placeholders (_1, _2, ...) in the global namespace is deprecated. Please use
# <boost/bind/bind.hpp> + using namespace boost::placeholders, or define BOOST_BIND_GLOBAL_PLACEHOLDERS to retain the
# current behavior.
# TODO(raggi): actually fixup the usage instead.
# TODO(raggi): move this to more specific targets in fc and the places where it's used in libraries/ such as chain.
add_compile_definitions("BOOST_BIND_GLOBAL_PLACEHOLDERS")
add_compile_definitions("BOOST_THREAD_DONT_PROVIDE_PROMISE_LAZY")

find_package(Backtrace QUIET)
if(Backtrace_LIBRARY)
  add_compile_definitions(BOOST_STACKTRACE_USE_BACKTRACE)
else()
  if(MINGW)
    add_compile_definitions(BOOST_STACKTRACE_USE_WINDBG)
  else()
    add_compile_definitions(BOOST_STACKTRACE_USE_ADDR2LINE)
    # Boosts stacktrace lib leaves a fair bit to be desired, but let's at least
    # use llvm-addr2line that can resolve symbols in less than 5 minutes per
    # symbol in these whacky objects. Good job llvm team.
    add_compile_definitions(BOOST_STACKTRACE_ADDR2LINE_LOCATION=/usr/bin/llvm-addr2line)
  endif()
endif()

# based on http://www.delorie.com/gnu/docs/gdb/gdb_70.html
# uncomment this line to tell GDB about macros (slows compile times)
# set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-2 -g3" )

set(ENABLE_COVERAGE_TESTING FALSE CACHE BOOL "Build Xgt for code coverage analysis")

if(ENABLE_COVERAGE_TESTING)
    SET(CMAKE_CXX_FLAGS "--coverage ${CMAKE_CXX_FLAGS}")
endif()

if(FAIL_ON_WARNINGS)
  add_compile_options(-Wall -Werror -Wno-conversion -Wno-deprecated-declarations -Wno-error=deprecated-copy -Wno-error=pessimizing-move -Wno-error=maybe-uninitialized -Wno-error=shadow -Wno-error=sign-compare)
endif()

if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
    add_compile_options(-fno-builtin-memcmp)
endif()

if(MINGW)

  if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
    add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wa,-mbig-obj>)
    # The above seems to be insufficient sometimes, unclear why.
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")
  endif()

  # # rocksdb sets this flag unconditionally, so we have to in order to have a compatible exception abi
  # add_compile_options( -fno-asynchronous-unwind-tables )
endif()

# When using ccache colored diagnostics are suppressed by default (as they also
# often are using Ninja, etc). This option forcefully re-enables them.
option (COLOR_DIAGNOSTICS "Force colored diagnostics" ON)
if (COLOR_DIAGNOSTICS)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()

# external_plugins needs to be compiled first because libraries/app depends on XGT_EXTERNAL_PLUGINS being fully populated
add_subdirectory( external_plugins )
add_subdirectory( libraries )
add_subdirectory( programs )
add_subdirectory( tests )
