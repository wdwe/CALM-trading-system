add_executable(test_redis
        test_redis.cpp
)

target_link_libraries(test_redis
        calm
)

target_link_directories(test_redis
        PUBLIC ${CMAKE_BINARY_DIR}/lib

)

target_include_directories(test_redis
        PUBLIC ${CMAKE_SOURCE_DIR}/lib
)