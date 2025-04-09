#include "market_data.h"

namespace calm {
    MarketDataManager::MarketDataManager(IBGateway &gateway): gateway{gateway} {
        logger = init_sub_logger("market_data");
    }

    void MarketDataManager::update_ticks(Event const &event) {
        auto data = std::static_pointer_cast<TickData>(event.data);
        {
            std::lock_guard lock{ticks_mutex};
            last_ticks[data->symbol] = data;
        }
    }

    std::shared_ptr<TickData> MarketDataManager::get_last_tick(std::string const &symbol) {
        std::lock_guard lock{ticks_mutex};
        if (last_ticks.contains(symbol)) {
            return last_ticks[symbol];
        }
        return nullptr;
    }

    void MarketDataManager::subscribe(std::string const &symbol, bool delayed) {
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

}
