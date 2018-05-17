//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSRETAILPRICEIMPROVEMSG_H
#define PITCH_SPIRIT_BATSRETAILPRICEIMPROVEMSG_H

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/python.hpp>
#include <string>
#include <sstream>
#include "BATSMessageBase.h"
#include "BATSUtil.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

class BATSRetailPriceImproveMsg : public BATSMessageBase
{
public:
    // nested class for decoding the wire msg
    template<typename Iterator>
    struct retail_price_improve_decoder : qi::grammar<Iterator, BATSRetailPriceImproveMsg()>
    {
        retail_price_improve_decoder(char msgtype);
        qi::rule<Iterator, BATSRetailPriceImproveMsg()> m_wire_msg; // member variables
    };

public:
    BATSRetailPriceImproveMsg() : BATSMessageBase() {}
    BATSRetailPriceImproveMsg(int timestamp, char msgtype, std::string const& symbol,
                              char retail_price_improve ) :
                              BATSMessageBase(timestamp, msgtype),
                              m_symbol(symbol),
                              m_retail_price_improve(retail_price_improve)
    {
    }
  
    std::string repr()
    {
        std::stringstream ss;
        ss  << "BATSRetailPriceImproveMsg(timestamp=" << m_timestamp << ", msgtype=" << m_msgtype << ", orderId=" 
            << ", symbol=" << m_symbol << ", retail_price_improve" << m_retail_price_improve << ")";
        return ss.str();
    }

    static void export_to_python()
    {
        boost::python::class_<BATSRetailPriceImproveMsg, boost::python::bases<BATSMessageBase>>("BATSRetailPriceImproveMsg")
                .def(boost::python::init<>())
                .def(boost::python::init<int, char, std::string, char>())
                .def_readwrite("symbol", &BATSRetailPriceImproveMsg::m_symbol)
                .def_readwrite("retail_price_improve", &BATSRetailPriceImproveMsg::m_retail_price_improve)
                .def("__repr__", &BATSRetailPriceImproveMsg::repr);
    }
  
    std::string m_symbol;
    char m_retail_price_improve;
};

template<typename Iterator>
BATSRetailPriceImproveMsg::retail_price_improve_decoder<Iterator>::retail_price_improve_decoder(char msgtype) :
        BATSRetailPriceImproveMsg::retail_price_improve_decoder<Iterator>::base_type(m_wire_msg)
{
    qi::uint_parser< uint32_t, 10, 8, 8 > p_ts;

    m_wire_msg  = ( p_ts >> qi::char_(msgtype)
                         >> qi::as_string[qi::repeat(8)[qi::char_]]
                         >> qi::char_("BASN") )
        [qi::_val = phi::construct<BATSRetailPriceImproveMsg>(qi::_1, qi::_2, qi::_3, qi::_4)];
}

#endif //PITCH_SPIRIT_BATSRETAILPRICEIMPROVEMSG_H
