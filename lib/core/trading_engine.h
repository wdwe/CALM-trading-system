#ifndef CALM_TRADER_TRADING_ENGINE_H
#define CALM_TRADER_TRADING_ENGINE_H
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <mutex>
#include <tuple>

#include "event.h"
#include "gateway/gateway.h"


namespace calm {
    class TradingEngine {
    public:
        TradingEngine();
        TradingEngine(TradingEngine const &other) = delete;
        TradingEngine(TradingEngine &&other) = delete;
        void start();
        void stop();
        void subscribe(std::string const & symbol, bool delayed=false);
        std::shared_ptr<TickData> get_last_tick(std::string const&);
        void send_event(Event&& event);
        bool register_cb(EventType const& e_type, std::string const& cb_name, std::function<void(Event const&)> cb);
    private:
        bool running{false};
        std::shared_ptr<spdlog::async_logger> logger;
        EventEngine event_engine;
        IBGateway gateway;


        // register_member
        bool register_cb(EventType const& e_type, std::string const& cb_name, void(TradingEngine::*)(Event const&));

        // gateway mutex
        std::mutex gateway_mutex;

        // market data
        std::mutex ticks_mutex;
        std::unordered_map<std::string, std::shared_ptr<TickData>> last_ticks;
        void update_ticks(Event const& event);

    };



}
#endif //CALM_TRADER_TRADING_ENGINE_H
