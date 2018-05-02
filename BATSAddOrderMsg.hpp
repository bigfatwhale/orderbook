//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSADDORDERMSG_HPP
#define PITCH_SPIRIT_BATSADDORDERMSG_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/python.hpp>
#include <sstream>
#include "BATSMessageBase.h"
#include "BATSUtil.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

class BATSAddOrderMsg : public BATSMessageBase
{

public:
    // nested class for decoding the wire msg
    template<typename Iterator>
    struct add_order_decoder : decoder_base, qi::grammar<Iterator, BATSAddOrderMsg()>
    {
        add_order_decoder(int timestamp, char msgtype, bool isLong);

        qi::rule<Iterator, BATSAddOrderMsg()> m_wire_msg; // member variables

        // leaving the below for reference on how to convert fixed_point to double using boost spirit
        //qi::rule<Iterator, double> m_price;
        //qi::rule<Iterator, fixed_point() > m_fixed_point;
    };

public:
    BATSAddOrderMsg() : BATSMessageBase() {}
    BATSAddOrderMsg(int timestamp, char msgtype, uint64_t orderId, char side, uint32_t shares,
                    std::string const &symbol, uint64_t price, char display, std::string const &partId) :
            BATSMessageBase(timestamp, msgtype),
            m_orderId(orderId),
            m_side(side),
            m_shares(shares),
            m_symbol(symbol),
            m_price(price),
            m_display(display),
            m_partId(partId)
    {
    }

    std::string repr()
    {
        std::stringstream ss;
        ss << "BATSAddOrderMsg(orderId=" << m_orderId << ", side=" << m_side
           << ", shares=" << m_shares << ", symbol=" << m_symbol << ", price="
           << m_price << ", display=" << m_display << ", partId=" << m_partId << ")";
        return ss.str();
    }

    static void export_to_python()
    {
        boost::python::class_<BATSAddOrderMsg>("BATSAddOrderMsg")
                .def(boost::python::init<>())
                .def(boost::python::init<int, char, uint64_t, char, uint32_t,
                        std::string, uint64_t, char, std::string>())
                .def_readwrite("orderId", &BATSAddOrderMsg::m_orderId)
                .def_readwrite("side", &BATSAddOrderMsg::m_side)
                .def_readwrite("shares", &BATSAddOrderMsg::m_shares)
                .def_readwrite("symbol", &BATSAddOrderMsg::m_symbol)
                .def_readwrite("price", &BATSAddOrderMsg::m_price)
                .def_readwrite("display", &BATSAddOrderMsg::m_display)
                .def_readwrite("partId", &BATSAddOrderMsg::m_partId)
                .def("__repr__", &BATSAddOrderMsg::repr);
    }
    
    uint64_t    m_orderId;
    char        m_side;
    uint32_t    m_shares;
    std::string m_symbol;
    uint64_t    m_price;
    char        m_display;
    std::string m_partId;
};

template<typename Iterator>
BATSAddOrderMsg::add_order_decoder<Iterator>::add_order_decoder(int timestamp, char msgtype, bool isLong) :
        decoder_base(timestamp, msgtype),
        BATSAddOrderMsg::add_order_decoder<Iterator>::base_type(m_wire_msg)
{
    // order and execution ids are 12 characters base 36
    qi::uint_parser< uint64_t, 36, 12, 12 > p_orderId;
    qi::uint_parser< uint32_t, 10,  6, 6  > p_shares;
    qi::uint_parser< uint32_t, 10, 10, 10 > m_price;

    // leaving the below for reference on how to convert fixed_point to double using boost spirit
    // qi::uint_parser<uint32_t, 10,  6, 6 > int_part;
    // qi::uint_parser<uint32_t, 10,  4, 4 > dec_part;
    // m_price       = m_fixed_point; // this converts to double from fixed point
    // m_fixed_point = int_part >> dec_part;

    m_wire_msg = (p_orderId >> qi::char_("BS")
                            >> p_shares
                            >> qi::as_string[qi::repeat(6)[qi::char_]]
                            >> m_price
                            >> qi::char_('Y')
                            >> (qi::as_string[qi::repeat(4)[qi::char_]] | qi::as_string[qi::eps]))
                [qi::_val = phi::construct<BATSAddOrderMsg>(
                    m_ts, m_mtype, qi::_1, qi::_2, qi::_3, qi::_4, qi::_5, qi::_6, qi::_7)];
}

#endif //PITCH_SPIRIT_BATSADDORDERMSG_HPP
