//
// Created by wayne on 16/1/25.
//

#include <iostream>
#include <chrono>

#include "api.h"
#include "objects/objects.h"
#include "utils/utils.h"


namespace calm {
    IBApi::IBApi(IBGateway& gateway):
            m_osSignal{1000}, //1-seconds timeout for process message's waitForSignal
            m_pClient{new EClientSocket(this, &m_osSignal)},
            m_sleepDeadline{0},
            m_extraAuth{false},
            gateway{gateway} {
    }

    IBApi::~IBApi() noexcept {
        stop();
        if( m_pReader )
            m_pReader.reset();

        delete m_pClient;
    }

    void IBApi::start(const char *host, int port, int client_id) {
        if (running) return;

        if (connect(host, port, client_id)) {
            running = true;
            cb_thread = std::thread(&IBApi::process_messages, this);
            while (order_id < 0) std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::cout << "reached" << std::endl;
        }
    }

    void IBApi::stop() {
        if (!running) return;
        running = false;

        disconnect();
        if (cb_thread.joinable()) cb_thread.join();
    }


    bool IBApi::connect(const char *host, int port, int client_id) {
        // trying to connect
        printf( "Connecting to %s:%d clientId:%d\n", !( host && *host) ? "127.0.0.1" : host, port, client_id);

        //! [connect]
        bool bRes = m_pClient->eConnect( host, port, client_id, m_extraAuth);
        //! [connect]

        if (bRes) {
            printf( "Connected to %s:%d clientId:%d\n", m_pClient->host().c_str(), m_pClient->port(), client_id);
            //! [ereader]
            m_pReader = std::make_unique<EReader>( m_pClient, &m_osSignal );
            m_pReader->start();
            //! [ereader]
        } else {
            printf("Cannot connect to %s:%d clientId:%d\n", m_pClient->host().c_str(), m_pClient->port(), client_id);
        }
        return bRes;
    }

    void IBApi::disconnect() const {
        m_pClient->eDisconnect();
        printf ( "Disconnected\n");
    }

    void IBApi::subscribe(std::string const & symbol, bool delayed) {
        if (!running) return;
        if (sym2req_ids.contains(symbol)) return;

        if (delayed) {
            m_pClient->reqMarketDataType(4);
        } else {
            m_pClient->reqMarketDataType(1);
        }
        auto contract = generate_contract(symbol);

        {
            std::lock_guard lock{req_m};
            sym2req_ids[symbol] = {req_id + 1, req_id + 2};
            req_id2sym[req_id + 1] = symbol;
            req_id2sym[req_id + 2] = symbol;
        }
        {
            std::lock_guard lock{tick_m};
            ticks[symbol] = Tick{symbol};
        }

        ++req_id;
        m_pClient->reqTickByTickData(req_id, contract, "Last", 10, false);
        ++req_id;
        m_pClient->reqTickByTickData(req_id, contract, "BidAsk", 10, false);

    }

    void IBApi::tickByTickBidAsk(int req_id, time_t time, double bid_price, double ask_price, Decimal bid_size, Decimal ask_size,
                            const TickAttribBidAsk &tick_attrib_bid_ask) {

//        gateway.logger->info("reqId:{} time:{} bid:{} ask:{} bid_size:{} ask_size:{}", req_id, to_string(time),
//                             bid_price, ask_price, decimalStringToDisplay(bid_size), decimalStringToDisplay(ask_size));
        std::string symbol;
        {
            std::lock_guard lock{req_m};
            if (!req_id2sym.contains(req_id)) return;
            symbol = req_id2sym[req_id];
        }
        Tick tick;
        {
            std::lock_guard lock{tick_m};
            auto &tick_ = ticks[symbol];
            tick_.ask_price = ask_price;
            tick_.ask_size = decimalToDouble(ask_size);
            tick_.bid_price = bid_price;
            tick_.bid_size = decimalToDouble(bid_size);
            tick_.timestamp = time;
            tick = tick_;
        }
        gateway.logger->info(to_string(tick));
    }

    void IBApi::tickByTickAllLast(int req_id, int tick_type, time_t time, double price, Decimal size,
                                  const TickAttribLast &tick_attrib_last, const std::string &exchange,
                                  const std::string &special_conditions) {

//        gateway.logger->info("reqId:{} time:{} tick_type:{} price:{} size:{} exchange:{}", req_id, to_string(time), tick_type,
//                             price, decimalStringToDisplay(size), exchange);
        std::string symbol;
        {
            std::lock_guard lock{req_m};
            if (!req_id2sym.contains(req_id)) return;
            symbol = req_id2sym[req_id];
        }
        Tick tick;
        {
            std::lock_guard lock{tick_m};
            auto &tick_ = ticks[symbol];
            tick_.last_price = price;
            tick_.last_size = decimalToDouble(size);
            tick_.timestamp = time;
            tick = tick_;
        }
        gateway.logger->info(to_string(tick));
    }

    void IBApi::error(int id, int errorCode, std::string const &errorString, std::string const &advancedOrderRejectJson) {
        gateway.logger->warn("id:{} errorCode:{} errorMsg:{} advancedOrderRejectionJson:{}", id, errorCode, errorString, advancedOrderRejectJson);
//        std::cout << "id:" << id << " errorCode: " << errorCode << " errorMsg: " << errorString << " advancedOrderRejectJson: " << advancedOrderRejectJson << std::endl;
    }

    void IBApi::process_messages() {
        while (running) {
            m_osSignal.waitForSignal();
            errno = 0;
            m_pReader->processMsgs();
        }
    }

    void IBApi::nextValidId(OrderId order_id) {
        gateway.logger->info("In nextValidId: updating order_id to {}", order_id);
        this->order_id = order_id;
    }

    bool IBApi::is_connected() const {
        return m_pClient->isConnected();
    }

    Contract IBApi::generate_contract(const std::string &symbol) {
        Contract contract;
        auto sym_ex = split(symbol, ".");
        auto const & fields_ = sym_ex[0], exchange = sym_ex[1];
        auto fields = split(fields_, "-");
        auto const & symbol_ = fields[0], sec_type = fields[1], curr = fields[2];
        contract.symbol = symbol_;
        contract.secType = sec_type;
        contract.currency = curr;
        contract.exchange = exchange;
        return contract;
    }

    OrderId IBApi::get_order_id() const {
        return order_id;
    }

    OrderId IBApi::send_order(const std::string &symbol, const std::string &direction, double quantity, double limit_price) {
        auto contract = generate_contract(symbol);
        Order order;
        order.action = direction;
        order.orderType = "LMT";
        order.totalQuantity = doubleToDecimal(quantity);
        order.lmtPrice = limit_price;
        order.tif = "DAY";

        m_pClient->placeOrder(order_id++, contract, order);
        return order_id - 1;
    }

    void IBApi::openOrder(OrderId order_id_, const Contract &contract, const Order &order, const OrderState &order_state) {
        gateway.logger->info("In openOrder: contract:{}, order:{}, order_state:{}", to_string(contract), to_string(order), to_string(order_state));

    }

    void IBApi::orderStatus(OrderId orderId, const std::string& status, Decimal filled, Decimal remaining,
                            double avgFillPrice, int permId, int parentId, double lastFillPrice, int clientId,
                            const std::string& whyHeld, double mktCapPrice){
        gateway.logger->info("In orderStatus: orderId:{}, status:{}, filled:{}, remaining:{}, avgFillPrice:{}, "
                             "permId:{}, parentId:{}, lastFillPrice:{}, clientId:{}, whyHeld:{}, mktCapPrice:{}",
                             orderId, status, decimalToDouble(filled), decimalToDouble(remaining), avgFillPrice, permId,
                             parentId, lastFillPrice, clientId, whyHeld, mktCapPrice);
    }

}






