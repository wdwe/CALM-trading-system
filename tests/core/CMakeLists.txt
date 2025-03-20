add_executable(test_event
        test_event.cpp
)

target_link_libraries(test_event
        calm
)

target_link_directories(test_event
        PUBLIC ${CMAKE_BINARY_DIR}/lib

)

target_include_directories(test_event
        PUBLIC ${CMAKE_SOURCE_DIR}/lib
)

add_executable(test_trading_engine
        test_trading_engine.cpp
)

target_link_libraries(test_trading_engine
        calm
)

target_link_directories(test_trading_engine
        PUBLIC ${CMAKE_BINARY_DIR}/lib

)

target_include_directories(test_trading_engine
        PUBLIC ${CMAKE_SOURCE_DIR}/lib
)

add_executable(test_trader
        test_trader.cpp
)

target_link_libraries(test_trader
        calm
)

target_link_directories(test_trader
        PUBLIC ${CMAKE_BINARY_DIR}/lib

)

target_include_directories(test_trader
        PUBLIC ${CMAKE_SOURCE_DIR}/lib
)