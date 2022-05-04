add_executable( get_dev_key get_dev_key.cpp )
target_link_libraries( get_dev_key
                       PRIVATE xgt_chain xgt_protocol xgt_utilities fc)


add_executable( sign_digest sign_digest.cpp )
target_link_libraries( sign_digest
                       PRIVATE xgt_chain xgt_protocol xgt_utilities fc)

add_executable( sign_transaction sign_transaction.cpp )
target_link_libraries( sign_transaction
                       PRIVATE xgt_chain xgt_protocol xgt_utilities fc)

add_executable( dump_xgt_schema dump_xgt_schema.cpp )
target_link_libraries( dump_xgt_schema
                       PRIVATE xgt_chain xgt_protocol fc)

add_executable( schema_test schema_test.cpp )
add_test(NAME schema_test COMMAND schema_test)
target_link_libraries( schema_test
                       PRIVATE xgt_chain fc)

add_executable( test_block_log test_block_log.cpp )
add_test(NAME test_block_log COMMAND test_block_log)
target_link_libraries( test_block_log
                       PRIVATE xgt_chain xgt_protocol fc ${CMAKE_DL_LIB})

add_executable( test_fixed_string test_fixed_string.cpp )
add_test(NAME test_fixed_string COMMAND test_fixed_string)
target_link_libraries( test_fixed_string
                       PRIVATE xgt_chain xgt_protocol fc ${CMAKE_DL_LIB})

add_executable( test_sqrt test_sqrt.cpp )
add_test(NAME test_sqrt COMMAND test_sqrt)
target_link_libraries( test_sqrt PRIVATE fc)
