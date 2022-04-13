add_executable( js_operation_serializer main.cpp )

target_link_libraries( js_operation_serializer
                       PRIVATE xgt_chain xgt_protocol fc)

install( TARGETS
   js_operation_serializer

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
