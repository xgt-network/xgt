file(GLOB_RECURSE HEADERS include/*.hpp)
add_library( appbase application.cpp ${HEADERS})

find_package(Boost 1.58 QUIET REQUIRED COMPONENTS program_options)
target_link_libraries( appbase Boost::program_options )

target_include_directories( appbase PUBLIC include)