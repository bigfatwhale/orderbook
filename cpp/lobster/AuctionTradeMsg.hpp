//
// Created by Uncle Chu on 12/5/18.
//

#ifndef ORDERBOOK_AUCTIONTRADEMSG_HPP
#define ORDERBOOK_AUCTIONTRADEMSG_HPP

#include <ctime>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "LobsterUtil.h"
#include "MessageBase.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

namespace lobster
{

    class AuctionTradeMsg : public MessageBase
    {

    public:
        template <typename Iterator>
        struct auction_trade_decoder : decoder_base, qi::grammar<Iterator, AuctionTradeMsg()>
        {
            auction_trade_decoder(timespec timestamp, char msgtype);

            qi::rule<Iterator, AuctionTradeMsg()> m_wire_msg; // member variables
        };

    public:

        AuctionTradeMsg() : MessageBase() {}
        AuctionTradeMsg(timespec timestamp, char msgtype, int8_t orderId, uint32_t shares,
                    uint64_t price, int8_t side) : MessageBase(timestamp, msgtype), m_orderId{orderId},
                                                   m_shares{shares}, m_price{price}, m_side{side}
        {}

        // http://help.lobsterdata.com/data-documentation/event-type-6, order id field will be negative
        int8_t   m_orderId;
        uint32_t m_shares;
        uint64_t m_price;
        int8_t   m_side;
    };

    template<typename Iterator>
    AuctionTradeMsg::auction_trade_decoder<Iterator>::auction_trade_decoder(timespec timestamp, char msgtype) :
            decoder_base(timestamp, msgtype),
            AuctionTradeMsg::auction_trade_decoder<Iterator>::base_type(m_wire_msg)
    {
        m_wire_msg = ( qi::int_ >> qi::char_(",")
                                  >> qi::int_ >> qi::char_(",")
                                  >> qi::ulong_long >> qi::char_(",")
                                  >> qi::int_ )
        [qi::_val = phi::construct<AuctionTradeMsg> (
                        m_ts, m_mtype, qi::_1, qi::_3, qi::_5, qi::_7 )];
    }


} // lobster

#endif //ORDERBOOK_AUCTIONTRADEMSG_HPP
