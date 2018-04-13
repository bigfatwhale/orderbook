//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSTRADINGSTATUSMSG_HPP
#define PITCH_SPIRIT_BATSTRADINGSTATUSMSG_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "BATSMessageBase.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

class BATSTradingStatusMsg : public BATSMessageBase
{

public:
    // nested class for decoding the wire msg
    template<typename Iterator>
    struct trading_status_decoder : qi::grammar<Iterator, BATSTradingStatusMsg()>
    {
        trading_status_decoder(int timestamp, char msgtype);
        qi::rule<Iterator, BATSTradingStatusMsg()> m_wire_msg; // member variables
        int  m_ts;
        char m_mtype;
    };

public:
    BATSTradingStatusMsg() : BATSMessageBase() {}
    BATSTradingStatusMsg(int timestamp, char msgtype, std::string const& symbol,
                         char halt_status, uint8_t reg_sho_action,
                         char reserved1, char reserved2):
            BATSMessageBase(timestamp, msgtype),
            m_symbol(symbol),
            m_halt_status(halt_status),
            m_reg_sho_action(reg_sho_action),
            m_reserved1(reserved1),
            m_reserved2(reserved2)
    {}

    std::string m_symbol;
    char m_halt_status;
    uint8_t m_reg_sho_action;
    char m_reserved1;
    char m_reserved2;

};

template<typename Iterator>
BATSTradingStatusMsg::trading_status_decoder<Iterator>::trading_status_decoder(int timestamp, char msgtype) :
        BATSTradingStatusMsg::trading_status_decoder<Iterator>::base_type(m_wire_msg),
        m_ts(timestamp), m_mtype(msgtype)
{
    // order and execution ids are 12 characters base 36
    qi::uint_parser<uint8_t, 10,  1, 1 > action;

    m_wire_msg = ( qi::as_string[qi::repeat(8)[qi::char_]]
                    >> qi::char_("HQT")
                    >> action
                    >> qi::char_
                    >> qi::char_ )
                  [qi::_val = phi::construct<BATSTradingStatusMsg>(
                          m_ts, m_mtype, qi::_1, qi::_2, qi::_3, qi::_4, qi::_5)];


}

#endif //PITCH_SPIRIT_BATSTRADINGSTATUSMSG_HPP
