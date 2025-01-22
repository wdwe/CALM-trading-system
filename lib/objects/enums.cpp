#include "enums.h"
#include <stdexcept>
#include "fmt/format.h"

namespace calm {
/*
 * order
 */
    std::unordered_map<Action, std::string> action_to_ib = {
            {Action::BUY, "BUY"},
            {Action::SELL, "SELL"}
    };

    std::unordered_map<std::string, Action> action_to_calm =  invert_KV(action_to_ib);


    std::unordered_map<OrderType, std::string> order_type_to_ib {
            {OrderType::LIMIT, "LMT"},
            {OrderType::MARKET, "MKT"}
    };

    std::unordered_map<std::string, OrderType> order_type_to_calm = invert_KV(order_type_to_ib);


    std::unordered_map<OrderStatus, std::string> order_status_to_ib {
            {OrderStatus::PENDING_SUBMIT, "PendingSubmit"},
            {OrderStatus::PENDING_CANCEL, "PendingCancel"},
            {OrderStatus::PRE_SUBMITTED, "PreSubmitted"},
            {OrderStatus::SUBMITTED, "Submitted"},
            {OrderStatus::API_CANCELLED, "ApiCancelled"},
            {OrderStatus::CANCELLED, "Cancelled"},
            {OrderStatus::FILLED, "Filled"},
            {OrderStatus::INACTIVE, "Inactive"}
    };

    std::unordered_map<std::string, OrderStatus> order_status_to_calm = invert_KV(order_status_to_ib);
}