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
using AddOrderMsgDecoder           = BATSAddOrderMsg::add_order_decoder<char *>;
using OrderExecutedMsgDecoder      = BATSOrderExecutedMsg::order_executed_decoder<char *>;
using OrderCancelMsgDecoder        = BATSOrderCancelMsg::order_cancel_decoder<char *>;
using TradeMsgDecoder              = BATSTradeMsg::trade_decoder<char *>;
using TradeBreakMsgDecoder         = BATSTradeBreakMsg::trade_break_decoder<char *>;
using TradingStatusMsgDecoder      = BATSTradingStatusMsg::trading_status_decoder<char *>;
using AuctionUpdateMsgDecoder      = BATSAuctionUpdateMsg::auction_update_decoder<char *>;
using AuctionSummaryMsgDecoder     = BATSAuctionSummaryMsg::auction_summary_decoder<char *>;
using RetailPriceImproveMsgDecoder = BATSRetailPriceImproveMsg::retail_price_improve_decoder<char *>;

struct DecodeHelper {

    template<typename DecodeT> static
    shared_ptr<DecodeT> make_decoder( int timestamp, char msgtype, bool isLong)
    {
        return std::make_shared<DecodeT>(timestamp, msgtype);
    }

    template<typename DecodeT, typename MsgT> static
    shared_ptr<BATSMessageBase> decode(int timestamp, char msgtype, char* start, char* end, bool isLong=false)
    {
        shared_ptr<DecodeT>  decoder = DecodeHelper::make_decoder<DecodeT>( timestamp, msgtype, isLong );
        auto data = make_shared<MsgT>();

        bool ret = qi::parse(start, end, *decoder, *data);
        if (ret)
            return data;
        else
            return nullptr;
    }
};

// specialise for three param AddOrderMsgDecoder constructor types
template<>
shared_ptr<AddOrderMsgDecoder> DecodeHelper::make_decoder<AddOrderMsgDecoder>( int timestamp, char msgtype, bool isLong)
{
    return std::make_shared<AddOrderMsgDecoder>(timestamp, msgtype, isLong);
};

// specialise for three param TradeMsgDecoder constructor
template<>
shared_ptr<TradeMsgDecoder> DecodeHelper::make_decoder<TradeMsgDecoder>( int timestamp, char msgtype, bool isLong)
{
    return std::make_shared<TradeMsgDecoder>(timestamp, msgtype, isLong);
};

shared_ptr<BATSMessageBase>
BATSMsgFactory::createMsg(int timestamp, char msgtype, std::string msg)
{
    char *m = const_cast<char *>( msg.data() );
//    return createMsg(timestamp, msgtype, msg.data(), msg.data() + msg.size());
    return createMsg(timestamp, msgtype, m, m + msg.size());
}

shared_ptr<BATSMessageBase>
BATSMsgFactory::createMsg(int timestamp, char msgtype, char* start, char* end)
{
    switch (msgtype)
    {
        case 'A':
        case 'd': {
            return DecodeHelper::decode<AddOrderMsgDecoder, BATSAddOrderMsg>
                    (timestamp, msgtype, start, end, msgtype == 'd' ? true : false);
            break;
        }
        case 'E': {
            return DecodeHelper::decode<OrderExecutedMsgDecoder, BATSOrderExecutedMsg>(timestamp, msgtype, start, end);
            break;
        }
        case'X':{
            return DecodeHelper::decode<OrderCancelMsgDecoder, BATSOrderCancelMsg>(timestamp, msgtype, start, end);
            break;
        }
        case 'P':
        case 'r': {
            return DecodeHelper::decode<TradeMsgDecoder, BATSTradeMsg>
                    (timestamp, msgtype, start, end, msgtype == 'r' ? true : false);
            break;
        }
        case 'B': {
            return DecodeHelper::decode<TradeBreakMsgDecoder, BATSTradeBreakMsg>(timestamp, msgtype, start, end);
            break;
        }
        case 'H':{
            return DecodeHelper::decode<TradingStatusMsgDecoder, BATSTradingStatusMsg>(timestamp, msgtype, start, end);
            break;
        }
        case 'I':{
            return DecodeHelper::decode<AuctionUpdateMsgDecoder, BATSAuctionUpdateMsg>(timestamp, msgtype, start, end);
            break;
        }
        case 'J':{
            return DecodeHelper::decode<AuctionSummaryMsgDecoder, BATSAuctionSummaryMsg>(timestamp, msgtype, start, end);
            break;
        }
        case 'R':{
            return DecodeHelper::decode<RetailPriceImproveMsgDecoder, BATSRetailPriceImproveMsg>(timestamp, msgtype, start, end);
            break;
        }
        default:{
            throw runtime_error("Error parsing message = " + string(start, end));
        }
    }
}
