#ifndef CALM_TRADER_ALGO_ENGINE_H
#define CALM_TRADER_ALGO_ENGINE_H
#include <string>

namespace calm {
    struct TickMsg {
        char symbol[32];
        double last_price;
        long long timestamp;
    };

    struct PosMsg {
        char symbol[32];
        long long pos;
        long long timestamp;
    };

    std::string to_string(TickMsg const &msg);
    std::string to_string(PosMsg const &msg);
}


#endif //CALM_TRADER_ALGO_ENGINE_H
