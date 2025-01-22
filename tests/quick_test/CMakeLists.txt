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
)


target_link_directories(quick
        PUBLIC ${CMAKE_BINARY_DIR}/lib
)

target_include_directories(quick
        PUBLIC ${CMAKE_SOURCE_DIR}/lib
)
