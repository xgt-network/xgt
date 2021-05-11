file(GLOB HEADERS "include/graphene/time/*.hpp")

set(SOURCES time.cpp)

add_library( graphene_time ${SOURCES} ${HEADERS} )

target_link_libraries( graphene_time
  PUBLIC fc )
target_include_directories( graphene_time
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_BINARY_DIR}/include" )

install( TARGETS
   graphene_time

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
install( FILES ${HEADERS} DESTINATION "include/graphene/time" )
