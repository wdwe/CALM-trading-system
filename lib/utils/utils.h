#ifndef TRADING_UTILS_H
#define TRADING_UTILS_H

#include <string>
#include <vector>

namespace calm {
    std::vector<std::string> split(std::string const& s, std::string const& delim);

    std::string to_string(time_t time);
}

#endif //TRADING_UTILS_H
