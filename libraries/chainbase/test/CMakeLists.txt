find_package(Boost REQUIRED QUIET COMPONENTS unit_test_framework)
file(GLOB UNIT_TESTS "*.cpp")
add_executable( chainbase_test ${UNIT_TESTS}  )
target_link_libraries( chainbase_test  chainbase mira Boost::unit_test_framework)

