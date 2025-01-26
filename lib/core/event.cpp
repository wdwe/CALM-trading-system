#include "event.h"

#include <utility>

#include "utils/logging.h"
#include "spdlog/spdlog.h"

namespace calm {
    EventEngine::EventEngine() {
        logger = init_sub_logger("event_engine");
    }

    EventEngine::~EventEngine() {
        stop();

    }

    void EventEngine::start() {
        {
            std::lock_guard lock{m};
            if (!running) running = true;
        }
        cb_thread = std::thread(&EventEngine::run, this);
    }

    void EventEngine::stop() {
        {
            std::lock_guard lock{m};
            if (running) running = false;
            if (cb_thread.joinable()) cb_thread.join();
        }
    }


    bool EventEngine::register_cb(EventType const &e_type, std::string const &cb_name, std::function<void(Event const&)> cb) {
        {
            std::lock_guard lock{m};
            if (running) {
                logger->error("In register_cb - callback:{} for e_type:{} cannot be registered when the engine in running",
                              cb_name, static_cast<int>(e_type));
                return false;
            }
            if (m_cbs[e_type].contains(cb_name)) {
                logger->warn("In register_cb - replacing callback:{} for e_type:{}", cb_name, static_cast<int>(e_type));
            } else {
                logger->info("In register_cb - registering callback:{} for e_type:{}", cb_name, static_cast<int>(e_type));
            }
            m_cbs[e_type][cb_name] = std::move(cb);
            return true;
        }
    }

    void EventEngine::run() {
        while (running) {
            Event event;
            if(q.try_pop(event, 1'000'000)) {
                if (event.e_type == EventType::EVENT_ENGINE_STOP) {
                    logger->info("In run - received EVENT_ENGINE_STOP, stopping event engine.");
                    {
                        std::lock_guard lock{m};
                        running = false;
                    }
                } else if (m_cbs.contains(event.e_type)) {
                    for (auto &[cb_name, cb]: m_cbs[event.e_type]) {
                        try {
                            cb(event);
                        } catch (std::exception const& e) {
                            logger->error("In run - exception caught for event:{} callback:{}, error:{}", static_cast<int>(event.e_type), cb_name, e.what());
                            // TODO clear positions and shut down system here
                        } catch (...) {
                            logger->error("In run - unknown exception caught event:{} callback:{}", static_cast<int>(event.e_type), cb_name);
                            // TODO clear positions and shut down system here
                        }
                    }
                }
            }
        }
    }

    void EventEngine::wait_till_finish() {
        if (cb_thread.joinable()) cb_thread.join();
    }

    void EventEngine::send(Event &&event) {
        q.wait_push(std::move(event));
    }

    void EventEngine::send(Event const &event) {
        q.wait_push(event);
    }


}