add_executable( js_operation_serializer main.cpp )

target_link_libraries( js_operation_serializer
                       PRIVATE xgt_chain xgt_protocol fc)
