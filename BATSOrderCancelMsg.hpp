//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSORDERCANCELMSG_HPP
#define PITCH_SPIRIT_BATSORDERCANCELMSG_HPP

#include "BATSMessageBase.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>

namespace qi = boost::spirit::qi;

struct order_cancel_wire
{
    uint64_t oid;
    uint32_t numShares;
};

BOOST_FUSION_ADAPT_STRUCT( order_cancel_wire, oid, numShares )

class BATSOrderCancelMsg : public BATSMessageBase
{
public:
    // nested class for decoding the wire msg
    template<typename Iterator>
    struct order_cancel_decoder : qi::grammar<Iterator, order_cancel_wire()>
    {
        order_cancel_decoder(); // default ctor
        qi::rule<Iterator, order_cancel_wire()> m_wire_msg; // member variables
    };

public:
    BATSOrderCancelMsg(int timestamp, char msgtype, uint64_t orderId, uint32_t shares );

    uint64_t    m_orderId;
    uint32_t    m_shares;
};

BATSOrderCancelMsg::BATSOrderCancelMsg(int timestamp, char msgtype, uint64_t orderId,
                                           uint32_t shares) :
        BATSMessageBase(timestamp, msgtype),
        m_orderId(orderId),
        m_shares(shares)
{
}

template<typename Iterator>
BATSOrderCancelMsg::order_cancel_decoder<Iterator>::order_cancel_decoder() :
        BATSOrderCancelMsg::order_cancel_decoder<Iterator>::base_type(m_wire_msg)
{
    // order and execution ids are 12 characters base 36
    qi::uint_parser<uint64_t, 36, 12, 12> p_orderId;
    qi::uint_parser<uint32_t, 10,  6, 6 > p_shares;

    m_wire_msg  = p_orderId >> p_shares;
}

#endif //PITCH_SPIRIT_BATSORDERCANCELMSG_HPP
