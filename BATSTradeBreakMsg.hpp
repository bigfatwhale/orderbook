//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSTRADEBREAKMSG_HPP
#define PITCH_SPIRIT_BATSTRADEBREAKMSG_HPP

#include "BATSMessageBase.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <string>

namespace qi = boost::spirit::qi;

struct trade_break_wire
{
    uint64_t execId;
};

BOOST_FUSION_ADAPT_STRUCT( trade_break_wire, execId )

class BATSTradeBreakMsg : public BATSMessageBase
{
public:
    // nested class for decoding the wire msg
    template<typename Iterator>
    struct trade_break_decoder : qi::grammar<Iterator, trade_break_wire()>
    {
        trade_break_decoder(); // default ctor
        qi::rule<Iterator, trade_break_wire()> m_wire_msg; // member variables
    };

public:
    BATSTradeBreakMsg(int timestamp, char msgtype, uint64_t execId );

    uint64_t m_execId;
};

BATSTradeBreakMsg::BATSTradeBreakMsg(int timestamp, char msgtype, uint64_t execId ) :
        BATSMessageBase(timestamp, msgtype),
        m_execId(execId)
{
}

template<typename Iterator>
BATSTradeBreakMsg::trade_break_decoder<Iterator>::trade_break_decoder() :
        BATSTradeBreakMsg::trade_break_decoder<Iterator>::base_type(m_wire_msg)
{
    // order and execution ids are 12 characters base 36
    qi::uint_parser<uint64_t, 36, 12, 12> p_execId;
    m_wire_msg  = p_execId;
}

#endif //PITCH_SPIRIT_BATSTRADEBREAKMSG_HPP
