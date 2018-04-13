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
using AddOrderMsgDecoder       = BATSAddOrderMsg::add_order_decoder<string::iterator>;
using OrderExecutedMsgDecoder  = BATSOrderExecutedMsg::order_executed_decoder<string::iterator>;
using OrderCancelMsgDecoder    = BATSOrderCancelMsg::order_cancel_decoder<string::iterator>;
using TradeMsgDecoder          = BATSTradeMsg::trade_decoder<string::iterator>;
using TradeBreakMsgDecoder     = BATSTradeBreakMsg::trade_break_decoder<string::iterator>;
using TradingStatusMsgDecoder  = BATSTradingStatusMsg::trading_status_decoder<string::iterator>;
using AuctionUpdateMsgDecoder  = BATSAuctionUpdateMsg::auction_update_decoder<string::iterator>;
using AuctionSummaryMsgDecoder = BATSAuctionSummaryMsg::auction_summary_decoder<string::iterator>;
using RetailPriceImproveMsgDecoder = BATSRetailPriceImproveMsg::retail_price_improve_decoder<string::iterator>;

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
            OrderExecutedMsgDecoder decoder( timestamp, msgtype );
            auto data = make_shared<BATSOrderExecutedMsg>();

            bool ret = qi::parse(msg.begin(), msg.end(), decoder, *data);

            if (ret)
                return data;

            break;
        }
        case'X':{
            OrderCancelMsgDecoder decoder(timestamp, msgtype);
            auto data = make_shared<BATSOrderCancelMsg>();

            bool ret = qi::parse(msg.begin(), msg.end(), decoder, *data);

            if (ret)
                return data;

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
            TradeBreakMsgDecoder decoder(timestamp, msgtype);
            auto data = make_shared<BATSTradeBreakMsg>();

            bool ret = qi::parse(msg.begin(), msg.end(), decoder, *data);

            if (ret)
                return data;

            break;
        }
        case 'H':{
            TradingStatusMsgDecoder decoder(timestamp, msgtype);
            auto data = make_shared<BATSTradingStatusMsg>();

            bool ret = qi::parse(msg.begin(), msg.end(), decoder, *data);

            if (ret)
               return data;
            break;

        }
        case 'I':{
            AuctionUpdateMsgDecoder decoder(timestamp, msgtype);
            auto data = make_shared<BATSAuctionUpdateMsg>();

            bool ret = qi::parse(msg.begin(), msg.end(), decoder, *data);

            if (ret)
                return data;
            break;

        }
        case 'J':{
            AuctionSummaryMsgDecoder decoder(timestamp, msgtype);
            auto data = make_shared<BATSAuctionSummaryMsg>();

            bool ret = qi::parse(msg.begin(), msg.end(), decoder, *data);

            if (ret)
                return data;
            break;
        }
        case 'R':{
            RetailPriceImproveMsgDecoder decoder(timestamp, msgtype);
            auto data = make_shared<BATSRetailPriceImproveMsg>();

            bool ret = qi::parse(msg.begin(), msg.end(), decoder, *data);

            if (ret)
                return data;
            break;
        }
        default:
            throw runtime_error("Error parsing message = " + msg);
    }
}