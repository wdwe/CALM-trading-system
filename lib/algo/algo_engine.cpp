#include <fmt/format.h>

#include "algo_engine.h"
#include "cfg/cfg.h"
#include "objects/utils.h"

namespace calm {
    std::string to_string(TickMsg const& msg) {
        return fmt::format("TickMsg(symbol:{}, last_price:{}, timestamp:{})", msg.symbol, msg.last_price, msg.timestamp);
    }

    std::string to_string(PosMsg const& msg) {
        return fmt::format("PosMsg(symbol:{}, pos:{}, timestamp:{})", msg.symbol, msg.pos, msg.timestamp);
    }

    std::string to_string(PosUpdate const& msg) {
        return fmt::format("PosUpdate(symbol:{}, pos:{}, timestamp:{}, retry:{})", msg.symbol, msg.pos, msg.timestamp, msg.retry);
    }

    AlgoEngine::AlgoEngine(TradingEngine *trading_engine): trading_engine(trading_engine) {
        logger = init_sub_logger("algo_engine");
        auto const& global_cfg = Config::get();
        auto const& algo_cfg_path = global_cfg.algo_config_path;
        auto cfg = YAML::LoadFile(algo_cfg_path);
        mkt_data_symbol = cfg["market_data_symbol"].as<std::string>();
        order_symbol = cfg["order_symbol"].as<std::string>();

        auto redis_node = cfg["redis_cfg"];
        redis_host = redis_node["host"].as<std::string>();
        redis_port = redis_node["port"].as<int>();
        pub_redis = RedisClient(redis_host, redis_port);

        auto mq_node = cfg["mq"];
        tick_channel = mq_node["channels"]["tick"].as<std::string>();
        action_channel = mq_node["channels"]["action"].as<std::string>();
        sentinel_token = mq_node["sentinel_token"].as<std::string>();

        register_cb(EventType::tick_data, "algo_engine_tick", &AlgoEngine::event_engine_tick_cb);
        register_cb(EventType::timer, "algo_engine_timer", &AlgoEngine::event_engine_timer_cb);
        register_cb(EventType::order_data, "algo_engine_order_data", &AlgoEngine::event_engine_order_cb);
    }


    void AlgoEngine::start() {
        running = true;
        logger->info("Starting engine...");
        pub_redis.connect();
        trading_engine->subscribe(mkt_data_symbol);
        listen_thread = std::thread(&AlgoEngine::run_redis_sub, this);
        action_thread = std::thread(&AlgoEngine::run_actions, this);
    }

    void AlgoEngine::stop() {
        if (!running) return;
        logger->info("Stop engine...");
        if (listen_thread.joinable()) listen_thread.join();
        if (action_thread.joinable()) action_thread.join();
        pub_redis.disconnect();
    }

    bool AlgoEngine::register_cb(EventType const &e_type, std::string const &cb_name, void(AlgoEngine::*cb)(Event const&)) {
        return trading_engine->register_cb(e_type, cb_name, [this, cb](Event const& event){(this->*cb)(event);});
    }

    void AlgoEngine::event_engine_timer_cb(Event const & event) {
        if (!running) return;
        auto timer = std::static_pointer_cast<Timer>(event.data);
        if (timer->sec == 0 || timer->sec == 30) {
            q.wait_push(event);
        }
    }


    void AlgoEngine::run_redis_sub() {
        RedisClient sub_redis(redis_host, redis_port);
        sub_redis.connect();
        sub_redis.subscribe(action_channel, [this](char const *data, std::size_t len){ mq_message_cb(data, len);});
    }


    void AlgoEngine::mq_message_cb(char const *data, std::size_t len) {
        auto pos_data = reinterpret_cast<PosMsg const*>(data);
        Event event(EventType::algo_pos_update,
                    std::make_shared<PosUpdate>(pos_data->symbol, pos_data->pos, pos_data->timestamp));
        q.wait_push(std::move(event));
    }

    void AlgoEngine::run_actions() {
        Event event;
        while (running) {
            if (q.try_pop(event, 1'000'000)) {
                switch (event.e_type) {
                    case EventType::tick_data:
                        tick_cb(event);
                        break;
                    case EventType::timer:
                        timer_cb(event);
                        break;
                    case EventType::algo_pos_update:
                        update_pos_cb(event);
                        break;
                    case EventType::order_data:
                        order_cb(event);
                        break;
                    default:
                        break;
                }
            }
        }

    }

    void AlgoEngine::timer_cb(Event const &event) {
        auto tick = trading_engine->get_last_tick(mkt_data_symbol);
        TickMsg tick_msg("", tick->last_price, tick->timestamp);
        std::strncpy(tick_msg.symbol, tick->symbol.c_str(), std::size(tick_msg.symbol));
        logger->info("sending tick to strategy {}", to_string(*tick));
        char* data_bytes = reinterpret_cast<char*>(&tick_msg);
        pub_redis.publish(tick_channel, data_bytes, sizeof(TickMsg));
    }

    void AlgoEngine::update_pos_cb(Event const &event) {
        auto pos = std::static_pointer_cast<PosUpdate>(event.data);
        {
            std::lock_guard lock{pos_order_m};
            if (inflight_order_id > 0) {
                if (!order_cancelled) { // this order has not been cancelled
                    logger->warn("in update_pos_cb - unfilled {}", to_string(order_reqs.at(inflight_order_id)));
                    trading_engine->cancel_order(inflight_order_id);
                    order_cancelled = true;
                } else if (!pos->retry) { // a new pos req arrived
                    logger->error("in update_pos_cb - order filling latency is too high");
                    throw std::runtime_error("in AlgoEngine::update_pos_cb - order filling latency is too high");
                }
                pos->retry = true;
                q.wait_push(event); // retry event later
                return;
            }
        }

        // no inflight order at this point
        if (target_pos != curr_pos) {
            logger->warn("in update_pos_cb - curr_pos:{} does not equal to old target_pos:{}", curr_pos, target_pos);
        }
        target_pos = pos->pos;
        logger->info(to_string(*pos));
        auto diff = target_pos - curr_pos;
        // TODO: round diff to min lot size here
        if (diff >= 1 or diff <= -1) {
            send_order(order_symbol, diff, OrderType::market);
        }

    }

    void AlgoEngine::send_order(std::string const &symbol, double quantity, OrderType order_type) {
        OrderReq order_req;
        order_req.symbol = symbol;
        order_req.action = quantity < 0? Action::sell: Action::buy;
        order_req.source = name;
        order_req.quantity = std::abs(quantity);
        order_req.order_type = OrderType::market;
        logger->info("Sending order:{}", to_string(order_req));
        OrderId order_id = trading_engine->send_order(order_req);
        {
            std::lock_guard lock{pos_order_m};
            inflight_order_id = order_id;
            order_status[order_id] = OrderStatus::pending_submit;
            order_data[order_id] = nullptr;
            order_reqs[order_id] = order_req;
            order_target = quantity;
            order_filled = 0;
        }
    }

    void AlgoEngine::event_engine_order_cb(Event const &event) {
        if (!running) return;
        q.wait_push(event);
    }

    void AlgoEngine::order_cb(Event const &event) {
        auto order_d = std::static_pointer_cast<OrderData>(event.data);
        OrderId order_id = order_d->order_id;
        {
            std::lock_guard lock{pos_order_m};
            // this order filtering cannot be in event_engine_order_cb, send_order and event_engine
            // are in two threads, so there is a possible race condition
            if (!order_status.contains(order_id)) return;
        }
        logger->info("in order_cb - {}", to_string(*order_d));
        auto status = order_d->status;
        auto traded = order_d->traded_quantity;
        int error_code = order_d->error_code;
        if (error_code) {
            logger->error("in order_cb - error:{} for {}", error_code, to_string(*order_d));
        }
        bool active = is_active_order(status) && !error_code;
        bool is_buy = order_d->action == Action::buy;
        {
            std::lock_guard lock{pos_order_m};
            auto diff = traded - order_filled;
            if (is_buy) curr_pos += diff;
            else curr_pos -= diff;

            order_filled = traded;
            order_status[order_id] = status;
            order_data[order_id] = order_d;
            if (!active) {
                inflight_order_id = -1;
                order_cancelled = false;
            }
        }
    }

    void AlgoEngine::event_engine_tick_cb(Event const & event) {
        if (!running) return;
        auto data = std::static_pointer_cast<TickData>(event.data);
        if (data->symbol == mkt_data_symbol) q.wait_push(event);
    }

    void AlgoEngine::tick_cb(Event const &event) {
        if (inflight_order_id < 0) return;
        {
            std::lock_guard lock{pos_order_m};
            auto &inflight_order_req = order_reqs[inflight_order_id];
            if (inflight_order_req.order_type == OrderType::market) return;
            // TODO: monitoring/changing limit order or other types of orders here
            return;
        }

    }


}