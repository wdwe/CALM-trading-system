#include "objects.h"
#include "fmt/format.h"

namespace calm {
/*
 * IB
 */
    // Contract
    std::string to_string(Contract const& contract) {
         return fmt::format(
                 "Contract(conId:{}, symbol:{}, secType:{}, lastTradeDateOrContractMonth:{}, "
                 "strike:{}, right:{}, multiplier:{}, exchange:{}, primaryExchange:{}, currency:{}, "
                 "localSymbol:{}, tradingClass:{}, includeExpired:{}, secIdType:{}, secId:{}, description:{}, "
                 "issuerId:{}, comboLegsDescrip:{})",
                 contract.conId, contract.symbol, contract.secType, contract.lastTradeDateOrContractMonth,
                 contract.strike, contract.right, contract.multiplier, contract.exchange, contract.primaryExchange,
                 contract.currency, contract.localSymbol, contract.tradingClass, contract.includeExpired,
                 contract.secIdType, contract.secId, contract.description, contract.issuerId, contract.comboLegsDescrip);
    }

    // ContractDetails
    std::string to_string(ContractDetails const & contract_details) {
        return fmt::format("ContractDetails(contract:{}, marketName:{}, minTick:{}, orderTypes:{}, validExchanges:{}, "
                    "priceMagnifier:{}, underConId:{}, longName:{}, contractMonth:{}, industry:{}, category:{}, "
                    "subcategory:{}, timeZoneId:{}, tradingHours:{}, liquidHours:{}, evRule:{}, evMultiplier:{}, "
                    "aggGroup:{}, underSymbol:{}, underSecType:{}, marketRuleIds:{}, realExpirationDate:{}, "
                    "lastTradeTime:{}, stockType:{}, minSize:{}, sizeIncrement:{}, suggestedSizeIncrement:{}, "
                    "cusip:{}, ratings:{}, descAppend:{}, bondType:{}, couponType:{}, callable:{}, putable:{}, "
                    "coupon:{}, convertible:{}, maturity:{}, issueDate:{}, nextOptionDate:{}, nextOptionType:{}, "
                    "nextOptionPartial:{}, notes:{})", to_string(contract_details.contract), contract_details.marketName,
                    contract_details.minTick, contract_details.orderTypes, contract_details.validExchanges,
                    contract_details.priceMagnifier, contract_details.underConId, contract_details.longName,
                    contract_details.contractMonth, contract_details.industry, contract_details.category,
                    contract_details.subcategory, contract_details.timeZoneId, contract_details.tradingHours,
                    contract_details.liquidHours, contract_details.evRule, contract_details.evMultiplier,
                    contract_details.aggGroup, contract_details.underSymbol, contract_details.underSecType,
                    contract_details.marketRuleIds, contract_details.realExpirationDate, contract_details.lastTradeTime,
                    contract_details.stockType, decimalToDouble(contract_details.minSize),
                    decimalToDouble(contract_details.sizeIncrement),
                    decimalToDouble(contract_details.suggestedSizeIncrement), contract_details.cusip,
                    contract_details.ratings, contract_details.descAppend, contract_details.bondType,
                    contract_details.couponType, contract_details.callable, contract_details.putable, contract_details.coupon,
                    contract_details.convertible, contract_details.maturity, contract_details.issueDate,
                    contract_details.nextOptionDate, contract_details.nextOptionType, contract_details.nextOptionPartial,
                    contract_details.notes);

    }

    // OrderState
    std::string to_string(OrderState const & order_state) {
        return fmt::format("OrderState(status:{}, initMarginBefore:{}, maintMarginBefore:{}, "
                           "equityWithLoanBefore:{}, initMarginChange:{}, maintMarginChange:{}, equityWithLoanChange:{}, "
                           "initMarginAfter:{}, maintMarginAfter:{}, equityWithLoanAfter:{}, commission:{}, "
                           "minCommission:{}, maxCommission:{}, commissionCurrency:{}, warningText:{}, completedTime:{}, "
                           "completedStatus:{})", order_state.status, order_state.initMarginBefore,
                           order_state.maintMarginBefore, order_state.equityWithLoanBefore,
                           order_state.initMarginChange, order_state.maintMarginChange,
                           order_state.equityWithLoanChange,
                           order_state.initMarginAfter, order_state.maintMarginAfter, order_state.equityWithLoanAfter,
                           order_state.commission, order_state.minCommission, order_state.maxCommission,
                           order_state.commissionCurrency, order_state.warningText, order_state.completedTime,
                           order_state.completedStatus);
    }

    // Order
    std::string to_string(Order const &order) {
        return fmt::format("Order(orderId:{}, clientId:{}, permId:{}, action:{}, totalQuantity:{}, orderType:{}, "
                           "lmtPrice:{}, auxPrice:{}, tif:{}, activeStartTime:{}, activeStopTime:{}, ocaGroup:{}, ocaType:{}, orderRef:{}, "
                           "transmit:{}, parentId:{}, blockOrder:{}, sweepToFill:{}, displaySize:{}, triggerMethod:{}, "
                           "outsideRth:{}, hidden:{}, goodAfterTime:{}, goodTillDate:{}, rule80A:{}, allOrNone:{}, "
                           "minQty:{}, percentOffset:{}, overridePercentageConstraints:{}, trailStopPrice:{}, "
                           "trailingPercent:{})", order.orderId, order.clientId, order.permId, order.action,
                           decimalToDouble(order.totalQuantity), order.orderType, order.lmtPrice, order.auxPrice,
                           order.tif, order.activeStartTime, order.activeStopTime, order.ocaGroup, order.ocaType,
                           order.orderRef, order.transmit, order.parentId,
                           order.blockOrder, order.sweepToFill, order.displaySize, order.triggerMethod,
                           order.outsideRth, order.hidden, order.goodAfterTime, order.goodTillDate, order.rule80A,
                           order.allOrNone, order.minQty, order.percentOffset, order.overridePercentageConstraints,
                           order.trailStopPrice, order.trailingPercent);
    }

    std::string to_string(Bar const &bar) {
        return fmt::format("Bar(time:{}, high:{}, low:{}, open:{}, close:{}, wap:{}, volume:{}, count:{})",
                           bar.time, bar.high, bar.low, bar.open, bar.close, decimalToDouble(bar.wap),
                           decimalToDouble(bar.volume), bar.count);
    }


/*
 * CALM trading
 */
    // Message
    std::string to_string(ErrMsg const & err_msg) {
        return fmt::format("ErrMsg(id:{}, error_code:{}, message:{}, order_rejection_json:{})",
                              err_msg.id, err_msg.error_code, err_msg.message, err_msg.order_rejection_json);
    }


    // TickData
    std::string to_string(TickData const &tick) {
        return fmt::format("TickData(symbol:{}, last_price:{}, last_size:{}, bid_price:{}, bid_size:{}, ask_price:{}, ask_size:{}, timestamp:{})",
                           tick.symbol, tick.last_price, tick.last_size, tick.bid_price, tick.bid_size, tick.ask_price, tick.ask_size, tick.timestamp);
    }

    // OrderReq
    std::string to_string(OrderReq const &req) {
        return fmt::format("OrderReq(symbol:{}, exchange:{}, source:{}, action:{}, order_type:{}, quantity:{}, price:{}, order_id:{})",
                           req.symbol, req.exchange, req.source, action_to_ib.at(req.action), order_type_to_ib.at(req.order_type), req.quantity, req.price, req.order_id);
    }

    // OrderData
    std::string to_string(OrderData const & order_data) {
        return fmt::format("OrderData(order_id:{}, symbol:{}, exchange:{}, source:{}, order_type:{}, action:{}, total_quantity:{}, "
                           "traded_quantity:{}, avg_trade_price:{}, commission:{}, OrderStatus:{}, error_code:{})",
                           order_data.order_id, order_data.symbol, order_data.exchange, order_data.source, order_type_to_ib.at(order_data.order_type),
                           action_to_ib.at(order_data.action), order_data.total_quantity, order_data.traded_quantity,
                           order_data.avg_trade_price, order_data.commission, order_status_to_ib.at(order_data.status),
                           order_data.error_code);
    }

    // Historical Data
    std::string to_string(HistBar const &bar) {
        return fmt::format("HistBar(id:{}, time:{}, high:{}, low:{}, open:{}, close:{}, wap:{}, volume:{}, count:{})",
                           bar.id, bar.time, bar.high, bar.low, bar.open, bar.close, bar.wap, bar.volume, bar.count);
    }

    std::string to_string(HistBarEnd const& bar_end) {
        return fmt::format("HistBarEnd(id:{}, start:{}, end:{})", bar_end.id, bar_end.start, bar_end.end);
    }

    // Timer
    std::string to_string(Timer const& timer) {
        return fmt::format("Timer(sec:{})", timer.sec);
    }

    // Portfolio
    std::string to_string(AlgoPnL const& pnl) {
        return fmt::format("AlgoPnL(algo:{}, pnl:{}, timestamp:{})", pnl.algo, pnl.pnl, pnl.timestamp);
    }

    std::string to_string(TotalPnL const& pnl) {
        return fmt::format("TotalPnl(pnl:{}, timestamp:{})", pnl.pnl, pnl.timestamp);
    }

}
