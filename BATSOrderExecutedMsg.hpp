//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSORDEREXECUTEDMSG_HPP
#define PITCH_SPIRIT_BATSORDEREXECUTEDMSG_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "BATSMessageBase.h"
#include "BATSUtil.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

class BATSOrderExecutedMsg : public BATSMessageBase
{
public:
    // nested class for decoding the wire msg
    template<typename Iterator>
    struct order_executed_decoder : decoder_base, qi::grammar<Iterator, BATSOrderExecutedMsg()>
    {
        order_executed_decoder(int timestamp, char msgtype);
        qi::rule<Iterator, BATSOrderExecutedMsg()> m_wire_msg; // member variables
    };

public:
    BATSOrderExecutedMsg() : BATSMessageBase() {}
    BATSOrderExecutedMsg(int timestamp, char msgtype, uint64_t orderId,
                         uint32_t shares, uint64_t execId ):
        BATSMessageBase(timestamp, msgtype),
        m_orderId(orderId),
        m_shares(shares),
        m_execId(execId)
    {
    }

    uint64_t    m_orderId;
    uint32_t    m_shares;
    uint64_t    m_execId;
};

template<typename Iterator>
BATSOrderExecutedMsg::order_executed_decoder<Iterator>::order_executed_decoder(int timestamp, char msgtype) :
        decoder_base(timestamp, msgtype),
        BATSOrderExecutedMsg::order_executed_decoder<Iterator>::base_type(m_wire_msg)
{
    // order and execution ids are 12 characters base 36
    qi::uint_parser<uint64_t, 36, 12, 12> p_orderId;
    qi::uint_parser<uint32_t, 10,  6, 6 > p_shares;
    qi::uint_parser<uint64_t, 36, 12, 12> p_execId;

    m_wire_msg = ( p_orderId >> p_shares >> p_execId )
        [qi::_val = phi::construct<BATSOrderExecutedMsg>( m_ts, m_mtype, qi::_1, qi::_2, qi::_3)];

}

#endif //PITCH_SPIRIT_BATSORDEREXECUTEDMSG_HPP
