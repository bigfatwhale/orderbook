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
using EnableIfAddOrder = enable_if_t<is_same_v<T, BATSAddOrderMsg>, BATSAddOrderMsg>;

template<typename T>
using EnableIfTradeMsg = enable_if_t<is_same_v<T, BATSTradeMsg>, BATSTradeMsg>;

template <typename T>
using DisableIf = enable_if_t< !( is_same_v<T, BATSAddOrderMsg> | is_same_v<T, BATSTradeMsg> ), void >;

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
    shared_ptr<BATSMessageBase> decode(char msgtype, const char* start, const char* end,
                                       DisableIf<MsgT> *dummy=0)
    {
        static DecodeT decoder( msgtype );
        return DecodeHelper::parse<DecodeT, MsgT>(start, end, decoder);
    }

    template<typename DecodeT, typename MsgT> static
    shared_ptr<BATSMessageBase> decode(char msgtype, const char* start, const char* end,
                                       EnableIfAddOrder<MsgT> *dummy=0)
    {
        static DecodeT decoder_short{ BATSAddOrderMsg::shortCode };
        static DecodeT decoder_long { BATSAddOrderMsg::longCode  };
        return DecodeHelper::parse<DecodeT, MsgT>(
                start, end, ( msgtype == BATSAddOrderMsg::shortCode ) ? decoder_short : decoder_long );
    }

    template<typename DecodeT, typename MsgT> static
    shared_ptr<BATSMessageBase> decode(char msgtype, const char* start, const char* end,
                                       EnableIfTradeMsg<MsgT> *dummy=0)
    {
        static DecodeT decoder_short{ BATSTradeMsg::shortMsgCode };
        static DecodeT decoder_long { BATSTradeMsg::longMsgCode  };
        return DecodeHelper::parse<DecodeT, MsgT>(
                start, end, ( msgtype == BATSTradeMsg::shortMsgCode ) ? decoder_short : decoder_long );
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
