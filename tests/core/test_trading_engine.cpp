#include "core/trading_engine.h"
#include "utils/logging.h"
#include "algo/algo_engine.h"

int main() {
    calm::init_root_logger("test_trading_engine.log");
    calm::TradingEngine engine;
    calm::AlgoEngine algo(&engine);
    engine.start();
    algo.start();
    algo.stop();
    engine.stop();

}