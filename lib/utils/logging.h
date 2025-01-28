//
// Created by wayne on 18/1/25.
//

#ifndef TRADING_LOGGING_H
#define TRADING_LOGGING_H

#include <iostream>
#include "spdlog/async.h"
#include <memory>

namespace calm {
    std::shared_ptr<spdlog::async_logger> init_root_logger(std::string const &logfile_path,
                                                           spdlog::level::level_enum const& level=spdlog::level::info);

    std::shared_ptr<spdlog::async_logger> init_sub_logger(std::string const &name);

}



#endif //TRADING_LOGGING_H
