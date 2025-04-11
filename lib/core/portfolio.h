#ifndef CALM_TRADER_PORTFOLIO_H
#define CALM_TRADER_PORTFOLIO_H

#include <unordered_map>
#include <string>
#include <mutex>
#include <thread>
#include <utils/logging.h>
#include "utils/lock_based_queue.h"
#include "event.h"
#include "market_data.h"

namespace calm {
    struct TradeRec {
        double qty{0};
        double px{0};
        double commission{0};
    };

    struct ContractPos {
        std::string symbol;
        double pos;
        double cost;
    };

    class Portfolio {
    public:
        Portfolio(EventEngine& EventEngine, MarketDataManager& mktd_mgr);
        Portfolio(Portfolio const&) = delete;
        ~Portfolio();
        void start();
        void stop();
        void add_market_data_mapping(std::string const& algo, std::string const& order_symbol, std::string const& market_symbol);
        void add_to_portfolio(std::string const& algo, std::string const& symbol);
        double get_total_pnl();
        double get_algo_pnl(std::string const& algo);
    private:
        typedef std::unordered_map<std::string, ContractPos> ContractData;
        typedef std::unordered_map<std::string, ContractData> AlgoData;

        EventEngine& event_engine;
        MarketDataManager& mktd_mgr;
        std::shared_ptr<spdlog::async_logger> logger;
        bool running{false};
        LBQueue<Event> q;
        std::thread action_thread;

        void run_actions();
        bool register_cb(EventType const &e_type, std::string const &cb_name, void(Portfolio::*cb)(Event const&));

        void event_engine_order_data_cb(Event const & event);
        void event_engine_timer_cb(Event const& event);


        std::mutex data_m;
        AlgoData algo_portfolio;
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> mktd_mapping;
        std::unordered_map<std::string, double> algo_pnl;
        std::unordered_map<std::string, std::unordered_map<OrderId, std::shared_ptr<OrderData>>> active_orders;
        double total_pnl{0};

        double fx_rate(std::string const &algo);
        void order_data_cb(Event const& event);
        void timer_cb(Event const& event);

    };
}

#endif //CALM_TRADER_PORTFOLIO_H
