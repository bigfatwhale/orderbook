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
#include <type_traits>
#include <memory>
#include <string>

using namespace std;

// good template practice below. EnableIfAddOrder/EnableIfTradeMsg/DisableIf are used
// for simulating template function partial specialization (not supported in C++).
// std enable_if allows the use of SFINAE pattern which chooses the special function
// we want to use for BATSAddOrderMsg and BATSTradeMsg. The dummy variable does the magic.
template<typename T>
using EnableIf  = enable_if_t<
        is_same_v<T, BATSAddOrderMsg> | is_same_v<T, BATSTradeMsg>,
        shared_ptr<BATSMessageBase> >;

template <typename T>
using DisableIf = enable_if_t<
        !( is_same_v<T, BATSAddOrderMsg> | is_same_v<T, BATSTradeMsg> ),
        shared_ptr<BATSMessageBase> >;

struct DecodeHelper {

    template<typename DecodeT, typename MsgT> static
    shared_ptr<BATSMessageBase> parse(const char* start, const char* end, DecodeT &decoder )
    {
        auto data = make_shared<MsgT>();

        bool ret = qi::parse(start, end, decoder, *data);
        if (ret)
            return data;
        else
            return nullptr;
    }

    template<typename DecodeT, typename MsgT> static
    DisableIf<MsgT> decode(char msgtype, const char* start, const char* end)
    {
        static DecodeT decoder( msgtype );
        return DecodeHelper::parse<DecodeT, MsgT>(start, end, decoder);
    }

    template<typename DecodeT, typename MsgT> static
    EnableIf<MsgT> decode(char msgtype, const char* start, const char* end)
    {
        static DecodeT decoder_short{ MsgT::shortMsgCode };
        static DecodeT decoder_long { MsgT::longMsgCode  };
        return DecodeHelper::parse<DecodeT, MsgT>(
                start, end, ( msgtype == MsgT::shortMsgCode ) ? decoder_short : decoder_long );
    }
};

shared_ptr<BATSMessageBase>
BATSMsgFactory::createMsg(char msgtype, std::string msg)
{
    return createMsg(msgtype, msg.data(), msg.data() + msg.size());
}

shared_ptr<BATSMessageBase>
BATSMsgFactory::createMsg(char msgtype, const char* start, const char* end)
{
    switch (msgtype)
    {
        case 'A':
        case 'd': {
            return DecodeHelper::decode<AddOrderMsgDecoder, BATSAddOrderMsg>( msgtype, start, end );
            break;
        }
        case 'E': {
            return DecodeHelper::decode<OrderExecutedMsgDecoder, BATSOrderExecutedMsg>( msgtype, start, end);
            break;
        }
        case'X':{
            return DecodeHelper::decode<OrderCancelMsgDecoder, BATSOrderCancelMsg>( msgtype, start, end);
            break;
        }
        case 'P':
        case 'r': {
            return DecodeHelper::decode<TradeMsgDecoder, BATSTradeMsg>( msgtype, start, end);
            break;
        }
        case 'B': {
            return DecodeHelper::decode<TradeBreakMsgDecoder, BATSTradeBreakMsg>( msgtype, start, end);
            break;
        }
        case 'H':{
            return DecodeHelper::decode<TradingStatusMsgDecoder, BATSTradingStatusMsg>( msgtype, start, end);
            break;
        }
        case 'I':{
            return DecodeHelper::decode<AuctionUpdateMsgDecoder, BATSAuctionUpdateMsg>( msgtype, start, end);
            break;
        }
        case 'J':{
            return DecodeHelper::decode<AuctionSummaryMsgDecoder, BATSAuctionSummaryMsg>( msgtype, start, end);
            break;
        }
        case 'R':{
            return DecodeHelper::decode<RetailPriceImproveMsgDecoder, BATSRetailPriceImproveMsg>( msgtype, start, end);
            break;
        }
        default:{
            throw runtime_error("Error parsing message = " + string(start, end));
        }
    }
}
