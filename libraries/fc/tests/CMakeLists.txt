
add_executable( bip_lock bip_lock.cpp )
target_link_libraries( bip_lock fc )

if( ECC_IMPL STREQUAL secp256k1 )
    add_executable( blind all_tests.cpp crypto/blind.cpp )
    target_link_libraries( blind fc )
endif()

add_executable( task_cancel_test all_tests.cpp thread/task_cancel.cpp )
target_link_libraries( task_cancel_test fc )

add_executable( thread_test all_tests.cpp thread/thread_tests.cpp )
target_link_libraries( thread_test fc )

add_executable( bloom_test all_tests.cpp bloom_test.cpp )
target_link_libraries( bloom_test fc )

add_executable( real128_test all_tests.cpp real128_test.cpp )
target_link_libraries( real128_test fc )

add_executable( hmac_test hmac_test.cpp )
target_link_libraries( hmac_test fc )

add_executable( ecc_test crypto/ecc_test.cpp )
target_link_libraries( ecc_test fc )

add_executable( log_test crypto/log_test.cpp )
target_link_libraries( log_test fc )

add_executable( saturation_test saturation_test.cpp )
target_link_libraries( saturation_test fc )

add_executable( ecdsa_canon_test crypto/ecdsa_canon_test.cpp )
target_link_libraries( ecdsa_canon_test fc )

add_executable( all_tests all_tests.cpp
                          crypto/aes_test.cpp
                          crypto/base_n_tests.cpp
                          crypto/bigint_test.cpp
                          crypto/blind.cpp
                          crypto/rand_test.cpp
                          crypto/sha_tests.cpp
                          thread/task_cancel.cpp
                          thread/thread_tests.cpp
                          bloom_test.cpp
                          real128_test.cpp
                          saturation_test.cpp
                          utf8_test.cpp
                          )
target_link_libraries( all_tests fc )
