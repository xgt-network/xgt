add_executable( xgtd main.cpp )
if( UNIX AND NOT APPLE )
  set(rt_library rt )
endif()

find_package( Gperftools QUIET )
if( GPERFTOOLS_FOUND )
    message( STATUS "Found gperftools; compiling xgtd with TCMalloc")
    list( APPEND PLATFORM_SPECIFIC_LIBS tcmalloc )
endif()

if( XGT_STATIC_BUILD )
   target_link_libraries( xgtd PRIVATE
      "-static-libstdc++ -static-libgcc"
      appbase
      xgt_utilities
      xgt_plugins
      ${CMAKE_DL_LIBS}
      ${PLATFORM_SPECIFIC_LIBS}
   )
else( XGT_STATIC_BUILD )
   target_link_libraries( xgtd PRIVATE
      appbase
      xgt_utilities
      xgt_plugins
      ${CMAKE_DL_LIBS}
      ${PLATFORM_SPECIFIC_LIBS}
   )
endif( XGT_STATIC_BUILD )

if( CLANG_TIDY_EXE )
   set_target_properties(
      xgtd PROPERTIES
      CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
   )
endif( CLANG_TIDY_EXE )

install( TARGETS
   xgtd

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
