//
// Created by Uncle Chu on 10/4/18.
//

#ifndef PITCH_SPIRIT_BATSMSGFACTORY_H
#define PITCH_SPIRIT_BATSMSGFACTORY_H

#include "BATSOrderExecutedMsg.hpp"
#include "BATSAddOrderMsg.hpp"
#include "BATSOrderCancelMsg.hpp"
#include "BATSTradeMsg.hpp"
#include "BATSTradeBreakMsg.hpp"
#include "BATSTradingStatusMsg.hpp"
#include "BATSAuctionUpdateMsg.hpp"
#include "BATSAuctionSummaryMsg.hpp"
#include "BATSRetailPriceImproveMsg.hpp"
#include <string>
#include <memory>

class BATSMessageBase;

class BATSMsgFactory
{
public:

    static std::shared_ptr<BATSMessageBase> createMsg( int timestamp, char msgtype, std::string msg );
    static std::shared_ptr<BATSMessageBase> createMsg( int timestamp, char msgtype, const char* start, const char* end );

private:
    // syntatic sugar...
    using AddOrderMsgDecoder           = BATSAddOrderMsg::add_order_decoder<const char*>;
    using OrderExecutedMsgDecoder      = BATSOrderExecutedMsg::order_executed_decoder<const char*>;
    using OrderCancelMsgDecoder        = BATSOrderCancelMsg::order_cancel_decoder<const char*>;
    using TradeMsgDecoder              = BATSTradeMsg::trade_decoder<const char*>;
    using TradeBreakMsgDecoder         = BATSTradeBreakMsg::trade_break_decoder<const char*>;
    using TradingStatusMsgDecoder      = BATSTradingStatusMsg::trading_status_decoder<const char*>;
    using AuctionUpdateMsgDecoder      = BATSAuctionUpdateMsg::auction_update_decoder<const char*>;
    using AuctionSummaryMsgDecoder     = BATSAuctionSummaryMsg::auction_summary_decoder<const char*>;
    using RetailPriceImproveMsgDecoder = BATSRetailPriceImproveMsg::retail_price_improve_decoder<const char*>;
};


#endif //PITCH_SPIRIT_BATSMSGFACTORY_H
