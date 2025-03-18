#include "redis_client.h"
#include <stdexcept>
#include <fmt/format.h>

namespace calm {
    RedisClient::RedisClient(std::string const &host, int port) {
        context = redisConnect(host.c_str(), port);
        if (context == nullptr || context->err) {
            if (context) {
                throw std::runtime_error(fmt::format("Error: {}", context->errstr));
            }
            throw std::runtime_error("Can't allocate redis context");
        }
    }


    RedisClient::~RedisClient() {
        if (reply != nullptr) free_reply();
        if (context != nullptr) free_context();
    }

    void RedisClient::free_context() {
        redisFree(context);
        context = nullptr;
    }

    void RedisClient::free_reply() {
        freeReplyObject(reply);
        reply = nullptr;
    }

    void RedisClient::publish(std::string const &channel, char const *data, std::size_t sz) {
        reply = static_cast<redisReply*>(redisCommand(context, "publish %s %b", channel.c_str(), data, sz));
        check_status();
    }

    void RedisClient::subscribe(std::string const &channel, RedisClient::Callback callback, std::string const sentinel) {
        reply = static_cast<redisReply*>(redisCommand(context, "SUBSCRIBE %s", channel.c_str()));
        check_status();
        free_reply();
        while(true) {
            if (redisGetReply(context, reinterpret_cast<void**>(&reply)) != REDIS_OK) {
                throw std::runtime_error(fmt::format("error occurred subscribe {}", context->errstr));
            }
            auto r = reply->element[2];
            if (is_sentinel(sentinel, r->str, r->len)) break;
            callback(r->str, r->len);
            free_reply();
        }
    }

    void RedisClient::check_status() {
        if (reply == nullptr) {
            throw std::runtime_error(fmt::format("error occurred {}", context->errstr));
        }
    }

    bool RedisClient::is_sentinel(std::string const &sentinel, char const *msg, std::size_t len) {
        if (len != sentinel.size() + 1) return false;
        for (std::size_t i{0}; i < sentinel.size(); ++i) {
            if (sentinel[i] != msg[i]) return false;
        }
        return true;
    }

}