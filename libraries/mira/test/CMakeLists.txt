find_package(Boost REQUIRED QUIET COMPONENTS unit_test_framework)

file(GLOB UNIT_TEST_SOURCES "*.cpp")
file(GLOB UNIT_TEST_HEADERS "*.hpp")
add_executable( mira_test ${UNIT_TEST_SOURCES} ${UNIT_TEST_HEADERS} )
add_test(mira_test mira_test)
target_link_libraries( mira_test mira fc chainbase xgt_protocol xgt_utilities Boost::unit_test_framework)
