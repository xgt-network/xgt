file(GLOB HEADERS "database_fixture.hpp")
set(SOURCES database_fixture.cpp)

add_library(db_fixture
            ${SOURCES}
            ${HEADERS})

target_link_libraries(db_fixture
                      xgt_chain
                      xgt_protocol
                      wallet_history_plugin
                      block_api_plugin
                      database_api_plugin
		      debug_node_plugin
                      webserver_plugin
                      witness_plugin
                      debug_node_plugin
                      fc
)
