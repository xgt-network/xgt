PROJECT( fc )
CMAKE_MINIMUM_REQUIRED( VERSION 3.20 )

# Setup module path to make visible used CMake extensions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/GitVersionGen")

INCLUDE(GetGitRevisionDescription)

get_git_head_revision(GIT_REFSPEC FC_GIT_REVISION_SHA)
get_git_unix_timestamp(FC_GIT_REVISION_UNIX_TIMESTAMP)

set( SECP256K1_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/secp256k1-zkp" )
file( GLOB SECP256K1_SOURCES "${SECP256K1_DIR}/src/secp256k1.c" )
add_library( secp256k1 ${SECP256K1_SOURCES} )
target_compile_options(secp256k1 PRIVATE -Wno-error=unused-function)
target_include_directories( secp256k1 PRIVATE "${SECP256K1_DIR}" PUBLIC "${SECP256K1_DIR}/include" )
set( SECP256K1_BUILD_DEFINES
    USE_FIELD_10X26
    USE_FIELD_INV_BUILTIN
    USE_NUM_NONE
    USE_SCALAR_8X32
    USE_SCALAR_INV_BUILTIN )
set_target_properties( secp256k1 PROPERTIES COMPILE_DEFINITIONS "${SECP256K1_BUILD_DEFINES}" LINKER_LANGUAGE C )

option( UNITY_BUILD OFF )

file( GLOB_RECURSE fc_sources src/*.cpp )
file( GLOB_RECURSE fc_headers include/*.hpp )

set( sources ${fc_sources})

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/git_revision.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/git_revision.cpp" @ONLY)
list(APPEND sources "${CMAKE_CURRENT_BINARY_DIR}/git_revision.cpp")
list(APPEND sources ${fc_headers})

add_library( fc ${sources} )

FIND_PACKAGE(Boost 1.53 QUIET REQUIRED COMPONENTS thread date_time system filesystem chrono context coroutine)
target_compile_definitions(fc PUBLIC BOOST_ASIO_HAS_STD_CHRONO)
target_link_libraries( fc PUBLIC Boost::thread Boost::date_time Boost::system Boost::filesystem Boost::chrono Boost::context Boost::coroutine)

# Attempt to find static OpenSSL, fall back to dynamic.
set(OPENSSL_USE_STATIC_LIBS ON)
find_package(OpenSSL)
if (NOT TARGET OpenSSL::Crypto)
  set(OPENSSL_USE_STATIC_LIBS OFF)
  find_package(OpenSSL REQUIRED)
endif()

message(STATUS "OPENSSL_USE_STATIC_LIBS: ${OPENSSL_USE_STATIC_LIBS}")
target_link_libraries(fc PUBLIC OpenSSL::Crypto)

if(MINGW)
  target_link_libraries(fc PUBLIC wsock32 ws2_32 userenv bcrypt)
endif()

target_include_directories(fc
  PUBLIC
    include
    vendor/websocketpp

  PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/secp256k1-zkp
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/bitcoin/src
  )

target_link_libraries( fc PRIVATE secp256k1 )

add_subdirectory( vendor/websocketpp EXCLUDE_FROM_ALL )

add_subdirectory(tests)
