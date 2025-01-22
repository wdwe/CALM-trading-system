#ifndef TRADING_UTILS_H
#define TRADING_UTILS_H

#include <string>
#include <vector>

namespace calm {
    std::vector<std::string> split(std::string const& s, std::string const& delim);

    std::string to_string(time_t time);

    bool is_unset_double(double num);
    bool is_unset_int(int num);
    bool is_unset_llong(long long num);
}

#endif //TRADING_UTILS_H
