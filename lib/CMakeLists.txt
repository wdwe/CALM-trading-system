add_library(calm
        core/event.cpp
        core/market_data.cpp
        core/trading_engine.cpp
        core/trader.cpp
        core/portfolio.cpp
        core/risk.cpp
        gateway/gateway.cpp
        utils/logging.cpp
        utils/utils.cpp
        utils/lock_based_queue.h
        objects/objects.cpp
        objects/enums.cpp
        objects/constants.h
        objects/utils.cpp
        data/historical_data.cpp
        data/data_recorder.cpp
        cfg/cfg.cpp
        database/redis_client.cpp
        algo/algo_engine.cpp
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