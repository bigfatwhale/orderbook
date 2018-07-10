
#ifndef ALGOSEEK_FILLORDERMSG_HPP
#define ALGOSEEK_FILLORDERMSG_HPP

#include <ctime>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "MessageBase.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

namespace algoseek
{
    class FillOrderMsg : public OrderMsgBase
    {

    public:

        template <typename Iterator>
        struct fill_order_decoder : msg_decoder<Iterator, FillOrderMsg>
        {
            fill_order_decoder(timespec timestamp, char msgtype, int8_t side, int64_t order_id) :
                msg_decoder<Iterator, FillOrderMsg>(timestamp, msgtype, side, order_id) {}
        };

        FillOrderMsg() : OrderMsgBase() {}
        FillOrderMsg(timespec timestamp, char msgtype, int64_t orderId, uint32_t shares,
                    uint64_t price, int8_t side) : OrderMsgBase(timestamp, msgtype,
                                                                orderId, shares,
                                                                price, side) {}

    };

} // namespace algoseek

#endif //ALGOSEEK_FILLORDERMSG_HPP
