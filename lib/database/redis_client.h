#ifndef CALM_TRADER_REDIS_CLIENT_H
#define CALM_TRADER_REDIS_CLIENT_H
#include <string>
#include <hiredis/hiredis.h>
#include <functional>
#include <thread>

namespace calm {
    class RedisClient {
    typedef std::function<void(char const*, std::size_t len)> Callback;
    public:
        explicit RedisClient(std::string const& host="localhost", int port=6379);
        ~RedisClient();
        void publish(std::string const& channel, char const* data, std::size_t sz);
        void subscribe(std::string const& channel, Callback callback, std::string sentinel = "sentinel");

    private:
        redisReply *reply{nullptr};
        redisContext *context{nullptr};

        void free_reply();
        void free_context();

        void check_status();
        static bool is_sentinel(std::string const& sentinel, char const* msg, std::size_t len);

    };

}
#endif //CALM_TRADER_REDIS_CLIENT_H
