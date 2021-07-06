cmake_minimum_required( VERSION 3.12 )

project( Xgt )

set( BLOCKCHAIN_NAME "Xgt" )
set( CMAKE_CXX_STANDARD 14 )

set( GUI_CLIENT_EXECUTABLE_NAME Xgt )
set( CUSTOM_URL_SCHEME "gcs" )
set( INSTALLER_APP_ID "68ad7005-8eee-49c9-95ce-9eed97e5b347" )

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

#set (ENABLE_INSTALLER 1)
#set (USE_PCH 1)

if (USE_PCH)
  include (cotire)
endif(USE_PCH)

list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libraries/fc/CMakeModules" )
list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libraries/fc/GitVersionGen" )
include( GetGitRevisionDescription )
get_git_head_revision( GIT_REFSPEC GIT_SHA2 )

OPTION( BUILD_XGT_TESTNET "Build source for test network (ON OR OFF)" OFF )
MESSAGE( STATUS "BUILD_XGT_TESTNET: ${BUILD_XGT_TESTNET}" )
if( BUILD_XGT_TESTNET  )
  add_compile_definitions("IS_TEST_NET")
endif()

OPTION( XGT_MAX_WITNESSES "Number of max witnesses" OFF )
MESSAGE( STATUS "XGT_MAX_WITNESSES: ${XGT_MAX_WITNESSES}" )
if( XGT_MAX_WITNESSES )
  add_compile_definitions("XGT_MAX_WITNESSES=${XGT_MAX_WITNESSES}")
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

OPTION( ENABLE_MIRA "Build source with MIRA (ON OR OFF)" OFF )
MESSAGE( STATUS "ENABLE_MIRA: ${ENABLE_MIRA}" )
if( ENABLE_MIRA )
  add_compile_definitions("ENABLE_MIRA")
endif()

OPTION( LOW_MEMORY_NODE "Build source for low memory node (ON OR OFF)" OFF )
MESSAGE( STATUS "LOW_MEMORY_NODE: ${LOW_MEMORY_NODE}" )
if( LOW_MEMORY_NODE )
  add_compile_definitions("IS_LOW_MEM")
endif()

OPTION( CHAINBASE_CHECK_LOCKING "Check locks in chainbase (ON or OFF)" ON )
MESSAGE( STATUS "CHAINBASE_CHECK_LOCKING: ${CHAINBASE_CHECK_LOCKING}" )
if( CHAINBASE_CHECK_LOCKING )
  add_compile_definitions("CHAINBASE_CHECK_LOCKING")
endif()

OPTION( SKIP_BY_TX_ID "Skip ordering operation history by transaction id (ON or OFF)" OFF )
MESSAGE( STATUS "SKIP_BY_TX_ID: ${SKIP_BY_TX_ID}" )
if( SKIP_BY_TX_ID )
  add_compile_definitions("SKIP_BY_TX_ID")
endif()

OPTION( XGT_STATIC_BUILD "Build xgtd as a static library (ON or OFF)" OFF )
if( XGT_STATIC_BUILD AND ( ( MSVC AND NOT MINGW ) OR APPLE ) )
   MESSAGE( STATUS "Statuc build is not available on Windows or OS X" )
   SET( XGT_STATIC_BUILD OFF )
endif()
MESSAGE( STATUS "XGT_STATIC_BUILD: ${XGT_STATIC_BUILD}" )

OPTION( XGT_CCACHE "Use ccache during the build" ON )
if ( XGT_CCACHE )
  SET( CMAKE_C_COMPILER_LAUNCHER "ccache" )
  SET( CMAKE_CXX_COMPILER_LAUNCHER "ccache" )
endif()
MESSAGE( STATUS "XGT_CCACHE: ${XGT_CCACHE}" )

OPTION( XGT_OPT_NATIVE "Optimize for native architecture" OFF )
if ( XGT_OPT_NATIVE )
  include(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-march=native" CXX_SUPPORTS_MARCH_NATIVE)
  if(CXX_SUPPORTS_MARCH_NATIVE)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
  endif()

  include(CheckCCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-march=native" C_SUPPORTS_MARCH_NATIVE)
  if(C_SUPPORTS_MARCH_NATIVE)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
  endif()
endif()
MESSAGE( STATUS "XGT_OPT_NATIVE: ${XGT_OPT_NATIVE}" )

SET( XGT_LINT_LEVEL "OFF" CACHE STRING "Lint level during xgt build (FULL, HIGH, LOW, OFF)" )
find_program(
   CLANG_TIDY_EXE
   NAMES "clang-tidy"
   DOC "Path to clain-tidy executable"
)

if( NOT CLANG_TIDY_EXE )
   message( STATUS "clang-tidy not found" )
elseif( VERSION LESS 3.6 )
   messgae( STATUS "clang-tidy found but only supported with CMake version >= 3.6" )
else()
   message( STATUS "clany-tidy found: ${CLANG_TIDY_EXE}" )
   if( "${XGT_LINT_LEVEL}" STREQUAL "FULL" )
      message( STATUS "Linting level set to: FULL" )
      set( DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "-checks='*'" )
   elseif( "${XGT_LINT_LEVEL}" STREQUAL "HIGH" )
      message( STATUS "Linting level set to: HIGH" )
      set( DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "-checks='boost-use-to-string,clang-analyzer-*,cppcoreguidelines-*,llvm-*,misc-*,performance-*,readability-*'" )
   elseif( "${XGT_LINT_LEVEL}" STREQUAL "LOW" )
      message( STATUS "Linting level set to: LOW" )
      set( DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "-checks='clang-analyzer-*'" )
   else()
      unset( CLANG_TIDY_EXE )
      message( STATUS "Linting level set to: OFF" )
   endif()
endif( NOT CLANG_TIDY_EXE )

SET(BOOST_COMPONENTS)
LIST(APPEND BOOST_COMPONENTS thread
                             date_time
                             system
                             filesystem
                             program_options
                             serialization
                             chrono
                             unit_test_framework
                             context
                             locale
                             coroutine)

SET( Boost_USE_STATIC_LIBS ON CACHE STRING "ON or OFF" )

IF( WIN32 )
  SET(BOOST_ROOT $ENV{BOOST_ROOT})
  set(Boost_USE_MULTITHREADED ON)
  set(BOOST_ALL_DYN_LINK OFF) # force dynamic linking for all libraries
ENDIF(WIN32)

FIND_PACKAGE(Boost 1.58 REQUIRED COMPONENTS ${BOOST_COMPONENTS})

if( NOT( Boost_VERSION LESS 106900 ) )
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
   SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
endif()

if( WIN32 )

   message( STATUS "Configuring Xgt on WIN32")
   set( DB_VERSION 60 )
   set( BDB_STATIC_LIBS 1 )

   set( ZLIB_LIBRARIES "" )
   SET( DEFAULT_EXECUTABLE_INSTALL_DIR bin/ )

   set(CRYPTO_LIB)

   if( MSVC )
      add_compile_options(/wd4503 /wd4267 /wd4244)
      #looks like this flag can have different default on some machines.
      SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
      SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")

      # Probably cmake has a bug and vcxproj generated for executable in Debug conf. has disabled debug info
      set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")

      # On windows tcl should be installed to the directory pointed by setenv.bat script
      SET(TCL_INCLUDE_PATH $ENV{TCL_ROOT}/include)
      MESSAGE(STATUS "tcl INCLUDE PATH: ${TCL_INCLUDE_PATH}")

      FIND_PACKAGE(TCL)
      MESSAGE(STATUS "tcl_library: ${TCL_LIBRARY}")

      SET(TCL_LIBS "optimized;${TCL_LIBRARY};debug;")
      get_filename_component(TCL_LIB_PATH "${TCL_LIBRARY}" PATH)
      get_filename_component(TCL_LIB_NAME "${TCL_LIBRARY}" NAME_WE)
      get_filename_component(TCL_LIB_EXT "${TCL_LIBRARY}" EXT)

      SET(TCL_LIBS "${TCL_LIBS}${TCL_LIB_PATH}/${TCL_LIB_NAME}g${TCL_LIB_EXT}")
      SET(TCL_LIBRARY ${TCL_LIBS})

   elseif( MINGW )
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -msse4.2 -Wa,-mbig-obj")
      SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
      # Optimization flag apparently needed to get rid of "File too big" assembler errors when compiling in Debug mode
      # See: https://stackoverflow.com/questions/14125007/gcc-string-table-overflow-error-during-compilation/14601779#29479701
      SET(CMAKE_CXX_FLAGS_DEBUG "-O2")
      if ( FULL_STATIC_BUILD )
        set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
      endif ( FULL_STATIC_BUILD )
   endif( MSVC )

else( WIN32 ) # Apple AND Linux

    if( APPLE )
        # Apple Specific Options Here
        message( STATUS "Configuring Xgt on OS X" )
        set( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++14 -stdlib=libc++ -Wall -Wno-conversion -Wno-deprecated-declarations" )
    else( APPLE )
        # Linux Specific Options Here
        message( STATUS "Configuring Xgt on Linux" )
        set( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++14 -Wall" )
        set( rt_library rt )
        set( pthread_library pthread)
        if ( NOT DEFINED crypto_library )
          # I'm not sure why this is here, I guess someone has openssl and can't detect it with find_package()?
          # if you have a normal install, you can define crypto_library to the empty string to avoid a build error
          set( crypto_library crypto)
        endif ()
        if ( FULL_STATIC_BUILD )
          set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
        endif ( FULL_STATIC_BUILD )
    endif( APPLE )

    if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-builtin-memcmp" )
    endif()

    if( "${CMAKE_GENERATOR}" STREQUAL "Ninja" )
        if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
            set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics" )
        endif()
    endif()

    set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

    # based on http://www.delorie.com/gnu/docs/gdb/gdb_70.html
    # uncomment this line to tell GDB about macros (slows compile times)
    # set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-2 -g3" )

endif( WIN32 )

set(ENABLE_COVERAGE_TESTING FALSE CACHE BOOL "Build Xgt for code coverage analysis")

if(ENABLE_COVERAGE_TESTING)
    SET(CMAKE_CXX_FLAGS "--coverage ${CMAKE_CXX_FLAGS}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wno-error=shadow -Wno-error=sign-compare -DBOOST_THREAD_DONT_PROVIDE_PROMISE_LAZY")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -DBOOST_THREAD_DONT_PROVIDE_PROMISE_LAZY" )

# supress unwanted warnings:
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-Wno-error=deprecated-copy -Wno-error=pessimizing-move" SUPPRESS_SOME_WARNINGS)
if(SUPPRESS_SOME_WARNINGS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=deprecated-copy -Wno-error=pessimizing-move")
endif()

# When using ccache colored diagnostics are suppressed by default (as they also
# often are using Ninja, etc). This option forcefully re-enables them.
option (COLOR_DIAGNOSTICS "Force colored diagnostics" OFF)
if (COLOR_DIAGNOSTICS)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()

# The practice of declaring the Bind placeholders (_1, _2, ...) in the global namespace is deprecated. Please use
# <boost/bind/bind.hpp> + using namespace boost::placeholders, or define BOOST_BIND_GLOBAL_PLACEHOLDERS to retain the
# current behavior.
# TODO(raggi): actually fixup the usage instead.
# TODO(raggi): move this to more specific targets in fc and the places where it's used in libraries/ such as chain.
add_compile_definitions("BOOST_BIND_GLOBAL_PLACEHOLDERS")

# external_plugins needs to be compiled first because libraries/app depends on XGT_EXTERNAL_PLUGINS being fully populated
add_subdirectory( external_plugins )
add_subdirectory( libraries )
add_subdirectory( programs )

#add_subdirectory( tests )

if (ENABLE_INSTALLER)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)


include(InstallRequiredSystemLibraries)

set(CPACK_OUTPUT_FILE_PREFIX ${CMAKE_BINARY_DIR}/packages)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

SET(CPACK_PACKAGE_DIRECTORY "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_NAME "xgt")
set(CPACK_PACKAGE_VENDOR "Gather.com, Inc.")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION "A client for the Xgt network")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A client for the Xgt network")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Xgt ${CPACK_PACKAGE_VERSION}")

if(WIN32)
 SET(CPACK_GENERATOR "ZIP;NSIS")
 set(CPACK_PACKAGE_NAME "Xgt") # override above
 set(CPACK_NSIS_EXECUTABLES_DIRECTORY .)
 set(CPACK_NSIS_PACKAGE_NAME "Xgt v${CPACK_PACKAGE_VERSION}")
 set(CPACK_NSIS_DISPLAY_NAME "${CPACK_NSIS_PACKAGE_NAME}")
 set(CPACK_NSIS_DEFINES "  !define MUI_STARTMENUPAGE_DEFAULTFOLDER \\\"Xgt\\\"")
 # it seems like windows zip files usually don't have a single directory inside them, unix tgz frequently do
 SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)

endif(WIN32)

if(APPLE)
  set(CPACK_GENERATOR "DragNDrop")
endif()

if(LINUX)
  # Linux gets a .tgz
  SET(CPACK_GENERATOR "TGZ")
  SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 1)
endif(LINUX)

 include(CPack)
endif(ENABLE_INSTALLER)
