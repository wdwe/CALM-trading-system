#include "core/trader.h"
#include "algo/algo_engine.h"
#include "utils/logging.h"

namespace calm {
    class AlgoDummy {
    public:
        AlgoDummy(TradingEngine& trading_engine, Portfolio& portfolio): trading_engine(trading_engine) {
            logger = init_sub_logger("algo_dummy");
        }
        AlgoDummy(AlgoDummy const&) = default;
        AlgoDummy(AlgoDummy &&) = default;
        void start() {running = true; logger->info("in algo_dummy start");}
        void stop() {running = false; logger->info("in algo_dummy end");}
    private:
        TradingEngine& trading_engine;
        std::shared_ptr<spdlog::async_logger> logger;
        bool running{false};
        void print_timer(Event const& event) {
            auto timer = std::static_pointer_cast<Timer>(event.data);
            logger->info(to_string(*timer));
        }
    };
}



int main() {
    calm::init_root_logger("test_trader.log");
    calm::Trader<calm::AlgoDummy, calm::AlgoEngine> trader;
    trader.run();
    return 0;

}