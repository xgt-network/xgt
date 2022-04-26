add_executable( size_checker main.cpp )

target_link_libraries( size_checker
                       PRIVATE xgt_chain xgt_protocol fc)

install( TARGETS
   size_checker

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
