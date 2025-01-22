#include "utils.h"

#include <chrono>
#include <limits.h>
#include <float.h>

#include "objects/constants.h"

#define UNSET_DOUBLE DBL_MAX
#define UNSET_INTEGER INT_MAX
#define UNSET_LONG LLONG_MAX

namespace calm {
    std::vector<std::string> split(std::string const & s, std::string const & delim) {
        std::vector<std::string> res;
        std::size_t last{0}, next{0};
        while ((next = s.find(delim, last)) != std::string::npos) {
            res.emplace_back(s.substr(last, next - last));
            last = next + 1;
        }
        res.emplace_back(s.substr(last));
        return res;
    }


    std::string to_string(time_t time) {
        char time_str[std::size("yyyy-mm-ddThh:mm:ssZ")];
        std::tm * ptm = std::gmtime(&time);
        std::strftime(time_str, std::size(time_str), "%FT%TZ", ptm);
        return time_str;
    }


    bool is_unset_double(double num) {
        return num == UnsetDouble || num == DBL_MAX;
    }
    bool is_unset_int(int num) {
        return num == UnsetInt || num == INT_MAX;
    }
    bool is_unset_llong(long long num) {
        return num == UnsetLong || num == LLONG_MAX;
    }
}
