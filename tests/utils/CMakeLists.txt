add_executable(test_queue
        test_queue.cpp
)

target_link_libraries(test_queue
        calm
)

target_link_directories(test_queue
        PUBLIC ${CMAKE_BINARY_DIR}/lib

)

target_include_directories(test_queue
        PUBLIC ${CMAKE_SOURCE_DIR}/lib
)