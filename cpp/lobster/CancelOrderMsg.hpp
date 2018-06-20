//
// Created by Uncle Chu on 12/5/18.
//

#ifndef ORDERBOOK_CANCELORDERMSG_HPP
#define ORDERBOOK_CANCELORDERMSG_HPP

#include <ctime>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "LobsterUtil.h"
#include "MessageBase.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

namespace lobster
{

    class CancelOrderMsg : public OrderMsgBase
    {

    public:
        template <typename Iterator>
        struct cancel_order_decoder : decoder_base, qi::grammar<Iterator, CancelOrderMsg()>
        {
            cancel_order_decoder(timespec timestamp, char msgtype);

            qi::rule<Iterator, CancelOrderMsg()> m_wire_msg; // member variables
        };

    public:

        CancelOrderMsg() : OrderMsgBase() {}
        CancelOrderMsg(timespec timestamp, char msgtype, uint64_t orderId, uint32_t shares,
                    uint64_t price, int8_t side) : OrderMsgBase(timestamp, msgtype,
                                                                orderId, shares,
                                                                price, side)
        {}

    };

    template<typename Iterator>
    CancelOrderMsg::cancel_order_decoder<Iterator>::cancel_order_decoder(timespec timestamp, char msgtype) :
        decoder_base(timestamp, msgtype),
        CancelOrderMsg::cancel_order_decoder<Iterator>::base_type(m_wire_msg)
    {
        // "21866417,200,2239600,-1"
        m_wire_msg = ( qi::ulong_long     >> qi::char_(",")
                        >> qi::int_       >> qi::char_(",")
                        >> qi::ulong_long >> qi::char_(",")
                        >> qi::int_ )
                [qi::_val = phi::construct<CancelOrderMsg> (
                        m_ts, m_mtype, qi::_1, qi::_3, qi::_5, qi::_7 )];
    }


} // lobster

#endif //ORDERBOOK_CANCELORDERMSG_HPP
