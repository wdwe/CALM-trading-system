#ifndef CALM_TRADER_ALGO_ENGINE_H
#define CALM_TRADER_ALGO_ENGINE_H
#include <string>
#include <memory>
#include <utils/logging.h>

#include "core/trading_engine.h"

namespace calm {
    struct TickMsg {
        char symbol[32];
        double last_price;
        long long timestamp;
    };

    struct PosMsg {
        char symbol[32];
        long long pos;
        long long timestamp;
    };

    std::string to_string(TickMsg const &msg);
    std::string to_string(PosMsg const &msg);

    class AlgoEngine {
    public:
        AlgoEngine(TradingEngine* trading_engine);
        // technically, copy and move constructors should be deleted, but then this
        // class cannot be initialised into a tuple in the Trader class. This is a design tradeoff.
        AlgoEngine(AlgoEngine const& other) = default;
        AlgoEngine(AlgoEngine &&other) = default;
        void init();
        void start();
        void stop();
    private:
        TradingEngine* trading_engine{nullptr};
        std::shared_ptr<spdlog::async_logger> logger;
        bool running{false};
        bool register_cb(EventType const &e_type, std::string const &cb_name, void (AlgoEngine::*cb)(Event const &));
        void tick_callback(Event const &);
    };

}


#endif //CALM_TRADER_ALGO_ENGINE_H
