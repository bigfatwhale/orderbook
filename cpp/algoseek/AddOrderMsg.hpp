
#ifndef ALGOSEEK_ADDORDERMSG_HPP
#define ALGOSEEK_ADDORDERMSG_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "AlgoseekUtils.h"
#include "MessageBase.h"

namespace qi  = boost::spirit::qi;
namespace phi = boost::phoenix;

namespace algoseek {

    class AddOrderMsg : public OrderMsgBase
    {

    public:

        template <typename Iterator>
        struct add_order_decoder : msg_decoder<Iterator, AddOrderMsg>
        {
            add_order_decoder(timespec timestamp, char msgtype, int8_t side, int64_t order_id) :
                msg_decoder<Iterator, AddOrderMsg>(timestamp, msgtype, side, order_id) {}
        };

        AddOrderMsg() : OrderMsgBase() {}
        AddOrderMsg(timespec timestamp, char msgtype, int64_t orderId, uint32_t shares,
                    uint64_t price, int8_t side) : OrderMsgBase(timestamp, msgtype,
                                                                orderId, shares,
                                                                price, side) {}

    };

} //algoseek

#endif // ALGOSEEK_ADDORDERMSG_HPP