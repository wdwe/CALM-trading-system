//
// Created by wayne on 16/1/25.
//

#include "gateway.h"
#include "utils/logging.h"

namespace calm {
    IBGateway::IBGateway(EventEngine &event_engine): event_engine{event_engine} {
        logger = init_sub_logger("ib_gateway");
    }

    void IBGateway::on_event(calm::Event const & event) {
        event_engine.send(event);
    }

    void IBGateway::on_event(calm::Event &&event) {
        event_engine.send(std::move(event));
    }

    void IBGateway::on_order(calm::OrderData const &order_data) {
        Event event{EventType::order_data, std::make_shared<OrderData>(order_data)};
        on_event(std::move(event));
    }

    void IBGateway::on_tick(TickData const &tick_data) {
        Event event{EventType::tick_data, std::make_shared<TickData>(tick_data)};
        on_event(std::move(event));
    }

    void IBGateway::on_error(ErrMsg const &err_msg) {
        Event event{EventType::ib_err_msg, std::make_shared<ErrMsg>(err_msg)};
        on_event(std::move(event));
    }

    void IBGateway::on_hist_bar(HistBar const &bar) {
        Event event{EventType::hist_bar, std::make_shared<HistBar>(bar)};
        on_event(std::move(event));
    }

    void IBGateway::on_hist_bar_end(HistBarEnd const &bar_end) {
        Event event{EventType::hist_bar_end, std::make_shared<HistBarEnd>(bar_end)};
        on_event(std::move(event));
    }


}



