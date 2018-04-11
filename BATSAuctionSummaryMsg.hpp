//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSAUCTIONSUMMARYMSG_HPP
#define PITCH_SPIRIT_BATSAUCTIONSUMMARYMSG_HPP


#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include "BATSMessageBase.h"
#include "BATSUtil.h"

namespace qi = boost::spirit::qi;

struct auction_summary_wire
{
    std::string symbol;
    char auction_type;
    double price;
    uint32_t shares;
};

BOOST_FUSION_ADAPT_STRUCT( auction_summary_wire, symbol, auction_type, price, shares )

class BATSAuctionSummaryMsg : public BATSMessageBase
{

public:
    // nested class for decoding the wire msg
    template<typename Iterator>
    struct auction_summary_decoder : qi::grammar<Iterator, auction_summary_wire()>
    {
        auction_summary_decoder(); // default ctor

        qi::rule<Iterator, auction_summary_wire()> m_wire_msg; // member variables
        qi::rule<Iterator, double> m_price;
        qi::rule<Iterator, fixed_point() > m_fixed_point;
    };

public:
    BATSAuctionSummaryMsg(int timestamp, char msgtype, std::string const& symbol, char auction_type,
                          double price, uint32_t shares);

    std::string m_symbol;
    char m_auction_type;
    double m_price;
    uint32_t m_shares;
};

BATSAuctionSummaryMsg::BATSAuctionSummaryMsg(int timestamp, char msgtype, std::string const& symbol,
                                             char auction_type, double price, uint32_t shares) :
        BATSMessageBase(timestamp, msgtype),
        m_symbol(symbol),
        m_auction_type(auction_type),
        m_price(price),
        m_shares(shares)
{
}

template<typename Iterator>
BATSAuctionSummaryMsg::auction_summary_decoder<Iterator>::auction_summary_decoder() :
        BATSAuctionSummaryMsg::auction_summary_decoder<Iterator>::base_type(m_wire_msg)
{
    // order and execution ids are 12 characters base 36
    qi::uint_parser<uint32_t, 10,  10, 10 > p_shares;
    qi::uint_parser<uint32_t, 10,  6, 6 > int_part;
    qi::uint_parser<uint32_t, 10,  4, 4 > dec_part;

    m_price       = m_fixed_point; // this converts to double from fixed point
    m_fixed_point = int_part >> dec_part;

    m_wire_msg    = qi::repeat(8)[qi::char_]
                    >> qi::char_("OCHI")
                    >> m_price
                    >> p_shares;

}


#endif //PITCH_SPIRIT_BATSAUCTIONSUMMARYMSG_HPP
