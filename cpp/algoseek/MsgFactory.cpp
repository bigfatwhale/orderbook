//
// Created by Uncle Chu on 10/4/18.
//

#include "MsgFactory.h"
#include "MessageBase.h"
// #include "BATSOrderExecutedMsg.hpp"
#include "AddOrderMsg.hpp"
// #include "BATSOrderCancelMsg.hpp"
// #include "BATSTradeMsg.hpp"
// #include "BATSTradeBreakMsg.hpp"
// #include "BATSTradingStatusMsg.hpp"
// #include "BATSAuctionUpdateMsg.hpp"
// #include "BATSAuctionSummaryMsg.hpp"
// #include "BATSRetailPriceImproveMsg.hpp"
#include <type_traits>
#include <memory>
#include <string>
#include "MsgFactory.h"

using namespace std;
using namespace algoseek;

namespace algoseek { 

    using AddOrderMsgDecoder           = AddOrderMsg::add_order_decoder<string::iterator>;
    // // using CancelOrderMsgDecoder        = CancelOrderMsg::cancel_order_decoder<string::iterator>;
    // // using DeleteOrderMsgDecoder        = DeleteOrderMsg::delete_order_decoder<string::iterator>;
    // // using OrderExecutedMsgDecoder      = OrderExecutedMsg::order_executed_decoder<string::iterator>;
    // // using AuctionTradeMsgDecoder       = AuctionTradeMsg::auction_trade_decoder<string::iterator>;
    // // using TradeHaltMsgDecoder          = TradeHaltMsg::trade_halt_decoder<string::iterator>;

    template<typename DecodeT, typename MsgT>
    shared_ptr<MessageBase> decode(timespec timestamp, char msgtype, int8_t side, int64_t order_id, string msg)
    {
        shared_ptr<DecodeT>  decoder = std::make_shared<DecodeT>(timestamp, msgtype, side, order_id);
        auto data = make_shared<MsgT>();

        bool ret = qi::parse(msg.begin(), msg.end(), *decoder, *data);
        if (ret)
            return data;
        else
            return nullptr;
    }

    shared_ptr<MessageBase>
    MsgFactory::createMsg(timespec timestamp, char msgtype, int8_t side, int64_t order_id, std::string msg)
    {
        switch (msgtype)
        {
            case 'B':
            case 'S': {
                return decode<AddOrderMsgDecoder, AddOrderMsg>( timestamp, msgtype, side, order_id, msg );
                break;
            }
    //         // case '2': {
    //         //     return decode<CancelOrderMsgDecoder, CancelOrderMsg>( timestamp, msgtype, msg );
    //         //     break;
    //         // }
    //         // case '3': {
    //         //     return decode<DeleteOrderMsgDecoder, DeleteOrderMsg>( timestamp, msgtype, msg );
    //         //     break;
    //         // }
    //         // case '4':
    //         // case '5': {
    //         //     return decode<OrderExecutedMsgDecoder, OrderExecutedMsg>( timestamp, msgtype, msg );
    //         //     break;
    //         // }
    //         // case '6': {
    //         //     return decode<AuctionTradeMsgDecoder, AuctionTradeMsg>( timestamp, msgtype, msg );
    //         //     break;
    //         // }
    //         // case '7': {
    //         //     return decode<TradeHaltMsgDecoder, TradeHaltMsg>( timestamp, msgtype, msg );
    //         //     break;
    //         // }
    //         default:
    //             throw runtime_error("Error parsing message = " + msg);
        }
        return nullptr;
    }

} // namespace algoseek