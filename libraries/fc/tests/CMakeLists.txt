

find_package(Boost REQUIRED QUIET COMPONENTS unit_test_framework)

if( ECC_IMPL STREQUAL secp256k1 )
    add_executable( blind all_tests.cpp crypto/blind.cpp )
    target_link_libraries( blind fc )
endif()

add_executable( task_cancel_test all_tests.cpp thread/task_cancel.cpp )
add_test(NAME task_cancel_test COMMAND task_cancel_test)
target_link_libraries( task_cancel_test fc Boost::unit_test_framework)

add_executable( thread_test all_tests.cpp thread/thread_tests.cpp )
add_test(NAME thread_test COMMAND thread_test)
target_link_libraries( thread_test fc  Boost::unit_test_framework)

add_executable( hmac_test hmac_test.cpp )
add_test(NAME hmac_test COMMAND hmac_test)
target_link_libraries( hmac_test fc  Boost::unit_test_framework)

add_executable( ecc_test crypto/ecc_test.cpp )
add_test(NAME ecc_test COMMAND ecc_test)
target_link_libraries( ecc_test fc  Boost::unit_test_framework)

add_executable( log_test crypto/log_test.cpp )
add_test(NAME log_test COMMAND log_test)
target_link_libraries( log_test fc  Boost::unit_test_framework)

add_executable( saturation_test saturation_test.cpp )
add_test(NAME saturation_test COMMAND saturation_test)
target_link_libraries( saturation_test fc  Boost::unit_test_framework)

add_executable( ecdsa_canon_test crypto/ecdsa_canon_test.cpp )
add_test(NAME ecdsa_canon_test COMMAND ecdsa_canon_test)
target_link_libraries( ecdsa_canon_test fc  Boost::unit_test_framework)

add_executable( all_tests all_tests.cpp
                          crypto/aes_test.cpp
                          crypto/base_n_tests.cpp
                          crypto/bigint_test.cpp
                          crypto/blind.cpp
                          crypto/rand_test.cpp
                          crypto/sha_tests.cpp
                          thread/task_cancel.cpp
                          thread/thread_tests.cpp
                          saturation_test.cpp
                          )
add_test(NAME all_tests COMMAND all_tests)
target_link_libraries( all_tests fc  Boost::unit_test_framework)
