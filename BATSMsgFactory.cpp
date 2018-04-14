//
// Created by Uncle Chu on 10/4/18.
//

#include "BATSMsgFactory.h"
#include "BATSMessageBase.h"
#include "BATSOrderExecutedMsg.hpp"
#include "BATSAddOrderMsg.hpp"
#include "BATSOrderCancelMsg.hpp"
#include "BATSTradeMsg.hpp"
#include "BATSTradeBreakMsg.hpp"
#include "BATSTradingStatusMsg.hpp"
#include "BATSAuctionUpdateMsg.hpp"
#include "BATSAuctionSummaryMsg.hpp"
#include "BATSRetailPriceImproveMsg.hpp"
#include <memory>
#include <string>

using namespace std;

// syntatic sugar...
using AddOrderMsgDecoder           = BATSAddOrderMsg::add_order_decoder<string::iterator>;
using OrderExecutedMsgDecoder      = BATSOrderExecutedMsg::order_executed_decoder<string::iterator>;
using OrderCancelMsgDecoder        = BATSOrderCancelMsg::order_cancel_decoder<string::iterator>;
using TradeMsgDecoder              = BATSTradeMsg::trade_decoder<string::iterator>;
using TradeBreakMsgDecoder         = BATSTradeBreakMsg::trade_break_decoder<string::iterator>;
using TradingStatusMsgDecoder      = BATSTradingStatusMsg::trading_status_decoder<string::iterator>;
using AuctionUpdateMsgDecoder      = BATSAuctionUpdateMsg::auction_update_decoder<string::iterator>;
using AuctionSummaryMsgDecoder     = BATSAuctionSummaryMsg::auction_summary_decoder<string::iterator>;
using RetailPriceImproveMsgDecoder = BATSRetailPriceImproveMsg::retail_price_improve_decoder<string::iterator>;

template<typename DecodeT, typename MsgT>
shared_ptr<BATSMessageBase> decode(int timestamp, char msgtype, string msg)
{
    DecodeT decoder(timestamp, msgtype);
    auto data = make_shared<MsgT>();

    bool ret = qi::parse(msg.begin(), msg.end(), decoder, *data);
    if (ret)
        return data;
    else
        return nullptr;
};

shared_ptr<BATSMessageBase>
BATSMsgFactory::createMsg(int timestamp, char msgtype, std::string msg)
{

    switch (msgtype)
    {
        case 'A':
        case 'd': {

            AddOrderMsgDecoder decoder( timestamp, msgtype,
                                        msgtype == 'd' ? true : false);
            auto data = make_shared<BATSAddOrderMsg>();

            bool ret = qi::parse(msg.begin(), msg.end(), decoder, *data);

            if (ret)
                return data;

            break;
        }
        case 'E': {
            return decode<OrderExecutedMsgDecoder, BATSOrderExecutedMsg>(timestamp, msgtype, msg);
            break;
        }
        case'X':{
            return decode<OrderCancelMsgDecoder, BATSOrderCancelMsg>(timestamp, msgtype, msg);
            break;
        }
        case 'P':
        case 'r': {
            TradeMsgDecoder decoder(timestamp, msgtype,
                                    msgtype == 'r' ? true : false);
            auto data = make_shared<BATSTradeMsg>();

            bool ret = qi::parse(msg.begin(), msg.end(), decoder, *data);

            if (ret)
                return data;

            break;
        }
        case 'B': {
            return decode<TradeBreakMsgDecoder, BATSTradeBreakMsg>(timestamp, msgtype, msg);
            break;
        }
        case 'H':{
            return decode<TradingStatusMsgDecoder, BATSTradingStatusMsg>(timestamp, msgtype, msg);
            break;
        }
        case 'I':{
            return decode<AuctionUpdateMsgDecoder, BATSAuctionUpdateMsg>(timestamp, msgtype, msg);
            break;
        }
        case 'J':{
            return decode<AuctionSummaryMsgDecoder, BATSAuctionSummaryMsg>(timestamp, msgtype, msg);
            break;
        }
        case 'R':{
            return decode<RetailPriceImproveMsgDecoder, BATSRetailPriceImproveMsg>(timestamp, msgtype, msg);
            break;
        }
        default:
            throw runtime_error("Error parsing message = " + msg);
    }
}