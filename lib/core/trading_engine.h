#ifndef CALM_TRADER_TRADING_ENGINE_H
#define CALM_TRADER_TRADING_ENGINE_H
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <mutex>
#include <tuple>

#include "event.h"
#include "market_data.h"
#include "gateway/gateway.h"
#include "portfolio.h"
#include "risk.h"


namespace calm {
    class TradingEngine {
    public:
        TradingEngine(EventEngine& event_engine, IBGateway& gateway, MarketDataManager& mktd_mgr, RiskManager& risk_manager);
        TradingEngine(TradingEngine const &other) = delete;
        TradingEngine(TradingEngine &&other) = delete;
        void subscribe(std::string const & symbol, bool delayed=false);
        std::shared_ptr<TickData> get_last_tick(std::string const&);
        OrderId send_order(OrderReq const & order_req);
        void cancel_order(OrderId order_id);
        void send_event(Event&& event);
        bool register_cb(EventType const& e_type, std::string const& cb_name, std::function<void(Event const&)> cb);
    private:
        bool running{false};
        std::shared_ptr<spdlog::async_logger> logger;
        EventEngine& event_engine;
        IBGateway& gateway;
        MarketDataManager& mktd_mgr;
        RiskManager& risk_manager;


        // register_member
        bool register_cb(EventType const& e_type, std::string const& cb_name, void(TradingEngine::*)(Event const&));

        // gateway mutex
        std::mutex gateway_mutex;

        // market data
        void update_ticks(Event const& event);

    };



}
#endif //CALM_TRADER_TRADING_ENGINE_H
