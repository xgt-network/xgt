

find_package(Boost REQUIRED QUIET COMPONENTS unit_test_framework)

if( ECC_IMPL STREQUAL secp256k1 )
    add_executable( blind all_tests.cpp crypto/blind.cpp )
    target_link_libraries( blind fc )
endif()

add_executable( task_cancel_test all_tests.cpp thread/task_cancel.cpp )
target_link_libraries( task_cancel_test fc Boost::unit_test_framework)

add_executable( thread_test all_tests.cpp thread/thread_tests.cpp )
target_link_libraries( thread_test fc  Boost::unit_test_framework)

add_executable( hmac_test hmac_test.cpp )
target_link_libraries( hmac_test fc  Boost::unit_test_framework)

add_executable( ecc_test crypto/ecc_test.cpp )
target_link_libraries( ecc_test fc  Boost::unit_test_framework)

add_executable( log_test crypto/log_test.cpp )
target_link_libraries( log_test fc  Boost::unit_test_framework)

add_executable( saturation_test saturation_test.cpp )
target_link_libraries( saturation_test fc  Boost::unit_test_framework)

add_executable( ecdsa_canon_test crypto/ecdsa_canon_test.cpp )
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
                          utf8_test.cpp
                          )
target_link_libraries( all_tests fc  Boost::unit_test_framework)
