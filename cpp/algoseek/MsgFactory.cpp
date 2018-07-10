
#include "MsgFactory.h"
#include "MessageBase.h"
#include "AddOrderMsg.hpp"
#include "OrderExecutedMsg.hpp"
#include "CancelOrderMsg.hpp"
#include "FillOrderMsg.hpp"
#include "DeleteOrderMsg.hpp"
#include "TradeMsg.hpp"
#include "CrossMsg.hpp"
#include <memory>
#include <string>

using namespace std;
using namespace algoseek;

namespace algoseek { 

    using AddOrderMsgDecoder           = AddOrderMsg::add_order_decoder<string::iterator>;
    using CancelOrderMsgDecoder        = CancelOrderMsg::cancel_order_decoder<string::iterator>;
    using FillOrderMsgDecoder          = FillOrderMsg::fill_order_decoder<string::iterator>;
    using DeleteOrderMsgDecoder        = DeleteOrderMsg::delete_order_decoder<string::iterator>;
    using OrderExecutedMsgDecoder      = OrderExecutedMsg::order_executed_decoder<string::iterator>;
    using TradeMsgDecoder              = TradeMsg::trade_decoder<string::iterator>;
    using CrossMsgDecoder              = CrossMsg::cross_msg_decoder<string::iterator>;

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

            case 'E': {
                return decode<OrderExecutedMsgDecoder, OrderExecutedMsg>( timestamp, msgtype, side, order_id, msg );
                break;
            }

            case 'C': {
                return decode<CancelOrderMsgDecoder, CancelOrderMsg>( timestamp, msgtype, side, order_id, msg );
                break;
            }

            case 'F': {
                return decode<FillOrderMsgDecoder, FillOrderMsg>( timestamp, msgtype, side, order_id, msg );
                break;
            }

            case 'D': {
                return decode<DeleteOrderMsgDecoder, DeleteOrderMsg>( timestamp, msgtype, side, order_id, msg );
                break;
            }

            case 'T': {
                return decode<TradeMsgDecoder, TradeMsg>( timestamp, msgtype, side, order_id, msg );
                break;
            }

            case 'X': {
                return decode<CrossMsgDecoder, CrossMsg>( timestamp, msgtype, side, order_id, msg );
                break;
            }

            default:
                throw runtime_error("Error parsing message = " + msg);
        }
    
    }

} // namespace algoseek