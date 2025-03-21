#include "database/redis_client.h"
#include <string_view>
#include <iostream>
void print(char const* msg, std::size_t len) {
    std::cout << std::string_view(msg, len) << std::endl;
}


int main() {
    calm::RedisClient client("localhost", 6380);
    client.connect();
    client.subscribe("channel_1", print, "end");
//    client.publish("channel_1", "abcde", 5);
    return 0;
}

