#ifndef CALM_TRADER_RISK_H
#define CALM_TRADER_RISK_H

#include <unordered_set>

#include "utils/logging.h"
#include "portfolio.h"
#include "event.h"

namespace calm {
    class RiskManager {
    public:
        RiskManager(EventEngine& event_engine, Portfolio& portfolio);
        RiskManager(RiskManager const&) = delete;
        ~RiskManager() = default;

        bool check_risk(OrderReq const& order_req);
    private:
        bool register_cb(EventType const &e_type, std::string const &cb_name, void(RiskManager::*cb)(Event const&));
        std::shared_ptr<spdlog::async_logger> logger;
        EventEngine& event_engine;
        Portfolio& portfolio;

        std::string get_inner_cfg_str();

        std::unordered_map<std::string, double> algo_pnl_lmt;
        double total_pnl_lmt = -1'000'000;

        void event_engine_order_cb(Event const&);
        std::mutex order_m;
        unsigned int order_flow_lmt{100};
        std::unordered_set<OrderId> active_orders;

    };
}


#endif //CALM_TRADER_RISK_H
