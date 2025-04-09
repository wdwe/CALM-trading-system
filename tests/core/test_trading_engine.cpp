#include "core/trading_engine.h"
#include "utils/logging.h"
#include "algo/algo_engine.h"
#include <chrono>

using namespace std::chrono_literals;

int main() {
    calm::init_root_logger("test_trading_engine.log");
    calm::EventEngine event_engine;
    calm::TradingEngine engine{event_engine};
    calm::AlgoEngine algo(&engine);
    event_engine.start();
    engine.start();
    algo.start();
    std::this_thread::sleep_for(100s);
    algo.stop();
    engine.stop();
    event_engine.stop();
}