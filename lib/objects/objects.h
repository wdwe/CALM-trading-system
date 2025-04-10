#ifndef TRADING_OBJECTS_H
#define TRADING_OBJECTS_H

#include <string>
#include <chrono>
#include <iostream>
#include <limits>

#include "Contract.h"
#include "OrderState.h"
#include "Order.h"
#include "enums.h"
#include "constants.h"
#include "bar.h"


namespace calm {
    typedef long OrderId;
/*
 * tws api
 */
    // Contract
    std::string to_string(Contract const& contract);
    // ContractDetails
    std::string to_string(ContractDetails const& contract_details);
    // OrderState
    std::string to_string(OrderState const & order_state);
    // Order
    std::string to_string(Order const &order);
    // Bar
    std::string to_string(Bar const &bar);



/*
 * calm trading
 */
    // Message
    struct ErrMsg {
        int id;
        int error_code;
        std::string message;
        std::string order_rejection_json;
    };
    std::string to_string(ErrMsg const & err_msg);


    // Tick
    struct TickData {
        std::string symbol;
        double last_price{0};
        double last_size{0};
        double bid_price{0};
        double bid_size{0};
        double ask_price{0};
        double ask_size{0};
        time_t timestamp{0};
    };
    std::string to_string(TickData const & tick);

    // Order
    struct OrderReq {
        std::string symbol;
        std::string exchange;
        std::string source;
        Action action;
        OrderType order_type;
        double quantity;
        double price{UnsetDouble};
        OrderId order_id{UnsetLong}; // set to modify order
    };
    std::string to_string(OrderReq const &req);

    struct OrderUpdateReq {
        OrderId order_id;
        std::string symbol;
        double quantity{UnsetDouble};
        double price{UnsetDouble};
    };


    struct OrderData {
        OrderId order_id;
        std::string symbol;
        std::string exchange;
        std::string source;

        OrderType order_type;
        Action action;
        double total_quantity;
        double traded_quantity{0};
        double avg_trade_price{0};
        double commission{0};
        OrderStatus status{OrderStatus::pending_submit};
        int error_code{0};
    };
    std::string to_string(OrderData const & order_data);


    // Historical Data
    struct HistBar {
        long long id;
        std::string time;
        double high;
        double low;
        double open;
        double close;
        double wap;
        double volume;
        int count;
    };
    std::string to_string(HistBar const &bar);

    struct HistBarEnd {
        long long id;
        std::string start;
        std::string end;
    };
    std::string to_string(HistBarEnd const& bar_end);


    // timer
    struct Timer {
        int sec = -1;
        time_t timestamp = 0;
    };

    std::string to_string(Timer const& timer);

    // Portfolio
    struct AlgoPnL {
        std::string algo;
        double pnl;
        time_t timestamp;
    };

    std::string to_string(AlgoPnL const& pnl);

    struct TotalPnL {
        double pnl;
        time_t timestamp;
    };

    std::string to_string(TotalPnL const& pnl);


}

#endif //TRADING_OBJECTS_H
