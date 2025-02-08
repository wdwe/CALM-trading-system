#include <vector>

#include "utils/logging.h"
#include "core/event.h"
#include "gateway/gateway.h"
#include "data/historical_data.h"

int main() {
    auto logger = calm::init_root_logger("historian.log");
    logger->info("Starting historian");

    calm::EventEngine event_engine;
    calm::IBGateway gateway{event_engine};

//    std::vector<calm::HistBarReq> reqs;
//    reqs.emplace_back("6758-STK-JPY.TSEJ", "2025-01-03 00:00:00", "2025-01-10 00:01:05", "UTC", "TRADES", "./sony.csv");
//    reqs.emplace_back("7203-STK-JPY.TSEJ", "2025-01-03 00:00:00", "2025-01-10 00:00:00", "UTC", "TRADES", "./toyota.csv");

//    calm::HistBarGetter hist_getter{event_engine, gateway, reqs};

//    calm::HistBarGetter hist_getter{event_engine, gateway};
    calm::HistBarGetter hist_getter{event_engine, gateway, "../lib/cfg/hist_download.yaml"};

    event_engine.start();
    gateway.start();

    hist_getter.run();

    gateway.stop();
    event_engine.stop();

    return 0;
}