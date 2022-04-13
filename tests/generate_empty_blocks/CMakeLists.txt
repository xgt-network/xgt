add_executable( generate_empty_blocks main.cpp )

target_link_libraries( generate_empty_blocks
                       PRIVATE vaporware_app vaporware_chain fc)

install( TARGETS
   generate_empty_blocks

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
