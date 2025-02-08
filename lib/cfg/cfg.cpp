#include "cfg.h"

namespace calm {
    const Config &Config::get() {
        static Config cfg;
        return cfg;
    }

    Config::Config() {}
}