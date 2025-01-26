#ifndef CALM_TRADER_ENUMS_H
#define CALM_TRADER_ENUMS_H

#include <type_traits>
#include <string>
#include <unordered_map>

namespace calm {
/*
 * utils
 */
    template<typename Enum>
    typename std::underlying_type<Enum>::type enum_to_integral(Enum enum_obj) {
        return static_cast<typename std::underlying_type<Enum>::type>(enum_obj);
    }


    template<typename K, typename V>
    std::unordered_map<V, K> invert_KV(std::unordered_map<K, V> const & kv) {
        std::unordered_map<V, K> vk;
        for (auto &[k, v]: kv) {
            vk[v] = k;
        }
        return vk;
    }

/*
 * order
 */

    enum class Action {
            BUY,
            SELL
        };

    extern std::unordered_map<Action, std::string> action_to_ib;
    extern std::unordered_map<std::string, Action> action_to_calm;


    enum class OrderType {
        LIMIT,
        MARKET
    };

    extern std::unordered_map<OrderType, std::string> order_type_to_ib;
    extern std::unordered_map<std::string, OrderType> order_type_to_calm;


    enum class OrderStatus {
        PENDING_SUBMIT,
        PENDING_CANCEL,
        PRE_SUBMITTED,
        SUBMITTED,
        API_CANCELLED,
        CANCELLED,
        FILLED,
        INACTIVE
    };


    extern std::unordered_map<OrderStatus, std::string> order_status_to_ib;
    extern std::unordered_map<std::string, OrderStatus> order_status_to_calm;


    enum class EventType {
        // event engine
        EVENT_ENGINE_STOP,

        // testing
        EVENT_ENGINE_TEST,
        EVENT_ENGINE_TEST_1,

        // IB gateway
        TICK_DATA,
        ORDER_DATA,
        IB_ERR_MSG
    };

}



#endif //CALM_TRADER_ENUMS_H
