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
            add_order_wire wire_data;
            AddOrderMsgDecoder decoder(msgtype == 'd' ? true : false);

            bool ret = qi::parse(msg.begin(), msg.end(), decoder, wire_data);

            if (ret)
                return make_shared<BATSAddOrderMsg>(
                        timestamp, msgtype, wire_data.oid, wire_data.side,
                        wire_data.numShares, wire_data.symbol,
                        wire_data.price, wire_data.display, wire_data.partId);

            break;
        }
        case 'E': {
            OrderExecutedMsgDecoder decoder;
            order_executed_wire wire_data;

            bool ret = qi::parse(msg.begin(), msg.end(), decoder, wire_data);

            if (ret)
                return make_shared<BATSOrderExecutedMsg>(
                        timestamp, msgtype, wire_data.oid, wire_data.numShares, wire_data.execId);
            break;
        }
        case'X':{
            OrderCancelMsgDecoder decoder;
            order_cancel_wire wire_data;

            bool ret = qi::parse(msg.begin(), msg.end(), decoder, wire_data);

            if (ret)
                return make_shared<BATSOrderCancelMsg>(
                        timestamp, msgtype, wire_data.oid, wire_data.numShares);
            break;
        }
        case 'P':
        case 'r': {
            TradeMsgDecoder decoder(msgtype == 'r' ? true : false);
            trade_wire wire_data;

            bool ret = qi::parse(msg.begin(), msg.end(), decoder, wire_data);

            if (ret)
                return make_shared<BATSTradeMsg>(
                        timestamp, msgtype, wire_data.oid, wire_data.side,
                        wire_data.numShares, wire_data.symbol,
                        wire_data.price, wire_data.execId);
            break;
        }
        case 'B': {
            TradeBreakMsgDecoder decoder(timestamp, msgtype);
            shared_ptr<BATSTradeBreakMsg> data = make_shared<BATSTradeBreakMsg>();

            bool ret = qi::parse(msg.begin(), msg.end(), decoder, *data);

            if (ret)
                return data;

            break;
        }
        case 'H':{
            TradingStatusMsgDecoder decoder;
            trading_status_wire wire_data;

            bool ret = qi::parse(msg.begin(), msg.end(), decoder, wire_data);

            if (ret)
                return make_shared<BATSTradingStatusMsg>(
                        timestamp, msgtype, wire_data.symbol, wire_data.halt_status,
                        wire_data.reg_sho_action, wire_data.reserved1, wire_data.reserved2);
            break;

        }
        case 'I':{
            AuctionUpdateMsgDecoder decoder;
            auction_update_wire wire_data;

            bool ret = qi::parse(msg.begin(), msg.end(), decoder, wire_data);

            if (ret)
                return make_shared<BATSAuctionUpdateMsg>(
                        timestamp, msgtype, wire_data.symbol, wire_data.auction_type,
                        wire_data.reference_price, wire_data.buyshares, wire_data.sellshares,
                        wire_data.indicative_price, wire_data.auction_only_price);
            break;

        }
        case 'J':{
            AuctionSummaryMsgDecoder decoder;
            auction_summary_wire wire_data;

            bool ret = qi::parse(msg.begin(), msg.end(), decoder, wire_data);

            if (ret)
                return make_shared<BATSAuctionSummaryMsg>(
                        timestamp, msgtype, wire_data.symbol, wire_data.auction_type,
                        wire_data.price, wire_data.shares);
            break;
        }
        case 'R':{
            RetailPriceImproveMsgDecoder decoder;
            retail_price_improve_wire wire_data;

            bool ret = qi::parse(msg.begin(), msg.end(), decoder, wire_data);

            if (ret)
                return make_shared<BATSRetailPriceImproveMsg>(
                        timestamp, msgtype, wire_data.symbol, wire_data.retail_price_improve);
            break;
        }
        default:
            throw runtime_error("Error parsing message = " + msg);
    }
}