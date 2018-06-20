//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSTRADEMSG_HPP
#define PITCH_SPIRIT_BATSTRADEMSG_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "BATSMessageBase.h"
#include "BATSUtil.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

class BATSTradeMsg : public BATSMessageBase
{

public:
    // nested class for decoding the wire msg
    template<typename Iterator>
    struct trade_decoder : qi::grammar<Iterator, BATSTradeMsg()>
    {
        trade_decoder(char msgtype);

        qi::rule<Iterator, BATSTradeMsg()> m_wire_msg; // member variables
    };

public:
    BATSTradeMsg() : BATSMessageBase() {}
    BATSTradeMsg(int timestamp, char msgtype, uint64_t orderId, char side, uint32_t shares,
                    std::string const &symbol, uint64_t price, uint64_t execId) :
            BATSMessageBase(timestamp, msgtype),
            m_orderId(orderId),
            m_side(side),
            m_shares(shares),
            m_symbol(symbol),
            m_price(price),
            m_execId(execId)
    {
    }
    
    uint64_t    m_orderId; // Base 36 Numeric values come over the wire in ascii
    char        m_side;
    uint32_t    m_shares;
    std::string m_symbol;
    uint64_t    m_price;
    uint64_t    m_execId; // Base 36 Numeric values come over the wire in ascii

    static constexpr char longMsgCode  {'r'};
    static constexpr char shortMsgCode {'P'};
    static constexpr auto sideFlag    {"BS"};
};

template<typename Iterator>
BATSTradeMsg::trade_decoder<Iterator>::trade_decoder(char msgtype) :
        BATSTradeMsg::trade_decoder<Iterator>::base_type(m_wire_msg)
{
    // order and execution ids are 12 characters base 36
    qi::uint_parser< uint64_t, 36, 12, 12 > p_orderId;
    qi::uint_parser< uint64_t, 36, 12, 12 > p_execId;
    qi::uint_parser< uint32_t, 10,  6,  6 > p_shares;
    qi::uint_parser< uint32_t, 10, 10, 10 > m_price;
    qi::uint_parser< uint32_t, 10,  8,  8 > p_ts;

    if (msgtype == BATSTradeMsg::longMsgCode)
        m_wire_msg = ( p_ts >> qi::char_(msgtype)
                            >> p_orderId
                            >> qi::char_(BATSTradeMsg::sideFlag)
                            >> p_shares
                            >> qi::as_string[qi::repeat(8)[qi::char_]]
                            >> m_price
                            >> p_execId )
                [qi::_val = phi::construct<BATSTradeMsg>(
                        qi::_1, qi::_2, qi::_3, qi::_4, qi::_5, qi::_6, qi::_7, qi::_8)];

    else if ( msgtype == BATSTradeMsg::shortMsgCode )
        m_wire_msg = ( p_ts >> qi::char_(msgtype)
                            >> p_orderId
                            >> qi::char_(BATSTradeMsg::sideFlag)
                            >> p_shares
                            >> qi::as_string[qi::repeat(6)[qi::char_]]
                            >> m_price
                            >> p_execId )
                [qi::_val = phi::construct<BATSTradeMsg>(
                        qi::_1, qi::_2, qi::_3, qi::_4, qi::_5, qi::_6, qi::_7, qi::_8)];

}


#endif //PITCH_SPIRIT_BATSTRADEMSG_HPP
