#include "gateway/api.h"
#include "core/event_engine.h"
#include "gateway/gateway.h"
#include "fmt/format.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "utils/logging.h"
#include "objects/objects.h"
#include "objects/enums.h"


using namespace std::chrono_literals;

int main() {
    using namespace calm;

    auto root_logger = init_root_logger("test.log");
    root_logger->info("test_ib_api");

    EventEngine event_engine;
    IBGateway gateway{event_engine};
    IBApi api{gateway};
    auto host = "";
    int port = 7497;
    api.start(host, port, 0);

    api.subscribe("6758-STK-JPY.TSEJ", false);// 6758-STK-JPY.TSEJ ETH-CRYPTO-USD.PAXOS USD-CASH-SGD.IDEALPRO 1810-FUT-HKD.HKFE
//    api.send_order("1810-STK-HKD.SEHK", "BUY", 200, 35.65);
    OrderReq req{"ADS-STK-EUR.SMART", "SMART", Action::BUY, OrderType::LIMIT, 10, 259};
    api.send_order(req);
//    api.send_order("USD-CASH-SGD.IDEALPRO", "BUY", 200, 1.359);
//    api.send_order("ETH-CRYPTO-USD.PAXOS", "BUY", 0.012, 3120);
    while (true) {
        std::this_thread::sleep_for(1ms);
    }
    api.stop();
    return 0;
}