//
// Created by wayne on 16/1/25.
//

#ifndef TRADING_GATEWAY_H
#define TRADING_GATEWAY_H
#include "DefaultEWrapper.h"

#include "EWrapper.h"
#include "EReaderOSSignal.h"
#include "EReader.h"
#include "EClientSocket.h"
#include "Order.h"

#include <memory>
#include <spdlog/async_logger.h>
#include <memory>
#include <vector>
#include <thread>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>

#include "core/event.h"
#include "objects/objects.h"

namespace calm {
    class IBGateway;


    class IBApi: public DefaultEWrapper {
    public:
        IBApi(IBGateway& gateway);
        ~IBApi() noexcept;
        void start(const char *host="", int port=7497, int client_id=0);
        void stop();
        void error(int id, int errorCode, std::string const &errorString, std::string const &advancedOrderRejectJson);
        void nextValidId(OrderId order_id);
        bool is_connected() const;

        // market data subscription
        void subscribe(std::string const & symbol, bool delayed);
        void tickByTickBidAsk(int req_id, time_t time, double bid_price, double ask_price, Decimal bid_size, Decimal ask_size, TickAttribBidAsk const & tick_attrib_bid_ask);
        void tickByTickAllLast(int req_id, int tick_type, time_t time, double price, Decimal size, const TickAttribLast& tick_attrib_last, std::string const & exchange, std::string const & special_conditions);

        // order
        OrderId get_order_id() const;
        OrderId send_order(OrderReq const & order_req);
        void openOrder(OrderId order_id_, Contract const& contract, Order const &order, OrderState const& order_state);
        void orderStatus(OrderId orderId, const std::string& status, Decimal filled, Decimal remaining,
                         double avgFillPrice, int permId, int parentId, double lastFillPrice, int clientId,
                         const std::string& whyHeld, double mktCapPrice);
        void cancel_order(OrderId order_id);
        void cancel_all_orders();

        // contract
        void req_contract_details(std::string const & symbol);
        void contractDetails(int req_id, ContractDetails const & details);
        void contractDetailsEnd(int req_id);

        // historical data
        int req_historical_bar(std::string const &symbol, std::string const& end_time, std::string const & duration,
                               std::string const &bar_size, std::string const &wts, int use_rth, int format);
        void historicalData(TickerId req_iq, Bar const& bar);
        void historicalDataEnd(int req_id, std::string const& start_str, std::string const& end_str);

    private:
        static Contract generate_contract(std::string const &symbol);
        bool connect(const char *host, int port, int client_id);
        void disconnect() const;
        void process_messages();
        std::shared_ptr<spdlog::async_logger> logger;
        IBGateway &gateway;

        EReaderOSSignal m_osSignal;
        EClientSocket * const m_pClient;
        time_t m_sleepDeadline;
        std::unique_ptr<EReader> m_pReader;
        bool m_extraAuth;
        std::string m_bboExchange;

        bool running{false};
        std::thread cb_thread;

        int m_req_id{10'000'000};  // req_id is set be different from order_id
        // market data subscription
        std::mutex tick_m;
        std::mutex req_m;
        std::unordered_map<std::string, TickData> ticks;
        std::unordered_map<std::string, std::pair<int, int>> sym2req_ids;
        std::unordered_map<int, std::string> req_id2sym;

        // order
        static Order generate_ib_order(OrderReq const& order_req);
        OrderId m_order_id{-1};
        std::mutex order_req_m;
        std::unordered_map<OrderId, OrderReq> order_reqs;
        std::mutex orders_m;
        std::unordered_map<OrderId, OrderData> orders;

    };

    class IBGateway{
    public:
        IBGateway(EventEngine &event_engine);

        void on_event(Event const & event);
        void on_error(ErrMsg const & err_msg);
        void on_event(Event&& event);
        void on_order(OrderData const & order_data);
        void on_tick(TickData const & tick_data);
        void on_hist_bar(HistBar const& bar);
        void on_hist_bar_end(HistBarEnd const &bar_end);


    private:
        std::shared_ptr<spdlog::async_logger> logger;
        IBApi api;
        EventEngine &event_engine;
    };


}

#endif //TRADING_GATEWAY_H
