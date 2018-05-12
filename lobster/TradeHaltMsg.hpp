//
// Created by Uncle Chu on 12/5/18.
//

#ifndef ORDERBOOK_TRADEHALTMSG_HPP
#define ORDERBOOK_TRADEHALTMSG_HPP


#include <ctime>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "LobsterUtil.h"
#include "MessageBase.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

namespace lobster
{

    class TradeHaltMsg : public MessageBase
    {

    public:
        template <typename Iterator>
        struct trade_halt_decoder : decoder_base, qi::grammar<Iterator, TradeHaltMsg()>
        {
            trade_halt_decoder(timespec timestamp, char msgtype);

            qi::rule<Iterator, TradeHaltMsg()> m_wire_msg; // member variables
        };

    public:

        TradeHaltMsg() : MessageBase() {}
        TradeHaltMsg(timespec timestamp, char msgtype, int8_t price) :
                MessageBase(timestamp, msgtype), m_price{price}
        {}

        int8_t  m_price; // price is either of [-1, 0, 1]
    };

    template<typename Iterator>
    TradeHaltMsg::trade_halt_decoder<Iterator>::trade_halt_decoder(timespec timestamp, char msgtype) :
            decoder_base(timestamp, msgtype),
            TradeHaltMsg::trade_halt_decoder<Iterator>::base_type(m_wire_msg)
    {
        m_wire_msg = ( qi::uint_(0) >> qi::char_(",")
                                      >> qi::uint_(0) >> qi::char_(",")
                                      >> qi::int_ >> qi::char_(",")
                                      >> (qi::int_(-1)|qi::int_(0)|qi::int_(1)) )
        [qi::_val = phi::construct<TradeHaltMsg>( m_ts, m_mtype, qi::_5 )];
    }


} // lobster


#endif //ORDERBOOK_TRADEHALTMSG_HPP
