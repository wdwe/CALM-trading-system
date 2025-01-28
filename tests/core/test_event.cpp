#include "core/event.h"
#include <string>
#include <memory>
#include <fmt/format.h>
#include <iostream>
#include "objects/objects.h"
#include "objects/enums.h"
#include <stdexcept>
#include "utils/logging.h"
#include <thread>
#include <chrono>

struct Msg {
    int id;
    std::string s;
};

class Printer {
public:
    explicit Printer(calm::EventEngine &event_engine): event_engine{event_engine} {
        event_engine.register_cb(
                calm::EventType::event_engine_test, "printer",
                [this](calm::Event const& event) {this->print(event);}
                );
        event_engine.register_cb(
                calm::EventType::event_engine_test_1, "throw_error",
                [this](calm::Event const& event) {this->throw_error(event);}
                );
    }
private:
    void print(calm::Event const& event) {
        auto data = std::static_pointer_cast<Msg>(event.data);
        std::cout << fmt::format("received message Msg(id={}, s={})", data->id, data->s) << std::endl;
    }

    void throw_error(calm::Event const& event) {
        throw std::runtime_error("an error occurred");
    }


    calm::EventEngine &event_engine;

};


int main() {
    using namespace calm;

    auto logger = init_root_logger("test_event_engine.log");
    EventEngine engine;
    Printer p{engine};
    engine.start();
    for (int i{0}; i < 5; ++i) {
        engine.send({EventType::event_engine_test, std::make_shared<Msg>(i, "message")});
    }
//    engine.send({EventType::EVENT_ENGINE_STOP});
//    engine.wait_till_finish();

    using namespace std::chrono_literals;
//    std::this_thread::sleep_for(5s);
//    engine.stop();

    engine.send({EventType::event_engine_test_1});
    std::this_thread::sleep_for(10s);
    engine.stop();


    return 0;
}