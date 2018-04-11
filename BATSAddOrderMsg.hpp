//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSADDORDERMSG_HPP
#define PITCH_SPIRIT_BATSADDORDERMSG_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include "BATSMessageBase.h"
#include "BATSUtil.h"

namespace qi = boost::spirit::qi;

struct add_order_wire
{
    uint64_t oid;
    char side;
    uint32_t numShares;
    std::string symbol;
    double price;
    char display;
    std::string partId;
};

BOOST_FUSION_ADAPT_STRUCT( add_order_wire, oid, side, numShares, symbol, price, display, partId )

class BATSAddOrderMsg : public BATSMessageBase
{

public:
    // nested class for decoding the wire msg
    template<typename Iterator>
    struct add_order_decoder : qi::grammar<Iterator, add_order_wire()>
    {
        //add_order_decoder(bool isLong); // default ctor
        add_order_decoder(bool isLong); // default ctor

        qi::rule<Iterator, add_order_wire()> m_wire_msg; // member variables
        qi::rule<Iterator, double> m_price;
        qi::rule<Iterator, fixed_point() > m_fixed_point;
    };

public:
    BATSAddOrderMsg(int timestamp, char msgtype, uint64_t orderId, char side, uint32_t shares,
                    std::string const &symbol, double price, char display, std::string const &partId);

    uint64_t    m_orderId;
    char        m_side;
    uint32_t    m_shares;
    std::string m_symbol;
    double      m_price;
    char        m_display;
    std::string m_partId;
};

BATSAddOrderMsg::BATSAddOrderMsg(int timestamp, char msgtype, uint64_t orderId,
                                 char side, uint32_t shares, std::string const &symbol,
                                 double price, char display, std::string const &partId) :
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

template<typename Iterator>
BATSAddOrderMsg::add_order_decoder<Iterator>::add_order_decoder(bool isLong) :
        BATSAddOrderMsg::add_order_decoder<Iterator>::base_type(m_wire_msg)
{
    // order and execution ids are 12 characters base 36
    qi::uint_parser<uint64_t, 36, 12, 12> p_orderId;
    qi::uint_parser<uint32_t, 10,  6, 6 > p_shares;
    qi::uint_parser<uint64_t, 10, 10, 10> p_price;
    qi::uint_parser<uint32_t, 10,  6, 6 > int_part;
    qi::uint_parser<uint32_t, 10,  4, 4 > dec_part;

    m_price       = m_fixed_point; // this converts to double from fixed point
    m_fixed_point = int_part >> dec_part;

    if (isLong)
        m_wire_msg    = p_orderId >> qi::char_("BS")
                                  >> p_shares
                                  >> qi::repeat(6)[qi::char_]
                                  >> m_price
                                  >> qi::char_('Y')
                                  >> qi::repeat(4)[qi::char_];
    else
        m_wire_msg    = p_orderId >> qi::char_("BS")
                                  >> p_shares
                                  >> qi::repeat(6)[qi::char_]
                                  >> m_price
                                  >> qi::char_('Y');

}

#endif //PITCH_SPIRIT_BATSADDORDERMSG_HPP
