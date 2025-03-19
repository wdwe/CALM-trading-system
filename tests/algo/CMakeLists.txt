add_executable(test_algo_mq
        test_mq.cpp
)

target_link_libraries(test_algo_mq
        calm
)

target_link_directories(test_algo_mq
        PUBLIC ${CMAKE_BINARY_DIR}/lib

)

target_include_directories(test_algo_mq
        PUBLIC ${CMAKE_SOURCE_DIR}/lib
)