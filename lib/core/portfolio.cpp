#include "portfolio.h"
#include "objects/utils.h"
#include "utils/utils.h"

namespace calm {
    Portfolio::Portfolio(EventEngine& event_engine, MarketDataManager& mktd_mgr):
    event_engine{event_engine}, mktd_mgr{mktd_mgr} {
        logger = init_sub_logger("portfolio");
        register_cb(EventType::timer, "portfolio_timer_cb", &Portfolio::event_engine_timer_cb);
        register_cb(EventType::order_data, "portfolio_order_data_cb", &Portfolio::event_engine_order_data_cb);
    }

    Portfolio::~Portfolio() {
        stop();
    }

    void Portfolio::start() {
        if (!running) {
            running = true;
            action_thread = std::thread(&Portfolio::run_actions, this);
        }
    }

    void Portfolio::stop() {
        if (running) {
            running = false;
            if (action_thread.joinable()) {
                action_thread.join();
            }
        }
    }


    void Portfolio::add_market_data_mapping(std::string const& algo, std::string const& order_symbol, std::string const& market_symbol) {
        if (running) {
            logger->error(
                    "in add_market_data_mapping - algo:{}'s mapping ({} -> {}) cannot be added when portfolio is running",
                    algo, order_symbol, market_symbol
            );
            return;
        }
        mktd_mapping[algo][order_symbol] = market_symbol;
    }


    void Portfolio::add_to_portfolio(std::string const &algo, std::string const &symbol) {
        // initialise
        if (running) {
            logger->error(
                    "in add_to_portfolio - algo:{}'s symbol:{} cannot be added to portfolio while running",
                    algo, symbol
            );
            return;
        }
        algo_portfolio[algo][symbol];
    }

    double Portfolio::get_total_pnl() {
        return total_pnl;
    }

    double Portfolio::get_algo_pnl(std::string const &algo) {
        return algo_pnl[algo];
    }

    bool Portfolio::register_cb(EventType const &e_type, std::string const &cb_name, void (Portfolio::*cb)(Event const &)) {
        return event_engine.register_cb(e_type, cb_name, [this, cb](Event const& event){(this->*cb)(event);});
    }

    void Portfolio::event_engine_order_data_cb(Event const &event) {
        q.wait_push(event);
    }

    void Portfolio::event_engine_timer_cb(Event const &event) {
        q.wait_push(event);
    }

    void Portfolio::run_actions() {
        Event event;
        while (running) {
            q.try_pop(event, 1'000'000);
            switch (event.e_type) {
                case EventType::order_data:
                    order_data_cb(event); break;
                case EventType::timer:
                    timer_cb(event); break;
                default:
                    break;
            }
        }
    }


    void Portfolio::order_data_cb(const calm::Event &event) {
        auto data = std::static_pointer_cast<OrderData>(event.data);
        auto& algo = data->source;
        auto& symbol = data->symbol;
        auto order_id = data->order_id;
        {
            std::lock_guard lock{data_m};

            auto &algo_orders = active_orders[algo];
            auto &old_order = algo_orders[order_id];
            double change_qty = data->traded_quantity, new_px = data->avg_trade_price, change_comm = data->commission;
            double change_cost = change_qty * new_px;
            if (old_order != nullptr){
                double old_qty = old_order->traded_quantity, old_px = old_order->avg_trade_price, old_comm = old_order->commission;
                change_qty -= old_qty, change_cost -= old_qty * old_px, change_comm -= old_comm;
            }

            algo_orders[order_id] = data;

            if (data->action == Action::sell) {
                change_qty *= -1;
                change_cost *= -1;
            }
            auto& pos = algo_portfolio[algo][symbol];
            pos.pos += change_qty;
            pos.cost += change_cost;
            pos.cost += change_comm;
        }
    }

    void Portfolio::timer_cb(const calm::Event &event) {
        auto timer_data = std::static_pointer_cast<Timer>(event.data);
        {
            std::lock_guard lock{data_m};
            total_pnl = 0;

            for (auto &[algo, algo_port]: algo_portfolio) {
                auto& _pnl = algo_pnl[algo];
                _pnl = 0;
                auto& algo_orders = active_orders[algo];
                auto &algo_sym_mapping = mktd_mapping[algo];
                for (auto &[sym, pos]: algo_port) {
                    auto mkt_sym = sym;
                    if (algo_sym_mapping.contains(sym)) mkt_sym = algo_sym_mapping[sym];

                    auto tick = mktd_mgr.get_last_tick(mkt_sym);
                    if (tick == nullptr || is_unset_double(_pnl)) {
                        _pnl = UnsetDouble;
                    } else {
                        _pnl += pos.pos * tick->last_price - pos.cost;
                    }
                }
                if (is_unset_double(_pnl) || is_unset_double(total_pnl)) {
                    total_pnl = UnsetDouble;
                } else {
                    // TODO: fx_rate should be per contract to support algo that span several markets
                    total_pnl += _pnl * fx_rate(algo);
                }
            }
        }

        // push pnl once every 30 sec
        if (timer_data->sec == 30 || timer_data->sec == 0) {
            for (auto const& [algo, pnl]: algo_pnl) {
                Event algo_pnl_event{EventType::algo_pnl, std::make_shared<AlgoPnL>(algo, pnl, timer_data->timestamp)};
//                logger->info(to_string(*std::static_pointer_cast<AlgoPnL>(algo_pnl_event.data)));
                event_engine.send(std::move(algo_pnl_event));
            }
            Event total_pnl_event{EventType::total_pnl, std::make_shared<TotalPnL>(total_pnl, timer_data->timestamp)};
//            logger->info(to_string(*std::static_pointer_cast<TotalPnL>(total_pnl_event.data)));
            event_engine.send(std::move(total_pnl_event));
        }
    }

    double Portfolio::fx_rate(std::string const &algo) {
        return 1;
    }

}

