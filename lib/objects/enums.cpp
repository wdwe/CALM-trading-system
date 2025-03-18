#include "enums.h"
#include <stdexcept>
#include "fmt/format.h"

namespace calm {
/*
 * order
 */
    std::unordered_map<Action, std::string> const action_to_ib = {
            {Action::buy,  "BUY"},
            {Action::sell, "SELL"}
    };

    std::unordered_map<std::string, Action> const action_to_calm =  invert_KV(action_to_ib);


    std::unordered_map<OrderType, std::string> const order_type_to_ib {
            {OrderType::limit,  "LMT"},
            {OrderType::market, "MKT"}
    };

    std::unordered_map<std::string, OrderType> const order_type_to_calm = invert_KV(order_type_to_ib);


    std::unordered_map<OrderStatus, std::string> const order_status_to_ib {
            {OrderStatus::pending_submit, "PendingSubmit"},
            {OrderStatus::pending_cancel, "PendingCancel"},
            {OrderStatus::pre_submitted, "PreSubmitted"},
            {OrderStatus::submitted, "Submitted"},
            {OrderStatus::api_cancelled, "ApiCancelled"},
            {OrderStatus::cancelled, "Cancelled"},
            {OrderStatus::filled, "Filled"},
            {OrderStatus::inactive, "Inactive"}
    };

    std::unordered_map<std::string, OrderStatus> const order_status_to_calm = invert_KV(order_status_to_ib);
}