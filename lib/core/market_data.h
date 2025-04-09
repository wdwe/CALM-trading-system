#ifndef CALM_TRADER_MARKET_DATA_H
#define CALM_TRADER_MARKET_DATA_H
#include "gateway/gateway.h"
#include "utils/logging.h"

namespace calm {
    class MarketDataManager {
    public:
        explicit MarketDataManager(IBGateway& gateway);
        MarketDataManager(MarketDataManager const&) = delete;
        ~MarketDataManager() = default;
        void subscribe(std::string const & symbol, bool delayed=false);
        void update_ticks(Event const& event);
        std::shared_ptr<TickData> get_last_tick(std::string const&);
    private:
        std::shared_ptr<spdlog::async_logger> logger;
        IBGateway& gateway;
        std::mutex ticks_mutex;
        std::unordered_map<std::string, std::shared_ptr<TickData>> last_ticks;

    };
}

#endif //CALM_TRADER_MARKET_DATA_H
