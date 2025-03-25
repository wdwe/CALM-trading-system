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

    struct PosUpdate {
        std::string symbol;
        double pos;
        long long timestamp;
        bool retry = false;
    };

    std::string to_string(TickMsg const &msg);
    std::string to_string(PosMsg const &msg);
    std::string to_string(PosUpdate const &msg);

    class AlgoEngine {
    public:
        explicit AlgoEngine(TradingEngine* trading_engine);
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


        std::string mkt_data_symbol;
        std::string order_symbol;
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

        // action callback
        std::thread action_thread;
        void run_actions();

        // positions
        std::mutex pos_order_m;
        double target_pos{0};
        double curr_pos{0};
        void update_pos_cb(Event const& event);

        // timer
        void event_engine_timer_cb(Event const & event);
        void timer_cb(Event const &event);

        // orders
        std::unordered_map<OrderId, OrderStatus> order_status;
        std::unordered_map<OrderId, std::shared_ptr<OrderData>> order_data;
        std::unordered_map<OrderId, OrderReq> order_reqs;
        OrderId inflight_order_id = -1;
        bool order_cancelled = false;
        double order_target = 0;
        double order_filled = 0;
        void send_order(std::string const& symbol, double quantity, OrderType order_type);
        void event_engine_order_cb(Event const & event);
        void order_cb(Event const& event);

        // ticks
        void event_engine_tick_cb(Event const &);
        void tick_cb(Event const&);

    };

}


#endif //CALM_TRADER_ALGO_ENGINE_H
