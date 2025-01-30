#include "core/event.h"
#include "gateway/gateway.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "utils/logging.h"
#include "objects/objects.h"
#include "objects/enums.h"
#include "spdlog/spdlog.h"


using namespace std::chrono_literals;
using namespace calm;

void print_event(Event const& event) {
    if (event.e_type == EventType::tick_data) {
        auto data = std::static_pointer_cast<TickData>(event.data);
        std::cout << to_string(*data) << std::endl;
    } else if (event.e_type == EventType::order_data) {
        auto data = std::static_pointer_cast<OrderData>(event.data);
        std::cout << to_string(*data) << std::endl;
    } else if (event.e_type == EventType::hist_bar) {
        auto data = std::static_pointer_cast<HistBar>(event.data);
        std::cout << to_string(*data) << std::endl;
    } else if (event.e_type == EventType::hist_bar_end) {
        auto data = std::static_pointer_cast<HistBarEnd>(event.data);
        std::cout << to_string(*data) << std::endl;
    }
}



int main() {
    auto logger = init_root_logger("test.log", spdlog::level::debug);
    logger->info("test_ib_api");

    EventEngine event_engine;
    IBGateway gateway{event_engine};
//    IBApi api{gateway};

    std::vector<EventType> event_types{
            EventType::order_data, EventType::tick_data, EventType::hist_bar, EventType::hist_bar_end
    };

    for (auto t: event_types) {
        event_engine.register_cb(t, "print_event", print_event);
    }

    event_engine.start();

    auto host = "";
    int port = 7497;
    int client_id{0};
//    api.start(host, port, client_id);
    gateway.start(host, port, client_id);
/*
 * Live Data Subscription
 */
//    api.subscribe("USD-CASH-SGD.IDEALPRO", false);// 6758-STK-JPY.TSEJ ETH-CRYPTO-USD.PAXOS USD-CASH-SGD.IDEALPRO 1810-FUT-HKD.HKFE

/*
 * Order
 */
//    OrderReq req{"ADS-STK-EUR.SMART", "SMART", Action::buy, OrderType::limit, 10, 200};
//    OrderId id = api.send_order(req);
//    logger->info("Order id is {}", id);
//    api.cancel_order(21);
//    OrderReq req_1{"1810-STK-HKD.SEHK", "SEHK", Action::buy, OrderType::limit, 400, 36, 19};
//    OrderId id_1 = api.send_order(req_1);
//    logger->info("OrderId is {}", id_1);
//    api.send_order("USD-CASH-SGD.IDEALPRO", "buy", 200, 1.359);
//    api.send_order("ETH-CRYPTO-USD.PAXOS", "buy", 0.012, 3120);

/*
 * Contract Details
 */
//    api.req_contract_details("6758-STK-JPY.SMART");

/*
 *  Historical Data
 */
    gateway.req_historical_bar("6758-STK-JPY.SMART", "20250129 14:20:31 Asia/Singapore", "1800 S", "5 secs", "TRADES", 1, 1);



    while (true) {
        std::this_thread::sleep_for(1ms);
    }
    gateway.stop();
    return 0;
}