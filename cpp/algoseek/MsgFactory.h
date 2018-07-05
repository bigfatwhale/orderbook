//
// Created by Uncle Chu on 10/4/18.
//

#ifndef ALGOSEEK_MSGFACTORY_H
#define ALGOSEEK_MSGFACTORY_H

#include "AddOrderMsg.hpp"
#include <string>
#include <memory>

using namespace std;

namespace algoseek {

class MessageBase;

class MsgFactory
{
// public:

//     static std::shared_ptr<MessageBase> createMsg( timespec timestamp, char msgtype, std::string msg );

// private:
//     // syntatic sugar...
//     using AddOrderMsgDecoder           = AddOrderMsg::add_order_decoder<string::iterator>;
    // using OrderExecutedMsgDecoder      = BATSOrderExecutedMsg::order_executed_decoder<const char*>;
    // using OrderCancelMsgDecoder        = BATSOrderCancelMsg::order_cancel_decoder<const char*>;
    // using TradeMsgDecoder              = BATSTradeMsg::trade_decoder<const char*>;
    // using TradeBreakMsgDecoder         = BATSTradeBreakMsg::trade_break_decoder<const char*>;
    // using TradingStatusMsgDecoder      = BATSTradingStatusMsg::trading_status_decoder<const char*>;
    // using AuctionUpdateMsgDecoder      = BATSAuctionUpdateMsg::auction_update_decoder<const char*>;
    // using AuctionSummaryMsgDecoder     = BATSAuctionSummaryMsg::auction_summary_decoder<const char*>;
    // using RetailPriceImproveMsgDecoder = BATSRetailPriceImproveMsg::retail_price_improve_decoder<const char*>;
};

}

#endif // ALGOSEEK_MSGFACTORY_H
