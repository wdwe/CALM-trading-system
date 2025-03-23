#include <fmt/format.h>

#include "algo_engine.h"
#include "cfg/cfg.h"

namespace calm {
    std::string to_string(TickMsg const& msg) {
        return fmt::format("TickMsg(symbol:{}, last_price:{}, timestamp:{})", msg.symbol, msg.last_price, msg.timestamp);
    }

    std::string to_string(PosMsg const& msg) {
        return fmt::format("PosMsg(symbol:{}, pos:{}, timestamp:{})", msg.symbol, msg.pos, msg.timestamp);
    }

    AlgoEngine::AlgoEngine(TradingEngine *trading_engine): trading_engine(trading_engine) {
        logger = init_sub_logger("algo_engine");
        auto const& global_cfg = Config::get();
        auto const& algo_cfg_path = global_cfg.algo_config_path;
        auto cfg = YAML::LoadFile(algo_cfg_path);
        symbol = cfg["symbol"].as<std::string>();

        auto redis_node = cfg["redis_cfg"];
        redis_host = redis_node["host"].as<std::string>();
        redis_port = redis_node["port"].as<int>();
        pub_redis = RedisClient(redis_host, redis_port);

        auto mq_node = cfg["mq"];
        tick_channel = mq_node["channels"]["tick"].as<std::string>();
        action_channel = mq_node["channels"]["action"].as<std::string>();
        sentinel_token = mq_node["sentinel_token"].as<std::string>();

        register_cb(EventType::tick_data, "algo_engine_tick", &AlgoEngine::tick_callback);
        register_cb(EventType::timer, "algo_engine_timer", &AlgoEngine::event_engine_timer_cb);
    }


    void AlgoEngine::start() {
        running = true;
        logger->info("Starting engine...");
        pub_redis.connect();
        trading_engine->subscribe(symbol);
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
        auto timer = std::static_pointer_cast<Timer>(event.data);
        if (timer->sec == 0 || timer->sec == 30) {
            q.wait_push(event);
        }
    }

    void AlgoEngine::tick_callback(Event const & event) {
        if (!running) return;
        auto tick = std::static_pointer_cast<TickData>(event.data);
//        logger->info(to_string(*tick));
    }

    void AlgoEngine::run_redis_sub() {
        RedisClient sub_redis(redis_host, redis_port);
        sub_redis.connect();
        sub_redis.subscribe(action_channel, [this](char const *data, std::size_t len){ mq_message_cb(data, len);});
    }


    void AlgoEngine::mq_message_cb(char const *data, std::size_t len) {
        auto pos_data = reinterpret_cast<PosMsg const*>(data);
        Event event(EventType::algo_pos_update, std::make_shared<PosMsg>(*pos_data));
        q.wait_push(std::move(event));
    }

    void AlgoEngine::run_actions() {
        Event event;
        while (running) {
            if (q.try_pop(event, 1'000'000)) {
                if (event.e_type == EventType::timer) timer_cb(event);
                else if (event.e_type == EventType::algo_pos_update) update_pos_cb(event);
            }
        }

    }

    void AlgoEngine::timer_cb(Event const &event) {
        auto tick = trading_engine->get_last_tick(symbol);
        TickMsg tick_msg("", tick->last_price, tick->timestamp);
        logger->info(to_string(*tick));
        std::strncpy(tick_msg.symbol, tick->symbol.c_str(), std::size(tick_msg.symbol));
        char* data_bytes = reinterpret_cast<char*>(&tick_msg);
        pub_redis.publish(tick_channel, data_bytes, sizeof(TickMsg));
    }

    void AlgoEngine::update_pos_cb(Event const &event) {
        auto pos = std::static_pointer_cast<PosMsg>(event.data);
        if (target_pos != curr_pos) {
            logger->warn("in update_pos_cb - curr_pos:{} does not equal to old target_pos:{}", curr_pos, target_pos);
        }
        target_pos = pos->pos;
        logger->info(to_string(*pos));
        auto diff = target_pos - curr_pos;
        // TODO: round diff to tick size here
        if (diff >= 1 or diff <= -1) {
            OrderReq order_req;
            order_req.symbol = symbol;
            order_req.action = diff < 0? Action::sell: Action::buy;
            order_req.quantity = std::abs(diff);
            order_req.order_type = OrderType::market;
            logger->info("Sending order:{}", to_string(order_req));
        }

    }


}