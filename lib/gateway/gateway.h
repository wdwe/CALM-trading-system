//
// Created by wayne on 16/1/25.
//

#ifndef TRADING_GATEWAY_H
#define TRADING_GATEWAY_H

#include <memory>
#include <spdlog/async_logger.h>
#include "core/event_engine.h"

namespace calm {
    class IBGateway{
    public:
        IBGateway(EventEngine &event_engine);
        std::shared_ptr<spdlog::async_logger> logger;
    private:
        EventEngine &event_engine;
    };
}

#endif //TRADING_GATEWAY_H
