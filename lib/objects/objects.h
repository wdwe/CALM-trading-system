#ifndef TRADING_OBJECTS_H
#define TRADING_OBJECTS_H

#include <string>
#include <chrono>
#include <iostream>

#include "Contract.h"
#include "OrderState.h"

namespace calm {
    // Contract
    std::string to_string(Contract const& contract);
    // ContractDetails
    std::string to_string(ContractDetails const& contract_details);
    // OrderState
    std::string to_string(OrderState const & order_state);
    // Order
    std::string to_string(Order const &order);

    struct Tick {
        std::string symbol;
        double last_price{0};
        double last_size{0};
        double bid_price{0};
        double bid_size{0};
        double ask_price{0};
        double ask_size{0};
        time_t timestamp{0};
    };
    std::string to_string(Tick const & tick);


}

#endif //TRADING_OBJECTS_H
