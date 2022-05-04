file(GLOB HEADERS "include/xgt/schema/*.hpp")
add_library( xgt_schema schema.cpp ${HEADERS} )
target_link_libraries( xgt_schema fc )
target_include_directories( xgt_schema PUBLIC include )
