#include "trading_engine.h"
#include "objects/objects.h"
#include "cfg/cfg.h"
#include "utils/logging.h"


namespace calm {
    TradingEngine::TradingEngine(EventEngine& event_engine): event_engine{event_engine}, gateway{event_engine} {
        logger = init_sub_logger("trading_engine");
        register_cb(EventType::tick_data, "trading_engine_tick_update", &TradingEngine::update_ticks);

    }

    void TradingEngine::start() {
        running = true;
        auto const& cfg = Config::get();
        gateway.start(cfg.gateway_host, cfg.gateway_port, cfg.gateway_client_id);
    }


    void TradingEngine::stop() {
        if (!running) return;
        running = false;
        gateway.stop();
    }

    void TradingEngine::subscribe(std::string const &symbol, bool delayed) {
        {
            std::lock_guard lock{ticks_mutex};
            if (last_ticks.contains(symbol)) {
                logger->info("in subscribe - {} is already subscribed", symbol);
                return;
            }
            last_ticks[symbol] = std::make_shared<TickData>();
        }
        logger->info("in subscribe - subscribing to {} delayed:{}", symbol, delayed);
        gateway.subscribe(symbol, delayed);
    }

    std::shared_ptr<TickData> TradingEngine::get_last_tick(std::string const& symbol) {
        std::lock_guard lock{ticks_mutex};
        if (last_ticks.contains(symbol)) {
            return last_ticks[symbol];
        }
        return nullptr;
    }


    OrderId TradingEngine::send_order(OrderReq const & order_req) {
        // TODO: add Risk Management checks here
        return gateway.send_order(order_req);
    }

    void TradingEngine::cancel_order(calm::OrderId order_id) {
        gateway.cancel_order(order_id);
    }


    void TradingEngine::send_event(Event &&event) {
        event_engine.send(event);
    }


    bool TradingEngine::register_cb(const calm::EventType &e_type, const std::string &cb_name,
                                    std::function<void(const Event &)> cb) {
        return event_engine.register_cb(e_type, cb_name, cb);
    }

    bool TradingEngine::register_cb(EventType const &e_type, std::string const &cb_name, void(TradingEngine::*cb)(Event const&)) {
        // interestingly, because lambda creates a local class which has the same access as the enclosing scope
        // private member functions can be called inside the lambda by event engine
        return register_cb(e_type, cb_name, [this, cb](Event const& event){(this->*cb)(event);});
    }

    void TradingEngine::update_ticks(Event const &event) {
        auto data = std::static_pointer_cast<TickData>(event.data);
        {
            std::lock_guard lock{ticks_mutex};
            last_ticks[data->symbol] = data;
        }
    }


}