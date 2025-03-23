#ifndef CALM_TRADER_ALGO_ENGINE_H
#define CALM_TRADER_ALGO_ENGINE_H
#include <string>
#include <memory>
#include <utils/logging.h>
#include <yaml-cpp/yaml.h>
#include <memory>

#include "core/trading_engine.h"
#include "utils/lock_based_queue.h"
#include "database/redis_client.h"

namespace calm {
    struct TickMsg {
        char symbol[32];
        double last_price;
        long long timestamp;
    };

    struct PosMsg {
        char symbol[32];
        double pos;
        long long timestamp;
    };

    std::string to_string(TickMsg const &msg);
    std::string to_string(PosMsg const &msg);

    class AlgoEngine {
    public:
        explicit AlgoEngine(TradingEngine* trading_engine);
        // technically, copy and move constructors should be deleted, but then this
        // class cannot be initialised into a tuple in the Trader class. This is a design tradeoff.
        AlgoEngine(AlgoEngine const& other) = delete;
        AlgoEngine(AlgoEngine &&other) = delete;
        void start();
        void stop();

    private:
        TradingEngine* trading_engine{nullptr};
        std::shared_ptr<spdlog::async_logger> logger;
        bool running{false};
        bool register_cb(EventType const &e_type, std::string const &cb_name, void (AlgoEngine::*cb)(Event const &));

        LBQueue<Event> q;

        void event_engine_timer_cb(Event const & event);
        void tick_callback(Event const &);

        std::string symbol;
        // mq
        std::string redis_host;
        int redis_port{6379};
        std::string tick_channel;
        std::string action_channel;
        std::string sentinel_token;

        // redis
        RedisClient pub_redis;
        std::thread listen_thread;
        void run_redis_sub();
        void mq_message_cb(char const*, std::size_t len);

        // positions
        double target_pos{0};
        double curr_pos{0};

        // action callback
        std::thread action_thread;
        void run_actions();
        void timer_cb(Event const &event);
        void update_pos_cb(Event const& event);

    };

}


#endif //CALM_TRADER_ALGO_ENGINE_H
