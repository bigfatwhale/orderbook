//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSTRADINGSTATUSMSG_HPP
#define PITCH_SPIRIT_BATSTRADINGSTATUSMSG_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include "BATSMessageBase.h"

namespace qi = boost::spirit::qi;

struct trading_status_wire
{
    std::string symbol;
    char halt_status;
    uint8_t reg_sho_action;
    char reserved1;
    char reserved2;
};

BOOST_FUSION_ADAPT_STRUCT( trading_status_wire, symbol, halt_status,
                           reg_sho_action, reserved1, reserved2 )

class BATSTradingStatusMsg : public BATSMessageBase
{

public:
    // nested class for decoding the wire msg
    template<typename Iterator>
    struct trading_status_decoder : qi::grammar<Iterator, trading_status_wire()>
    {
        trading_status_decoder(); // default ctor
        qi::rule<Iterator, trading_status_wire()> m_wire_msg; // member variables
    };

public:
    BATSTradingStatusMsg(int timestamp, char msgtype, std::string const &symbol,
                         char halt_status, uint8_t reg_sho_action,
                         char reserved1, char reserved2);

    std::string m_symbol;
    char m_halt_status;
    uint8_t m_reg_sho_action;
    char m_reserved1;
    char m_reserved2;

};

BATSTradingStatusMsg::BATSTradingStatusMsg(int timestamp, char msgtype, std::string const &symbol,
                                           char halt_status, uint8_t reg_sho_action, char reserved1,
                                           char reserved2) :
        BATSMessageBase(timestamp, msgtype),
        m_symbol(symbol),
        m_halt_status(halt_status),
        m_reg_sho_action(reg_sho_action),
        m_reserved1(reserved1),
        m_reserved2(reserved2)
{
}

template<typename Iterator>
BATSTradingStatusMsg::trading_status_decoder<Iterator>::trading_status_decoder() :
        BATSTradingStatusMsg::trading_status_decoder<Iterator>::base_type(m_wire_msg)
{
    // order and execution ids are 12 characters base 36
    qi::uint_parser<uint8_t, 10,  1, 1 > action;

    m_wire_msg    = qi::repeat(8)[qi::char_] >> qi::char_("HQT")
                              >> action
                              >> qi::char_
                              >> qi::char_;

}

#endif //PITCH_SPIRIT_BATSTRADINGSTATUSMSG_HPP
