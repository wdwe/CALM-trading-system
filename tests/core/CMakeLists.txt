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