//
// Created by wayne on 16/1/25.
//

#include "gateway.h"
#include "utils/logging.h"
#include "utils/utils.h"
#include "objects/enums.h"

namespace calm {
/*
 * IBApi
 */

    IBApi::IBApi(IBGateway& gateway):
            m_osSignal{1000}, //1-seconds timeout for process message's waitForSignal
            m_pClient{new EClientSocket(this, &m_osSignal)},
            m_sleepDeadline{0},
            m_extraAuth{false},
            gateway{gateway} {
        logger = init_sub_logger("ib_api");
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
            while (m_order_id < 0) std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void IBApi::stop() {
        if (!running) return;
        running = false;

        disconnect();
        if (cb_thread.joinable()) cb_thread.join();
    }


    bool IBApi::connect(const char *host, int port, int client_id) {
        logger->info("Connecting to {}:{} clientId:{}", !( host && *host) ? "127.0.0.1": host, port, client_id);
        bool bRes = m_pClient->eConnect( host, port, client_id, m_extraAuth);

        if (bRes) {
            logger->info("Connected to {}:{} clientId:{}", m_pClient->host().c_str(), m_pClient->port(), client_id);
            m_pReader = std::make_unique<EReader>( m_pClient, &m_osSignal );
            m_pReader->start();
        } else {
            printf("Cannot connect to %s:%d clientId:%d\n", m_pClient->host().c_str(), m_pClient->port(), client_id);
            logger->error("Cannot connect to {}:{} clientId:{}", m_pClient->host().c_str(), m_pClient->port(), client_id);
        }
        return bRes;
    }

    void IBApi::disconnect() const {
        m_pClient->eDisconnect();
        logger->info("in disconnect - Disconnected");
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
            sym2req_ids[symbol] = {m_req_id, m_req_id + 1};
            req_id2sym[m_req_id] = symbol;
            req_id2sym[m_req_id + 1] = symbol;
        }
        {
            std::lock_guard lock{tick_m};
            ticks[symbol] = TickData{symbol};
        }

        m_pClient->reqTickByTickData(m_req_id++, contract, "Last", 10, false);
        m_pClient->reqTickByTickData(m_req_id++, contract, "BidAsk", 10, false);

    }

    void IBApi::tickByTickBidAsk(int req_id, time_t time, double bid_price, double ask_price, Decimal bid_size, Decimal ask_size,
                                 const TickAttribBidAsk &tick_attrib_bid_ask) {

//        logger->info("reqId:{} time:{} bid:{} ask:{} bid_size:{} ask_size:{}", req_id, to_string(time),
//                             bid_price, ask_price, decimalStringToDisplay(bid_size), decimalStringToDisplay(ask_size));
        std::string symbol;
        {
            std::lock_guard lock{req_m};
            if (!req_id2sym.contains(req_id)) return;
            symbol = req_id2sym[req_id];
        }
        TickData tick;
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
        logger->debug(to_string(tick));
        gateway.on_tick(tick);
    }

    void IBApi::tickByTickAllLast(int req_id, int tick_type, time_t time, double price, Decimal size,
                                  const TickAttribLast &tick_attrib_last, const std::string &exchange,
                                  const std::string &special_conditions) {

//        logger->info("reqId:{} time:{} tick_type:{} price:{} size:{} exchange:{}", req_id, to_string(time), tick_type,
//                             price, decimalStringToDisplay(size), exchange);
        std::string symbol;
        {
            std::lock_guard lock{req_m};
            if (!req_id2sym.contains(req_id)) return;
            symbol = req_id2sym[req_id];
        }
        TickData tick;
        {
            std::lock_guard lock{tick_m};
            auto &tick_ = ticks[symbol];
            tick_.last_price = price;
            tick_.last_size = decimalToDouble(size);
            tick_.timestamp = time;
            tick = tick_;
        }
        logger->debug(to_string(tick));
        gateway.on_tick(tick);
    }

    void IBApi::error(int id, int errorCode, std::string const &errorString, std::string const &advancedOrderRejectJson) {
        ErrMsg err_msg{id, errorCode, errorString, advancedOrderRejectJson};
        logger->warn("{}", to_string(err_msg));
        gateway.on_error(err_msg);

        OrderData order_data;
        bool order_flag{false};

        {
            std::lock_guard lock{orders_m};
            if (orders.contains(id)) {
                orders[id].error_code = errorCode;
                order_data = orders[id];
                order_flag = true;
            }
        }
        if (order_flag) gateway.on_order(order_data);

    }

    void IBApi::process_messages() {
        while (running) {
            m_osSignal.waitForSignal();
            errno = 0;
            m_pReader->processMsgs();
        }
    }

    void IBApi::nextValidId(OrderId order_id_) {
        logger->info("In nextValidId: updating order_id to {}", order_id_);
        m_order_id = order_id_;
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
        return m_order_id;
    }


    OrderId IBApi::send_order(calm::OrderReq const & order_req) {
        auto contract = generate_contract(order_req.symbol);
        if (!order_req.exchange.empty()) contract.exchange = order_req.exchange;

        OrderId order_id_ = is_unset_llong(order_req.order_id) ? m_order_id++ : order_req.order_id;
        auto order = generate_ib_order(order_req);
        OrderData order_data{order_id_, order_req.symbol, order_req.exchange, order_req.order_type, order_req.action,
                             order_req.quantity};

        bool order_flag{false};
        {
            std::lock_guard lock{orders_m};
            if (!orders.contains(order_id_)) {
                orders[order_id_] = order_data;
                order_flag = true;
            }
        }
        if (order_flag) gateway.on_order(order_data);
        m_pClient->placeOrder(order_id_, contract, order);
        return order_id_;

    }

    void IBApi::openOrder(OrderId order_id_, const Contract &contract, const Order &order, const OrderState &order_state) {
        logger->debug("In openOrder: contract:{}, order:{}, order_state:{}", to_string(contract), to_string(order), to_string(order_state));
        double commission = order_state.commission;
        if (!is_unset_double(commission)) {
            OrderData order_data;
            {
                std::lock_guard lock{orders_m};
                if (!orders.contains(order_id_)) {
                    logger->warn("OrderId {} is not found in this->orders", order_id_);
                    return;
                }
                orders[order_id_].commission = commission;
                order_data = orders[order_id_];
            }
            gateway.on_order(order_data);
        }

    }

    void IBApi::orderStatus(OrderId orderId, const std::string& status, Decimal filled, Decimal remaining,
                            double avgFillPrice, int permId, int parentId, double lastFillPrice, int clientId,
                            const std::string& whyHeld, double mktCapPrice) {
        logger->debug("In orderStatus: orderId:{}, status:{}, filled:{}, remaining:{}, avgFillPrice:{}, "
                      "permId:{}, parentId:{}, lastFillPrice:{}, clientId:{}, whyHeld:{}, mktCapPrice:{}",
                      orderId, status, decimalToDouble(filled), decimalToDouble(remaining), avgFillPrice, permId,
                      parentId, lastFillPrice, clientId, whyHeld, mktCapPrice);

        OrderStatus status_ = order_status_to_calm[status];
        double filled_ = decimalToDouble(filled);

        OrderData order_data_;

        {
            std::lock_guard lock{orders_m};
            if (!orders.contains(orderId)) {
                logger->warn("OrderId {} is not found in this->orders", orderId);
                return;
            }
            auto &order_data = orders[orderId];
            order_data.status = status_;
            order_data.traded_quantity = filled_;
            order_data.avg_trade_price = avgFillPrice;
            order_data_ = order_data;
        }

        gateway.on_order(order_data_);
    }


    Order IBApi::generate_ib_order(OrderReq const & order_req) {
        Order order;
        order.action = action_to_ib[order_req.action];
        order.totalQuantity = doubleToDecimal(order_req.quantity);
        order.orderType = order_type_to_ib[order_req.order_type];
        if (order_req.order_type == OrderType::limit) order.lmtPrice = order_req.price;

        order.tif = "DAY";
        return order;
    }

    void IBApi::req_contract_details(const std::string &symbol) {
        Contract contract = generate_contract(symbol);
        m_pClient->reqContractDetails(m_req_id++, contract);
    }

    void IBApi::contractDetails(int req_id, const ContractDetails &details) {
        logger->info("req_id:{}, details:{}", req_id, to_string(details));
    }

    void IBApi::contractDetailsEnd(int req_id) {
        logger->info("req_id:{}, contractDetailsEnd", req_id);
    }

    void IBApi::cancel_order(OrderId order_id_) {
        m_pClient->cancelOrder(order_id_, "");
    }

    void IBApi::cancel_all_orders() {
        m_pClient->reqGlobalCancel();
    }

    int IBApi::req_historical_bar(std::string const &symbol, std::string const &end_time,
                                  std::string const &duration, std::string const &bar_size, std::string const &wts,
                                  int use_rth, int format) {
        logger->debug("In req_historical_bar, req_id:{}, symbol:{}, end_time:{}, duration:{}, bar_size:{}, wts:{}, use_rth:{}, format:{}",
                      m_req_id, symbol, end_time, duration, bar_size, wts, use_rth, format);
        auto contract = generate_contract(symbol);
        m_pClient->reqHistoricalData(m_req_id++, contract, end_time, duration, bar_size, wts, use_rth, format, false, TagValueListSPtr());
        return m_req_id - 1;
    }

    void IBApi::historicalData(TickerId req_id, Bar const &bar) {
        logger->debug("In historicalData - req_id:{}, bar:{}", req_id, to_string(bar));

        HistBar bar_ {
                req_id, bar.time, bar.high, bar.low, bar.open, bar.close,
                decimalToDouble(bar.wap), decimalToDouble(bar.volume), bar.count
        };
        gateway.on_hist_bar(bar_);
    }

    void IBApi::historicalDataEnd(int req_id, std::string const &start_str, std::string const &end_str) {
        logger->debug("In historicalDataEnd - req_id:{}, start_str:{}, end_str:{}", req_id, start_str, end_str);
        HistBarEnd bar_end{
                req_id, start_str, end_str
        };
        gateway.on_hist_bar_end(bar_end);
    }



/*
 * IBGateway
 */
    IBGateway::IBGateway(EventEngine &event_engine): event_engine{event_engine}, api(*this) {
        logger = init_sub_logger("ib_gateway");
    }

    void IBGateway::start(const char *host, int port, int client_id) {
        api.start(host, port, client_id);
    }

    void IBGateway::stop() {
        api.stop();
    }


    void IBGateway::subscribe(const std::string &symbol, bool delayed) {
        return api.subscribe(symbol, delayed);
    }

    OrderId IBGateway::send_order(const calm::OrderReq &order_req) {
        return api.send_order(order_req);
    }

    void IBGateway::cancel_order(calm::OrderId order_id) {
        api.cancel_order(order_id);
    }

    void IBGateway::cancel_all_orders() {
        api.cancel_all_orders();
    }

    void IBGateway::req_contract_details(const std::string &symbol) {
        api.req_contract_details(symbol);
    }

    int IBGateway::req_historical_bar(const std::string &symbol, const std::string &end_time,
                                      const std::string &duration, const std::string &bar_size, const std::string &wts,
                                      int use_rth, int format) {
        return api.req_historical_bar(symbol, end_time, duration, bar_size, wts, use_rth, format);
    }

    void IBGateway::on_event(calm::Event const & event) {
        event_engine.send(event);
    }

    void IBGateway::on_event(calm::Event &&event) {
        event_engine.send(std::move(event));
    }

    void IBGateway::on_order(calm::OrderData const &order_data) {
        Event event{EventType::order_data, std::make_shared<OrderData>(order_data)};
        on_event(std::move(event));
    }

    void IBGateway::on_tick(TickData const &tick_data) {
        Event event{EventType::tick_data, std::make_shared<TickData>(tick_data)};
        on_event(std::move(event));
    }

    void IBGateway::on_error(ErrMsg const &err_msg) {
        Event event{EventType::ib_err_msg, std::make_shared<ErrMsg>(err_msg)};
        on_event(std::move(event));
    }

    void IBGateway::on_hist_bar(HistBar const &bar) {
        Event event{EventType::hist_bar, std::make_shared<HistBar>(bar)};
        on_event(std::move(event));
    }

    void IBGateway::on_hist_bar_end(HistBarEnd const &bar_end) {
        Event event{EventType::hist_bar_end, std::make_shared<HistBarEnd>(bar_end)};
        on_event(std::move(event));
    }

}



