#include "objects.h"
#include "fmt/format.h"

namespace calm {
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



    // TickData
    std::string to_string(TickData const &tick) {
        return fmt::format("TickData(symbol:{}, last_price:{}, last_size:{}, bid_price:{}, bid_size:{}, ask_price:{}, ask_size:{}, timestamp:{})",
                           tick.symbol, tick.last_price, tick.last_size, tick.bid_price, tick.bid_size, tick.ask_price, tick.ask_size, tick.timestamp);
    }

    // OrderData
    std::string to_string(OrderData const & order_data) {
        return fmt::format("OrderData(placeholder)");
    }

}
