#include "data_recorder.h"

namespace calm {
    bsoncxx::v_noabi::document::value to_bson_doc(TickData const& t) {
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;
        return make_document(
                kvp("symbol", t.symbol),
                kvp("last_price", t.last_price),
                kvp("last_size", t.last_size),
                kvp("bid_price", t.bid_price),
                kvp("bid_size", t.bid_size),
                kvp("ask_price", t.ask_size),
                kvp("timestamp", t.timestamp)
        );

    }

    bsoncxx::v_noabi::document::value to_bson_doc(OrderReq const& r) {
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;
        return make_document(
                kvp("symbol", r.symbol),
                kvp("exchange", r.exchange),
                kvp("action", action_to_ib.at(r.action)),
                kvp("order_type", order_type_to_ib.at(r.order_type)),
                kvp("quantity", r.quantity),
                kvp("price", r.price),
                kvp("order_id", r.order_id)
        );
    }

    bsoncxx::v_noabi::document::value to_bson_doc(OrderData const& d) {
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;
        return make_document(
                kvp("order_id", d.order_id),
                kvp("symbol", d.symbol),
                kvp("exchange", d.exchange),
                kvp("source", d.source),
                kvp("order_type", order_type_to_ib.at(d.order_type)),
                kvp("action", action_to_ib.at(d.action)),
                kvp("total_quantity", d.total_quantity),
                kvp("traded_quantity", d.traded_quantity),
                kvp("avg_trade_price", d.avg_trade_price),
                kvp("commission", d.commission),
                kvp("status", order_status_to_ib.at(d.status)),
                kvp("error_code", d.error_code)
        );
    }


    DataRecorder::DataRecorder(calm::EventEngine &event_engine, std::string const& conn_str, std::string const& db_str):
    event_engine{event_engine}, mongo_client(mongocxx::uri{conn_str}), db{mongo_client[db_str]} {
        logger = init_sub_logger("data_recorder");
        logger->info("initialising DataRecorder");
        std::vector<EventType> event_types{EventType::tick_data, EventType::order_data};
        for (auto e_type: event_types) {
            register_cb(e_type, "data_recorder_copy", &DataRecorder::copy_to_queue);
        }
    }

    DataRecorder::~DataRecorder() {
        stop();
    }

    void DataRecorder::start() {
        if (running) return;
        running = true;
        action_thread = std::thread(&DataRecorder::run_action, this);
    }

    void DataRecorder::stop() {
        if (!running) return;
        running = false;
        if (action_thread.joinable()) {
            action_thread.join();
        }
    }
    bool DataRecorder::register_cb(EventType const &e_type, std::string const &cb_name, void(DataRecorder::*cb)(Event const&)) {
        return event_engine.register_cb(e_type, cb_name, [this, cb](Event const& event){(this->*cb)(event);});
    }

    void DataRecorder::copy_to_queue(Event const& event) {
        q.wait_push(event);
    }

    void DataRecorder::run_action() {
        Event event;
        while (running) {
            q.try_pop(event, 1'000'000);
            try {
                switch (event.e_type) {
                    case EventType::order_data:
                        save_order_data(event);
                        break;
                    case EventType::tick_data:
                        save_tick(event);
                        break;
                    default:
                        ;
                }
            } catch (mongocxx::exception const& e) {
                logger->error("in save_tick - mongo error: {}", e.what());
            }

        }
    }

    void DataRecorder::save_tick(Event const &event) {
        static auto coll = db["tick"];
        auto data = std::static_pointer_cast<TickData>(event.data);
        coll.insert_one(to_bson_doc(*data));
    }

    void DataRecorder::save_order_data(Event const &event) {
        static auto coll = db["order_data"];
        auto data = std::static_pointer_cast<OrderData>(event.data);
        coll.insert_one(to_bson_doc(*data));
    }


}