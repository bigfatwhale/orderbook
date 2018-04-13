//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSORDERCANCELMSG_HPP
#define PITCH_SPIRIT_BATSORDERCANCELMSG_HPP


#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "BATSMessageBase.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

class BATSOrderCancelMsg : public BATSMessageBase
{
public:
    // nested class for decoding the wire msg
    template<typename Iterator>
    struct order_cancel_decoder : qi::grammar<Iterator, BATSOrderCancelMsg()>
    {
        order_cancel_decoder(int timestamp, char msgtype); // default ctor
        qi::rule<Iterator, BATSOrderCancelMsg()> m_wire_msg; // member variables
        int  m_ts;
        char m_mtype;
    };

public:
    BATSOrderCancelMsg() : BATSMessageBase() {}
    BATSOrderCancelMsg(int timestamp, char msgtype, uint64_t orderId, uint32_t shares ) :
            BATSMessageBase(timestamp, msgtype),
            m_orderId(orderId),
            m_shares(shares)
    {
    }

    uint64_t    m_orderId;
    uint32_t    m_shares;
};


template<typename Iterator>
BATSOrderCancelMsg::order_cancel_decoder<Iterator>::order_cancel_decoder(int timestamp, char msgtype) :
        BATSOrderCancelMsg::order_cancel_decoder<Iterator>::base_type(m_wire_msg),
        m_ts(timestamp), m_mtype(msgtype)
{
    // order and execution ids are 12 characters base 36
    qi::uint_parser<uint64_t, 36, 12, 12> p_orderId;
    qi::uint_parser<uint32_t, 10,  6, 6 > p_shares;

    m_wire_msg  = ( p_orderId >> p_shares )
                    [ qi::_val = phi::construct<BATSOrderCancelMsg>( m_ts, m_mtype, qi::_1, qi::_2) ];
}

#endif //PITCH_SPIRIT_BATSORDERCANCELMSG_HPP
