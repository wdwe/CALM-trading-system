#include "gateway/api.h"
#include "core/event_engine.h"
#include "gateway/gateway.h"
#include "fmt/format.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "utils/logging.h"


using namespace std::chrono_literals;

int main() {
    // logging
//    auto stdout_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
    auto root_logger = calm::init_root_logger("test.log");
    root_logger->info("test_ib_api");
    calm::EventEngine event_engine;
    calm::IBGateway gateway{event_engine};
    calm::IBApi api{gateway};
    auto host = "";
    int port = 7497;
    api.start(host, port, 0);
    api.subscribe("GBP-CASH-USD.IDEALPRO", false);// 6758-STK-JPY.TSEJ ETH-CRYPTO-USD.PAXOS USD-CASH-SGD.IDEALPRO 1810-FUT-HKD.HKFE
//    api.send_order("USD-CASH-SGD.IDEALPRO", "BUY", 200, 1.359);
//    api.send_order("ETH-CRYPTO-USD.PAXOS", "BUY", 0.012, 3120);
    while (true) {
        std::this_thread::sleep_for(1ms);
    }
    api.stop();
    return 0;
}