file(GLOB HEADERS "include/xgt/schema/*.hpp")
add_library( xgt_schema schema.cpp ${HEADERS} )
target_link_libraries( xgt_schema fc )
target_include_directories( xgt_schema PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

install( TARGETS
   xgt_schema

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
install( FILES ${HEADERS} DESTINATION "include/xgt/schema" )
