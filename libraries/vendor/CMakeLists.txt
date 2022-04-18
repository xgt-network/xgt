# for each subdirectory containing a CMakeLists.txt, add that subdirectory

set(BUILD_SHARED_LIBS OFF)

#Configure needed build options of RocksDB.
SET(WITH_TESTS OFF CACHE BOOL "build with tests")
SET(WITH_BENCHMARKS OFF CACHE BOOL "build with BENCHMARKS")
SET(WITH_GFLAGS OFF CACHE BOOL "build with GFlags")
SET(USE_RTTI ON CACHE BOOL "build with RTTI")
SET(WITH_TESTS OFF CACHE BOOL "build with tests")
SET(WITH_TOOLS OFF CACHE BOOL "build with tools")
SET(WITH_BENCHMARK OFF CACHE BOOL "build with benchmarks")
SET(WITH_BENCHMARK_TOOLS OFF CACHE BOOL "build with benchmark tools")
SET(WITH_CORE_TOOLS OFF CACHE BOOL "build with core tools")
SET(ROCKSDB_BUILD_SHARED OFF CACHE BOOL "build only static lib")

if(XGT_OPT_NATIVE)
   SET(PORTABLE ON CACHE BOOL "build a portable binary")
else()
   SET(PORTABLE OFF CACHE BOOL "build a portable binary")
endif()

SET(FORCE_SSE42 ON CACHE BOOL "force building with SSE4.2, even when PORTABLE=ON")
SET(FORCE_AVX ON CACHE BOOL "force building with AVX, even when PORTABLE=ON")
SET(FORCE_AVX2 ON CACHE BOOL "force building with AVX2, even when PORTABLE=ON")

file( GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} * )
foreach( child ${children} )
   if( IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${child}" )
      if( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${child}/CMakeLists.txt" )
         add_subdirectory( "${child}" )
      endif()
   endif()
endforeach()

