#include "risk.h"
#include "objects/utils.h"
#include "cfg/cfg.h"
#include <yaml-cpp/yaml.h>

namespace calm {

    RiskManager::RiskManager(EventEngine& event_engine, Portfolio &portfolio): event_engine{event_engine}, portfolio{portfolio} {
        logger = init_sub_logger("risk");
        auto& global_cfg = Config::get();
        auto cfg = YAML::LoadFile(global_cfg.risk_config_path);
        logger->info("initialising RiskManager");
        for (auto it = cfg["algo_pnl_lmt"].begin(); it != cfg["algo_pnl_lmt"].end(); it++) {
            auto algo_name = it->first.as<std::string>();
            auto algo_pnl = it->second.as<double>();
            algo_pnl_lmt[algo_name] = algo_pnl;
        }
        total_pnl_lmt = cfg["total_pnl_lmt"].as<double>();
        order_flow_lmt = cfg["order_flow_lmt"].as<unsigned int>();
        logger->info(get_inner_cfg_str());

        register_cb(EventType::order_data, "risk_order_cb", &RiskManager::event_engine_order_cb);
    }

    bool RiskManager::check_risk(const calm::OrderReq &order_req) {
        auto algo = order_req.source;
        OrderId order_id = order_req.order_id;
        // check pnl
        if (portfolio.get_total_pnl() < total_pnl_lmt) {
            logger->warn("total_pnl_lmt check failed, order_req:{}", to_string(order_req));
            return false;
        }

        if (algo_pnl_lmt.contains(algo) && portfolio.get_algo_pnl(algo) < algo_pnl_lmt[algo]) {
            logger->warn("algo_pnl_lmt check failed, order_req:{}", to_string(order_req));
            return false;
        }

        // check flow last so order_m does not need to be released and required again
        {
            std::lock_guard lock{order_m};
            if (active_orders.size() >= order_flow_lmt) {
                logger->warn("order_flow_lmt check failed, order_req:{}", to_string(order_req));
                return false;
            }
            else active_orders.insert(order_id);
        }

        return true;
    }


    bool RiskManager::register_cb(EventType const &e_type, std::string const &cb_name, void (RiskManager::*cb)(Event const &)) {
        return event_engine.register_cb(e_type, cb_name, [this, cb](Event const& event){(this->*cb)(event);});
    }

    void RiskManager::event_engine_order_cb(const calm::Event &event) {
        auto order_data = std::static_pointer_cast<OrderData>(event.data);
        if (!is_active_order(order_data->status)) {
            auto order_id = order_data->order_id;
            std::lock_guard lock{order_m};
            active_orders.erase(order_id);
        }
    }

    std::string RiskManager::get_inner_cfg_str() {
        std::string cfg_str{"RiskManager cfg - "};
        cfg_str.append(std::string("algo_pnl_lmt("));
        for (auto it = algo_pnl_lmt.cbegin(); it != algo_pnl_lmt.cend(); ++it) {
            if (it != algo_pnl_lmt.cbegin()) cfg_str.append(std::string(", "));
            cfg_str.append(fmt::format("{}:{}", it->first, it->second));
        }
        cfg_str.append(std::string("), "));
        cfg_str.append(fmt::format("total_pnl_lmt:{}, order_flow_lmt:{}", total_pnl_lmt, order_flow_lmt));

        return cfg_str;
    }


}