#ifndef CALM_TRADER_TRADER_H
#define CALM_TRADER_TRADER_H
#include <chrono>
#include "trading_engine.h"
#include "algo/algo_engine.h"


namespace calm {
    template<typename ...Algos>
    class Trader {
    public:
        Trader();
        Trader(Trader const&) = delete;
        Trader(Trader &&) = delete;
        void run();
    private:
        TradingEngine trading_engine;
        std::tuple<Algos...> algos{Algos{&trading_engine}...};

        template<std::size_t I = 0>
        void start_algos();

        void send_timer();
    };

    template<typename ...Algos>
    Trader<Algos...>::Trader() {
        // init algos
        std::apply([](Algos&... algos_) {(algos_.init(),...);}, algos);
    }


    template<typename ...Algos>
    template<std::size_t I>
    void Trader<Algos...>::start_algos() {
        // or just use std::apply
        if constexpr (I < sizeof...(Algos)) {
            std::get<I>(algos).start();
            start_algos<I + 1>();
        }
    }


    template<typename ...Algos>
    void Trader<Algos...>::run() {
        trading_engine.start();
        start_algos();

        while (true) {
            send_timer();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            // algo's (and other modules') stopping conditions (risk too high, market closed...) can be checked here
        }

        trading_engine.stop();
    }

        template<typename ...Algos>
        void Trader<Algos...>::send_timer() {
            using namespace std::chrono;
            using namespace std::chrono_literals;
            static auto target = floor<seconds>(system_clock::now()) + 1s;
            auto t = floor<seconds>(system_clock::now());

            if (t >= target) {
                target += 1s;
                trading_engine.send_event(Event(EventType::timer, std::make_shared<Timer>(t.time_since_epoch().count() % 60)));
            }

        }


}



#endif //CALM_TRADER_TRADER_H
