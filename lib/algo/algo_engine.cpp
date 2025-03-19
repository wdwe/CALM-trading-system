#include "algo_engine.h"
#include <fmt/format.h>

namespace calm {
    std::string to_string(TickMsg const& msg) {
        return fmt::format("TickMsg(symbol:{}, last_price:{}, timestamp:{})", msg.symbol, msg.last_price, msg.timestamp);
    }

    std::string to_string(PosMsg const& msg) {
        return fmt::format("PosMsg(symbol:{}, pos:{}, timestamp:{})", msg.symbol, msg.pos, msg.timestamp);
    }

}