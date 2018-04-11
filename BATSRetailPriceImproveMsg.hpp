//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSRETAILPRICEIMPROVEMSG_H
#define PITCH_SPIRIT_BATSRETAILPRICEIMPROVEMSG_H

#include "BATSMessageBase.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <string>

namespace qi = boost::spirit::qi;

struct retail_price_improve_wire
{
    std::string symbol;
    char retail_price_improve;
};

BOOST_FUSION_ADAPT_STRUCT( retail_price_improve_wire, symbol, retail_price_improve )

class BATSRetailPriceImproveMsg : public BATSMessageBase
{
public:
    // nested class for decoding the wire msg
    template<typename Iterator>
    struct retail_price_improve_decoder : qi::grammar<Iterator, retail_price_improve_wire()>
    {
        retail_price_improve_decoder(); // default ctor
        qi::rule<Iterator, retail_price_improve_wire()> m_wire_msg; // member variables
    };

public:
    BATSRetailPriceImproveMsg(int timestamp, char msgtype, std::string const& symbol,
                              char retail_price_improve );

    std::string m_symbol;
    char m_retail_price_improve;
};

BATSRetailPriceImproveMsg::BATSRetailPriceImproveMsg(int timestamp, char msgtype, std::string const& symbol,
                                                     char retail_price_improve) :
        BATSMessageBase(timestamp, msgtype),
        m_symbol(symbol),
        m_retail_price_improve(retail_price_improve)
{
}

template<typename Iterator>
BATSRetailPriceImproveMsg::retail_price_improve_decoder<Iterator>::retail_price_improve_decoder() :
        BATSRetailPriceImproveMsg::retail_price_improve_decoder<Iterator>::base_type(m_wire_msg)
{
    m_wire_msg  = qi::repeat(8)[qi::char_] >> qi::char_("BASN");
}

#endif //PITCH_SPIRIT_BATSRETAILPRICEIMPROVEMSG_H
