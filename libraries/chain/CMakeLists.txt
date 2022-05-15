file(GLOB_RECURSE HEADERS include/*.hpp)

## SORT .cpp by most likely to change / break compile
add_library( xgt_chain

             # As database takes the longest to compile, start it first
             database.cpp
             index.cpp

             index_init1.cpp
             index_init2.cpp
             index_init3.cpp

             machine.cpp

             xtt_evaluator.cpp

             xtt_objects/nai_pool.cpp
             xgt_evaluator.cpp
             xgt_objects.cpp
             required_action_evaluator.cpp
             optional_action_evaluator.cpp

             fork_database.cpp

             shared_authority.cpp
             block_log.cpp

             generic_custom_operation_interpreter.cpp

             util/impacted.cpp
             util/advanced_benchmark_dumper.cpp
             util/xtt_token.cpp

             ${HEADERS}
           )

target_link_libraries( xgt_chain xgt_protocol chainbase xgt_schema appbase fc mira)
target_include_directories( xgt_chain PUBLIC include )
