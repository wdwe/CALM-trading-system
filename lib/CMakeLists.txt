add_library(calm
        core/event.cpp
        gateway/gateway.cpp
        utils/logging.cpp
        utils/utils.cpp
        utils/lock_based_queue.h
        objects/objects.cpp
        objects/enums.cpp
        objects/constants.h
        data/historical_data.cpp
        cfg/cfg.cpp
)

target_link_libraries(calm
        PUBLIC tws
        PUBLIC twsutils
        PUBLIC tz
)

target_link_directories(calm
        PUBLIC ${CMAKE_BINARY_DIR}/external/tws/client
        PUBLIC ${CMAKE_BINARY_DIR}/external/tws/utils
        PUBLIC ${CMAKE_BINARY_DIR}/external/date
)

target_include_directories(calm
        PUBLIC ${CMAKE_SOURCE_DIR}/external/tws/client
        PUBLIC ${CMAKE_SOURCE_DIR}/external/tws/utils
        PUBLIC ${CMAKE_SOURCE_DIR}/external/date/include
        PUBLIC ${CMAKE_SOURCE_DIR}/lib/
)