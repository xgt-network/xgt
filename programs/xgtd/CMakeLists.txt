cmake_minimum_required(VERSION 3.9.4)

add_executable( xgtd main.cpp )

if (!MINGW)
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT error)
if( ipo_supported )
       message(STATUS "IPO / LTO enabled")
       set_property(TARGET xgtd PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
       message(STATUS "IPO / LTO not supported: <${error}>")
endif()
endif()

if (NOT APPLE)
target_link_libraries( xgtd
	-static-libgcc
	-static-libstdc++
)
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
