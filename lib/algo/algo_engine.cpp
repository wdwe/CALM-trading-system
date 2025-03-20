#include "algo_engine.h"
#include <fmt/format.h>

namespace calm {
    std::string to_string(TickMsg const& msg) {
        return fmt::format("TickMsg(symbol:{}, last_price:{}, timestamp:{})", msg.symbol, msg.last_price, msg.timestamp);
    }

    std::string to_string(PosMsg const& msg) {
        return fmt::format("PosMsg(symbol:{}, pos:{}, timestamp:{})", msg.symbol, msg.pos, msg.timestamp);
    }

    AlgoEngine::AlgoEngine(TradingEngine *trading_engine): trading_engine(trading_engine) {}


    void AlgoEngine::init() {
        logger = init_sub_logger("algo_engine");
        register_cb(EventType::tick_data, "algo_engine_tick", &AlgoEngine::tick_callback);
    }

    void AlgoEngine::start() {
        running = true;
        logger->info("Starting engine...");
        trading_engine->subscribe("ETH-CRYPTO-USD.PAXOS");
        using namespace std::chrono_literals;
    }

    void AlgoEngine::stop() {
        if (!running) return;
        logger->info("Stop engine...");
    }

    bool AlgoEngine::register_cb(EventType const &e_type, std::string const &cb_name, void(AlgoEngine::*cb)(Event const&)) {
        return trading_engine->register_cb(e_type, cb_name, [this, cb](Event const& event){(this->*cb)(event);});
    }

    void AlgoEngine::tick_callback(Event const & event) {
        if (!running) return;
        auto tick = std::static_pointer_cast<TickData>(event.data);
        logger->info(to_string(*tick));
    }


}