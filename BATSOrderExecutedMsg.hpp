//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSORDEREXECUTEDMSG_HPP
#define PITCH_SPIRIT_BATSORDEREXECUTEDMSG_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include "BATSMessageBase.h"

namespace qi = boost::spirit::qi;

struct order_executed_wire
{
    uint64_t oid;
    uint32_t numShares;
    uint64_t execId;
};

BOOST_FUSION_ADAPT_STRUCT( order_executed_wire, oid, numShares, execId )

class BATSOrderExecutedMsg : public BATSMessageBase
{
public:
    // nested class for decoding the wire msg
    template<typename Iterator>
    struct order_executed_decoder : qi::grammar<Iterator, order_executed_wire()>
    {
        order_executed_decoder(); // default ctor
        qi::rule<Iterator, order_executed_wire()> m_wire_msg; // member variables
    };

public:
    BATSOrderExecutedMsg(int timestamp, char msgtype, uint64_t orderId, uint32_t shares, uint64_t execId );

    uint64_t    m_orderId;
    uint32_t    m_shares;
    uint64_t    m_execId;
};

BATSOrderExecutedMsg::BATSOrderExecutedMsg(int timestamp, char msgtype, uint64_t orderId,
                                           uint32_t shares, uint64_t execId) :
        BATSMessageBase(timestamp, msgtype),
        m_orderId(orderId),
        m_shares(shares),
        m_execId(execId)
{
}

template<typename Iterator>
BATSOrderExecutedMsg::order_executed_decoder<Iterator>::order_executed_decoder() :
        BATSOrderExecutedMsg::order_executed_decoder<Iterator>::base_type(m_wire_msg)
{
    // order and execution ids are 12 characters base 36
    qi::uint_parser<uint64_t, 36, 12, 12> p_orderId;
    qi::uint_parser<uint32_t, 10,  6, 6 > p_shares;
    qi::uint_parser<uint64_t, 36, 12, 12> p_execId;

    m_wire_msg  = p_orderId >> p_shares >> p_execId;

}

#endif //PITCH_SPIRIT_BATSORDEREXECUTEDMSG_HPP
