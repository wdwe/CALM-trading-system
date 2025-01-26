#ifndef TRADING_EVENT_H
#define TRADING_EVENT_H

#include <memory>
#include <functional>

#include "spdlog/async_logger.h"

#include "objects/enums.h"
#include "utils/lock_based_queue.h"

namespace calm {

    struct Event {
        EventType e_type;
        std::shared_ptr<void> data;
    };


    class EventEngine{
    public:
        EventEngine();
        ~EventEngine();
        void start();
        void stop();
        bool register_cb(EventType const& e_type, std::string const& cb_name, std::function<void(Event const&)> cb);
//        void unregister_cb(EventType const& e_type, std::string const& cb_name);
        void send(Event&& event);
        void send(Event const & event);
        void wait_till_finish();

        std::shared_ptr<spdlog::async_logger> logger;
    private:
        bool running{false};
        std::mutex m;
        LBQueue<Event> q;
        std::unordered_map<EventType, std::unordered_map<std::string, std::function<void(Event)>>> m_cbs;
        std::thread cb_thread;

        void run();
    };


}
#endif //TRADING_EVENT_H
