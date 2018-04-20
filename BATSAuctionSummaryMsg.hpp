//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSAUCTIONSUMMARYMSG_HPP
#define PITCH_SPIRIT_BATSAUCTIONSUMMARYMSG_HPP


#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "BATSMessageBase.h"
#include "BATSUtil.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

class BATSAuctionSummaryMsg : public BATSMessageBase
{

public:
    // nested class for decoding the wire msg
    template<typename Iterator>
    struct auction_summary_decoder : decoder_base, qi::grammar<Iterator, BATSAuctionSummaryMsg()>
    {
        auction_summary_decoder(int timestamp, char msgtype);

        qi::rule<Iterator, BATSAuctionSummaryMsg()> m_wire_msg; // member variables
    };

public:
    BATSAuctionSummaryMsg() : BATSMessageBase() {}
    BATSAuctionSummaryMsg(int timestamp, char msgtype, std::string const& symbol, char auction_type,
                          uint64_t price, uint32_t shares) :
            BATSMessageBase(timestamp, msgtype),
            m_symbol(symbol),
            m_auction_type(auction_type),
            m_price(price),
            m_shares(shares)
    {
    }

    std::string m_symbol;
    char m_auction_type;
    uint64_t m_price;
    uint32_t m_shares;
};

template<typename Iterator>
BATSAuctionSummaryMsg::auction_summary_decoder<Iterator>::auction_summary_decoder(int timestamp, char msgtype) :
        decoder_base(timestamp, msgtype),
        BATSAuctionSummaryMsg::auction_summary_decoder<Iterator>::base_type(m_wire_msg)
{
    // order and execution ids are 12 characters base 36
    qi::uint_parser<uint32_t, 10,  10, 10 > p_shares;
    qi::uint_parser<uint32_t, 10,  10, 10 > m_price;

    m_wire_msg = ( qi::as_string[qi::repeat(8)[qi::char_]]
                    >> qi::char_("OCHI")
                    >> m_price
                    >> p_shares )
            [qi::_val = phi::construct<BATSAuctionSummaryMsg>(m_ts, m_mtype, qi::_1, qi::_2, qi::_3, qi::_4)];

}

#endif //PITCH_SPIRIT_BATSAUCTIONSUMMARYMSG_HPP
