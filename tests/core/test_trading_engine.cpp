#include "core/trading_engine.h"
#include "utils/logging.h"
#include "algo/algo_engine.h"
#include "core/market_data.h"
#include "cfg/cfg.h"
#include <chrono>

using namespace std::chrono_literals;

int main() {
    calm::init_root_logger("test_trading_engine.log");
    calm::EventEngine event_engine;
    calm::IBGateway gateway{event_engine};
    calm::MarketDataManager mktd_mgr{gateway};
    calm::TradingEngine engine{event_engine, gateway, mktd_mgr};
    calm::AlgoEngine algo(&engine);
    auto const& cfg = calm::Config::get();
    event_engine.start();
    gateway.start(cfg.gateway_host, cfg.gateway_port, cfg.gateway_client_id);
    algo.start();
    std::this_thread::sleep_for(100s);
    algo.stop();
    gateway.stop();
    event_engine.stop();
}