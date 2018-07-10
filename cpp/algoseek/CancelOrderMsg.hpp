
#ifndef ALGOSEEK_CANCELORDERMSG_HPP
#define ALGOSEEK_CANCELORDERMSG_HPP

#include <ctime>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "MessageBase.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

namespace algoseek
{

    class CancelOrderMsg : public OrderMsgBase
    {

    public:

        template <typename Iterator>
        struct cancel_order_decoder : msg_decoder<Iterator, CancelOrderMsg>
        {
            cancel_order_decoder(timespec timestamp, char msgtype, int8_t side, int64_t order_id) :
                msg_decoder<Iterator, CancelOrderMsg>(timestamp, msgtype, side, order_id) {}
        };

        CancelOrderMsg() : OrderMsgBase() {}
        CancelOrderMsg(timespec timestamp, char msgtype, int64_t orderId, uint32_t shares,
                    uint64_t price, int8_t side) : OrderMsgBase(timestamp, msgtype,
                                                                orderId, shares,
                                                                price, side) {}

    };

} // namespace algoseek

#endif //ALGOSEEK_CANCELORDERMSG_HPP
