//
// Created by wayne on 16/1/25.
//

#ifndef TRADING_GATEWAY_H
#define TRADING_GATEWAY_H

#include <memory>
#include <spdlog/async_logger.h>

#include "core/event.h"
#include "objects/objects.h"

namespace calm {
    class IBApi;

    class IBGateway{
    public:
        IBGateway(EventEngine &event_engine);
        void on_event(Event const & event);
        void on_error(ErrMsg const & err_msg);
        void on_event(Event&& event);
        void on_order(OrderData const & order_data);
        void on_tick(TickData const & tick_data);
        void on_hist_bar(HistBar const& bar);
        void on_hist_bar_end(HistBarEnd const &bar_end);


        std::shared_ptr<spdlog::async_logger> logger;
    private:
        EventEngine &event_engine;
    };
}

#endif //TRADING_GATEWAY_H
