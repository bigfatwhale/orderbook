//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSTRADEMSG_HPP
#define PITCH_SPIRIT_BATSTRADEMSG_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/python.hpp>
#include "BATSMessageBase.h"
#include "BATSUtil.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

class BATSTradeMsg : public BATSMessageBase
{

public:
    // nested class for decoding the wire msg
    template<typename Iterator>
    struct trade_decoder : decoder_base, qi::grammar<Iterator, BATSTradeMsg()>
    {
        trade_decoder(int timestamp, char msgtype, bool isLong);

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
    
    std::string repr()
    {
        std::stringstream ss;
        ss  << "BATSTradeMsg(timestamp=" << m_timestamp << ", msgtype=" << m_msgtype << ", orderId=" 
            << m_orderId << ", side=" << m_side << ", shares=" << m_shares << ", symbol=" << m_symbol
            << ", price" << m_price << ", execId" << m_execId << ")";
        return ss.str();
    }
  
    static void export_to_python()
    {
        boost::python::class_<BATSTradeMsg, boost::python::bases<BATSMessageBase>>("BATSTradeMsg")
                .def(boost::python::init<>())
                .def(boost::python::init<int, char, uint64_t, char, uint32_t, std::string, uint64_t, uint64_t>())
                .def_readwrite("orderId", &BATSTradeMsg::m_orderId)
                .def_readwrite("side", &BATSTradeMsg::m_side)
                .def_readwrite("shares", &BATSTradeMsg::m_shares)
                .def_readwrite("symbol", &BATSTradeMsg::m_symbol)
                .def_readwrite("price", &BATSTradeMsg::m_price)
                .def_readwrite("execId", &BATSTradeMsg::m_execId)
                .def("__repr__", &BATSTradeMsg::repr);
    }

    uint64_t    m_orderId; // Base 36 Numeric values come over the wire in ascii
    char        m_side;
    uint32_t    m_shares;
    std::string m_symbol;
    uint64_t    m_price;
    uint64_t    m_execId; // Base 36 Numeric values come over the wire in ascii
};

template<typename Iterator>
BATSTradeMsg::trade_decoder<Iterator>::trade_decoder(int timestamp, char msgtype, bool isLong) :
        decoder_base(timestamp, msgtype),
        BATSTradeMsg::trade_decoder<Iterator>::base_type(m_wire_msg)
{
    // order and execution ids are 12 characters base 36
    qi::uint_parser<uint64_t, 36, 12, 12> p_orderId;
    qi::uint_parser<uint64_t, 36, 12, 12> p_execId;
    qi::uint_parser<uint32_t, 10,  6, 6 > p_shares;
    qi::uint_parser<uint32_t, 10,  10, 10 > m_price;

    if (isLong)
        m_wire_msg = ( p_orderId >> qi::char_("BS")
                                  >> p_shares
                                  >> qi::as_string[qi::repeat(8)[qi::char_]]
                                  >> m_price
                                  >> p_execId )
                [qi::_val = phi::construct<BATSTradeMsg>(
                        m_ts, m_mtype, qi::_1, qi::_2, qi::_3, qi::_4, qi::_5, qi::_6)];

    else
        m_wire_msg = ( p_orderId >> qi::char_("BS")
                                  >> p_shares
                                  >> qi::as_string[qi::repeat(6)[qi::char_]]
                                  >> m_price
                                  >> p_execId )
                [qi::_val = phi::construct<BATSTradeMsg>(
                        m_ts, m_mtype, qi::_1, qi::_2, qi::_3, qi::_4, qi::_5, qi::_6)];

}


#endif //PITCH_SPIRIT_BATSTRADEMSG_HPP
