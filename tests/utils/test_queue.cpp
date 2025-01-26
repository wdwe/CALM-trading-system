#include "utils/lock_based_queue.h"
#include <thread>
#include "unordered_set"
#include <mutex>
#include <vector>
#include <stdexcept>
#include "fmt/format.h"

struct Temp {
    int v;
//    std::vector<int> arr{10000, 0};
};

std::mutex m;
std::mutex d;
int done {0};
int const n_prod{15};


void push(calm::LBQueue<Temp> &q, int beg, int end) {
    for (int i{beg}; i < end; ++i) {
        Temp t{i};
        q.wait_push(t);
        // q.wait_push(std::move(t));
    }
    {
        std::lock_guard lock{d};
        ++done;
    }
}

void get(calm::LBQueue<Temp> &q, std::unordered_set<int>& s) {
    for (;;) {
        Temp t;
        auto suc = q.try_pop(t, 1000);
        if (suc) {
            std::lock_guard lock{m};
            s.insert(t.v);
        } else if (done == n_prod){
            break;
        }

    }


}

int main() {
    calm::LBQueue<Temp> q;
    std::unordered_set<int> s;
    std::vector<std::thread> push_threads, get_threads;

    int mul{100'000};
    for (int i{0}; i < n_prod; ++i) {
        push_threads.emplace_back(push, std::ref(q), i*mul, i*mul+mul);
    }
    for (int i{0}; i < 15; ++i) {
        get_threads.emplace_back(get, std::ref(q), std::ref(s));
    }
    for (auto &t: push_threads) t.join();
    for (auto &t: get_threads) t.join();

    if (s.size() > n_prod * mul) throw std::runtime_error("additional number");
    for (int i{0}; i < n_prod * mul; ++i) {
        if (!s.contains(i)) throw std::runtime_error(fmt::format("missing number: {}", i));
    }

    return 0;
}