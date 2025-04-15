#ifndef CALM_TRADER_DATA_RECORDER_H
#define CALM_TRADER_DATA_RECORDER_H
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/v_noabi/bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/v_noabi/bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/exception/exception.hpp>

#include "core/event.h"
#include "objects/objects.h"
#include "spdlog/spdlog.h"
#include "utils/logging.h"
#include "utils/utils.h"

namespace calm {
    bsoncxx::v_noabi::document::value to_bson_doc(TickData const&);
    bsoncxx::v_noabi::document::value to_bson_doc(OrderReq const&);
    bsoncxx::v_noabi::document::value to_bson_doc(OrderData const&);


    class DataRecorder {
    public:
        DataRecorder(EventEngine& event_engine, std::string const& conn_str, std::string const& db_str);
        DataRecorder(DataRecorder const&) = delete;
        ~DataRecorder();
        void start();
        void stop();
    private:
        EventEngine &event_engine;
        std::shared_ptr<spdlog::async_logger> logger;

        LBQueue<Event> q;
        bool register_cb(EventType const &e_type, std::string const &cb_name, void(DataRecorder::*cb)(Event const&));
        void copy_to_queue(Event const& event);

        std::thread action_thread;
        bool running{false};

        mongocxx::instance instance;
        mongocxx::client mongo_client;
        mongocxx::database db;
        void run_action();

        void save_tick(Event const&);
        void save_order_data(Event const&);

    };

}




#endif //CALM_TRADER_DATA_RECORDER_H
