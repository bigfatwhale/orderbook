//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSRETAILPRICEIMPROVEMSG_H
#define PITCH_SPIRIT_BATSRETAILPRICEIMPROVEMSG_H


#include <boost/spirit/include/qi.hpp>
#include <string>
#include "BATSMessageBase.h"

namespace qi = boost::spirit::qi;

class BATSRetailPriceImproveMsg : public BATSMessageBase
{
public:
    // nested class for decoding the wire msg
    template<typename Iterator>
    struct retail_price_improve_decoder : qi::grammar<Iterator, BATSRetailPriceImproveMsg()>
    {
        retail_price_improve_decoder(int timestamp, char msgtype); // default ctor
        qi::rule<Iterator, BATSRetailPriceImproveMsg()> m_wire_msg; // member variables
        int  m_ts;
        char m_mtype;
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

    std::string m_symbol;
    char m_retail_price_improve;
};

template<typename Iterator>
BATSRetailPriceImproveMsg::retail_price_improve_decoder<Iterator>::retail_price_improve_decoder
        (int timestamp, char msgtype) :
        BATSRetailPriceImproveMsg::retail_price_improve_decoder<Iterator>::base_type(m_wire_msg),
        m_ts(timestamp), m_mtype(msgtype)
{
    m_wire_msg  = ( qi::as_string[qi::repeat(8)[qi::char_]] >> qi::char_("BASN") )
        [qi::_val = phi::construct<BATSRetailPriceImproveMsg>(m_ts, m_mtype, qi::_1, qi::_2)];
}

#endif //PITCH_SPIRIT_BATSRETAILPRICEIMPROVEMSG_H
