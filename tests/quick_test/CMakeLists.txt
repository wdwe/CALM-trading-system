# super quick test
find_package(CURL REQUIRED)

add_executable(quick
        quick_test.cpp
)

add_executable(super
        super_quick_test.cpp
)

target_link_libraries(super
        PUBLIC tws
        PUBLIC tz
        ${CURL_LIBRARIES}
)

# quick test

target_link_libraries(quick
        PUBLIC tws
        PUBLIC tz
        ${CURL_LIBRARIES}
)


target_link_directories(quick
        PUBLIC ${CMAKE_BINARY_DIR}/external/tws/client
        PUBLIC ${CMAKE_BINARY_DIR}/lib
)

target_include_directories(quick
        PUBLIC ${CMAKE_SOURCE_DIR}/external/tws/client
        PUBLIC ${CURL_INCLUDE_DIR}
)
