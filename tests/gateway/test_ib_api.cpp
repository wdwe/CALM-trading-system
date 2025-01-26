#include "gateway/api.h"
#include "core/event.h"
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

    auto logger = init_root_logger("test.log");
    logger->info("test_ib_api");

    EventEngine event_engine;
    IBGateway gateway{event_engine};
    IBApi api{gateway};
    auto host = "";
    int port = 7497;
    api.start(host, port, 0);
    api.subscribe("USD-CASH-SGD.IDEALPRO", false);// 6758-STK-JPY.TSEJ ETH-CRYPTO-USD.PAXOS USD-CASH-SGD.IDEALPRO 1810-FUT-HKD.HKFE

/*
 * Order
 */
//    OrderReq req{"ADS-STK-EUR.SMART", "SMART", Action::BUY, OrderType::LIMIT, 10, 200};
//    OrderId id = api.send_order(req);
//    logger->info("Order id is {}", id);
//    api.cancel_order(21);
//    OrderReq req_1{"1810-STK-HKD.SEHK", "SEHK", Action::BUY, OrderType::LIMIT, 400, 36, 19};
//    OrderId id_1 = api.send_order(req_1);
//    logger->info("OrderId is {}", id_1);
//    api.send_order("USD-CASH-SGD.IDEALPRO", "BUY", 200, 1.359);
//    api.send_order("ETH-CRYPTO-USD.PAXOS", "BUY", 0.012, 3120);
/*
 * Contract Details
 */
    api.req_contract_details("6758-STK-JPY.SMART");
    while (true) {
        std::this_thread::sleep_for(1ms);
    }
    api.stop();
    return 0;
}