//
// Created by wayne on 18/1/25.
//

#include "logging.h"

#include <spdlog/spdlog.h>
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace calm {
    std::shared_ptr<spdlog::async_logger> init_root_logger(std::string const &logfile_path,
                                                           spdlog::level::level_enum const& level) {
        spdlog::init_thread_pool(8192, 1);
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
        sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logfile_path));
        auto root_logger = std::make_shared<spdlog::async_logger>("root_logger", sinks.cbegin(), sinks.cend(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        root_logger->set_level(level);
        spdlog::register_logger(root_logger);
        return root_logger;
    }

    std::shared_ptr<spdlog::async_logger> init_sub_logger(std::string const &name) {
        auto root_logger = spdlog::get("root_logger");
        auto sinks = root_logger->sinks();
        auto logger = std::make_shared<spdlog::async_logger>(name, sinks.cbegin(), sinks.cend(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        logger->set_level(root_logger->level());
        return logger;
    }
}