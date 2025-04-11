#ifndef CALM_TRADER_TRADER_H
#define CALM_TRADER_TRADER_H
#include <chrono>
#include <memory>
#include "trading_engine.h"
#include "portfolio.h"
#include "risk.h"
#include "cfg/cfg.h"


namespace calm {
    /*
     * Declaration
     */
    template<typename ...Algos>
    class Trader {
    public:
        Trader();
        Trader(Trader const&) = delete;
        Trader(Trader &&) = delete;
        void run();
    private:
        EventEngine event_engine;
        IBGateway gateway{event_engine};
        MarketDataManager mktd_mgr{gateway};
        Portfolio portfolio{event_engine, mktd_mgr};
        RiskManager risk_manager{event_engine, portfolio};
        TradingEngine trading_engine{event_engine, gateway, mktd_mgr, risk_manager};
        std::tuple<std::shared_ptr<Algos>...> algos{std::make_shared<Algos>(trading_engine, portfolio)...};

        template<std::size_t I = 0>
        void start_algos();

        void send_timer();
    };

    /*
     * Definition
     */
    template<typename ...Algos>
    Trader<Algos...>::Trader() = default;


    template<typename ...Algos>
    template<std::size_t I>
    void Trader<Algos...>::start_algos() {
        // or just use std::apply
        if constexpr (I < sizeof...(Algos)) {
            std::get<I>(algos)->start();
            start_algos<I + 1>();
        }
    }


    template<typename ...Algos>
    void Trader<Algos...>::run() {
        event_engine.start();
        auto const& cfg = Config::get();
        gateway.start(cfg.gateway_host, cfg.gateway_port, cfg.gateway_client_id);
        portfolio.start();
        start_algos();

        while (true) {
            send_timer();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            // algo's (and other modules') stopping conditions (risk too high, market closed...) can be checked here
        }
        portfolio.stop();
        gateway.stop();
        event_engine.stop();
    }

    template<typename ...Algos>
    void Trader<Algos...>::send_timer() {
        using namespace std::chrono;
        using namespace std::chrono_literals;
        static auto target = floor<seconds>(system_clock::now()) + 1s;
        auto t = floor<seconds>(system_clock::now());

        if (t >= target) {
            target += 1s;
            time_t timestamp = t.time_since_epoch().count();
            trading_engine.send_event(Event(EventType::timer, std::make_shared<Timer>(timestamp % 60, timestamp)));
        }

    }


}


#endif //CALM_TRADER_TRADER_H
