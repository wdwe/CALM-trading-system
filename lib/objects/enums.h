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
            buy,
            sell
        };

    extern std::unordered_map<Action, std::string> const action_to_ib;
    extern std::unordered_map<std::string, Action> const action_to_calm;


    enum class OrderType {
        limit,
        market
    };

    extern std::unordered_map<OrderType, std::string> const order_type_to_ib;
    extern std::unordered_map<std::string, OrderType> const order_type_to_calm;


    enum class OrderStatus {
        pending_submit,
        pending_cancel,
        pre_submitted,
        submitted,
        api_cancelled,
        cancelled,
        filled,
        inactive
    };


    extern std::unordered_map<OrderStatus, std::string> const order_status_to_ib;
    extern std::unordered_map<std::string, OrderStatus> const order_status_to_calm;


    enum class EventType {
        // event engine
        event_engine_stop,

        // testing
        event_engine_test,
        event_engine_test_1,

        // IB gateway
        tick_data,
        order_data,
        ib_err_msg,
        hist_bar,
        hist_bar_end
    };

}



#endif //CALM_TRADER_ENUMS_H
