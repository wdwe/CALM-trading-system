add_executable(test_ib_api
        test_ib_api.cpp
)

target_link_libraries(test_ib_api
        calm
)

target_link_directories(test_ib_api
        PUBLIC ${CMAKE_BINARY_DIR}/lib

)

target_include_directories(test_ib_api
        PUBLIC ${CMAKE_SOURCE_DIR}/lib
)