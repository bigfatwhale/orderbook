
#ifndef ALGOSEEK_ORDEREXECUTEDMSG_HPP
#define ALGOSEEK_ORDEREXECUTEDMSG_HPP

#include <ctime>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "MessageBase.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

namespace algoseek
{
    class OrderExecutedMsg : public OrderMsgBase
    {

    public:

        template <typename Iterator>
        struct order_executed_decoder : msg_decoder<Iterator, OrderExecutedMsg>
        {
            order_executed_decoder(timespec timestamp, char msgtype, int8_t side, int64_t order_id) :
                msg_decoder<Iterator, OrderExecutedMsg>(timestamp, msgtype, side, order_id) {}
        };

        OrderExecutedMsg() : OrderMsgBase() {}
        OrderExecutedMsg(timespec timestamp, char msgtype, int64_t orderId, uint32_t shares,
                    uint64_t price, int8_t side) : OrderMsgBase(timestamp, msgtype,
                                                                orderId, shares,
                                                                price, side) {}

    };

} // namespace algoseek

#endif //ALGOSEEK_ORDEREXECUTEDMSG_HPP
