//
// Created by wayne on 16/1/25.
//

#ifndef TRADING_API_H
#define TRADING_API_H
#include "DefaultEWrapper.h"

#include "EWrapper.h"
#include "EReaderOSSignal.h"
#include "EReader.h"
#include "EClientSocket.h"
#include "Order.h"

#include <memory>
#include <vector>
#include <thread>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>

#include "objects/objects.h"
#include "gateway/gateway.h"

namespace calm {
    class IBApi: public DefaultEWrapper {
    public:
        IBApi(IBGateway& gateway);
        ~IBApi() noexcept;
        void start(const char *host="", int port=7497, int client_id=0);
        void stop();
        void error(int id, int errorCode, std::string const &errorString, std::string const &advancedOrderRejectJson);
        void nextValidId(OrderId order_id);
        bool is_connected() const;

        // market dta subscription
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

    private:
        static Contract generate_contract(std::string const &symbol);
        bool connect(const char *host, int port, int client_id);
        void disconnect() const;
        void process_messages();

        IBGateway &gateway;

        EReaderOSSignal m_osSignal;
        EClientSocket * const m_pClient;
        time_t m_sleepDeadline;
        std::unique_ptr<EReader> m_pReader;
        bool m_extraAuth;
        std::string m_bboExchange;

        bool running{false};
        std::thread cb_thread;

        int req_id{0};
        // market data subscription
        std::mutex tick_m;
        std::mutex req_m;
        std::unordered_map<std::string, TickData> ticks;
        std::unordered_map<std::string, std::pair<int, int>> sym2req_ids;
        std::unordered_map<int, std::string> req_id2sym;

        // order
        static Order generate_ib_order(OrderReq const& order_req);
        OrderId order_id{-1};
        std::mutex orders_m;
        std::unordered_map<OrderId, OrderData> orders;


    };
}

#endif //TRADING_API_H
