#ifndef CALM_TRADER_LOCK_BASED_QUEUE_H
#define CALM_TRADER_LOCK_BASED_QUEUE_H

#include <queue>
#include <mutex>
#include <chrono>
#include <condition_variable>

namespace calm {
    template<typename T>
    class LBQueue {
    public:
        void wait_push(T const & item);
        void wait_push(T&& item);
        T wait_pop();
        bool try_pop(T& item, unsigned int us);

    private:
        std::mutex m;
        std::condition_variable cv;
        std::queue<T> q;
    };

    template<typename T>
    void LBQueue<T>::wait_push(T &&item) {
        {
            std::lock_guard lock{m};
            q.push(std::move(item));
            cv.notify_one();
        }
    }

    template<typename T>
    void LBQueue<T>::wait_push(T const & item) {
        {
            std::lock_guard lock{m};
            q.push(item);
            cv.notify_one();
        }
    }


    template<typename T>
    T LBQueue<T>::wait_pop() {
        std::unique_lock lock{m};
        cv.wait(lock, [this](){return !this->q.empty();});
        T item = std::move(q.front());
        q.pop();
        return item;
    }

    template<typename T>
    bool LBQueue<T>::try_pop(T &item, unsigned int us) {
        std::unique_lock lock{m};
        cv.wait_for(lock, std::chrono::microseconds(us), [this](){return !this->q.empty();});
        if (q.empty()) return false;

        item = std::move(q.front());
        q.pop();
        return true;
    }
}




#endif //CALM_TRADER_LOCK_BASED_QUEUE_H
