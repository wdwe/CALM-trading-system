# super quick
add_executable(super
        super_quick_test.cpp
)


# quick test

add_executable(quick
        quick_test.cpp
)

target_link_libraries(quick
        calm
        tz
)


target_link_directories(quick
        PUBLIC ${CMAKE_BINARY_DIR}/lib
        PUBLIC ${CMAKE_BINARY_DIR}/external/date
)

target_include_directories(quick
        PUBLIC ${CMAKE_SOURCE_DIR}/lib
        PUBLIC ${CMAKE_SOURCE_DIR}/external/date/include
        PUBLIC ${CMAKE_SOURCE_DIR}/external/tws/client
)
