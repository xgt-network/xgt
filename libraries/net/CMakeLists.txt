file(GLOB_RECURSE HEADERS include/*.hpp)
file(GLOB SOURCES *.cpp)

add_library( graphene_net ${SOURCES} ${HEADERS} )

target_link_libraries( graphene_net PUBLIC xgt_protocol fc )
target_include_directories( graphene_net PUBLIC include)