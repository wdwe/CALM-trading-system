add_library(calm
        core/event_engine.cpp
        gateway/api.cpp
        gateway/gateway.cpp
        utils/logging.cpp
        utils/utils.cpp
        objects/objects.cpp
        objects/enums.cpp
        objects/constants.h
)

target_link_libraries(calm
        PUBLIC tws
        PUBLIC twsutils
)

target_link_directories(calm
        PUBLIC ${CMAKE_BINARY_DIR}/external/tws/client
        PUBLIC ${CMAKE_BINARY_DIR}/external/tws/utils
)

target_include_directories(calm
        PUBLIC ${CMAKE_SOURCE_DIR}/external/tws/client
        PUBLIC ${CMAKE_SOURCE_DIR}/external/tws/utils
        PUBLIC ${CMAKE_SOURCE_DIR}/lib/
)