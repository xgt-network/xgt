cmake_minimum_required(VERSION 3.9.4)

set(DEFAULT_XGTD_STATIC ON)
if (APPLE)
   set(DEFAULT_XGTD_STATIC OFF)
endif()
option(XGTD_STATIC "Link xgtd as a static binary" ${DEFAULT_XGTD_STATIC})

add_executable( xgtd main.cpp )

if (XGTD_STATIC)
   if (APPLE)
      message(FATAL_ERROR "Static xgtd is not supported on macOS")
   endif()

   target_link_libraries( xgtd
      -static-libgcc
      -static-libstdc++
   )
endif()

if (XGTD_STATIC)
   target_link_libraries( xgtd -static )
endif()

target_link_libraries( xgtd
   appbase
   xgt_protocol
   xgt_utilities
   xgt_plugins
)

if(Backtrace_STATIC)
    target_link_libraries( xgtd ${Backtrace_STATIC} )
endif()

install( TARGETS
   xgtd

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
