#include "trading_engine.h"
#include "objects/objects.h"
#include "cfg/cfg.h"
#include "utils/logging.h"


namespace calm {
    TradingEngine::TradingEngine(EventEngine& event_engine, IBGateway& gateway, MarketDataManager& mktd_mgr, RiskManager& risk_manager):
    event_engine{event_engine}, gateway{gateway}, mktd_mgr{mktd_mgr}, risk_manager{risk_manager} {
        logger = init_sub_logger("trading_engine");
        register_cb(EventType::tick_data, "trading_engine_tick_update", &TradingEngine::update_ticks);
    }

    void TradingEngine::subscribe(std::string const &symbol, bool delayed) {
        mktd_mgr.subscribe(symbol, delayed);
    }

    std::shared_ptr<TickData> TradingEngine::get_last_tick(std::string const& symbol) {
        return mktd_mgr.get_last_tick(symbol);
    }


    OrderId TradingEngine::send_order(OrderReq const & order_req) {
        if (!risk_manager.check_risk(order_req)) return -1;
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
        mktd_mgr.update_ticks(event);
    }


}