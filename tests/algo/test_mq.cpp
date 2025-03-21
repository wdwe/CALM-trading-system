#include "database/redis_client.h"
#include <string_view>
#include <iostream>
#include "algo/algo_engine.h"
#include <ctime>
#include <thread>
#include <functional>
#include <chrono>

void print(char const* msg, std::size_t len) {
    std::cout << std::string_view(msg, len) << std::endl;
}

void print_pos_msg(char const*msg, std::size_t len) {
    auto pos_data = reinterpret_cast<calm::PosMsg const*>(msg);
    std::cout << calm::to_string(*pos_data) << std::endl;
}

void subscribe(std::function<void(char const*msg, std::size_t len)> cb, std::string const& sentinel) {
    calm::RedisClient client("localhost", 6380);
    client.connect();
    client.subscribe("action_channel", cb, sentinel);
}

int main() {
    using namespace std::chrono_literals;
    calm::RedisClient client("localhost", 6380);
    client.connect();
    std::string sentinel = "sentinel";
    std::thread sub_thread(subscribe, print_pos_msg, sentinel);

    for (int i{0}; i < 100; i++) {
        auto px = rand() % 400;
        time_t timestamp;
        time(&timestamp);
        auto tick = calm::TickMsg("abcdefg", px, timestamp);
        client.publish("tick_channel", reinterpret_cast<char const*>(&tick), sizeof(calm::TickMsg));
        std::this_thread::sleep_for(500ms);
    }
    client.publish("tick_channel", sentinel.c_str(), sentinel.size() + 1);
    client.publish("action_channel", sentinel.c_str(), sentinel.size() + 1);

    sub_thread.join();
    return 0;
}

