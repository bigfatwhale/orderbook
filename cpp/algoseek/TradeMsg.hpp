
#ifndef ALGOSEEK_TRADEMSG_HPP
#define ALGOSEEK_TRADEMSG_HPP

#include <ctime>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "MessageBase.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

namespace algoseek
{

    class TradeMsg : public OrderMsgBase
    {

    public:
        template <typename Iterator>
        struct trade_decoder : decoder_base, qi::grammar<Iterator, TradeMsg()>
        {
            trade_decoder(timespec timestamp, char msgtype, int8_t side, int64_t order_id);

            qi::rule<Iterator, TradeMsg()> m_wire_msg; // member variables
        };

    public:

        TradeMsg() : OrderMsgBase() {}

        TradeMsg(timespec timestamp, char msgtype, int64_t orderId, uint32_t shares,
                    uint64_t price, int8_t side) : OrderMsgBase(timestamp, msgtype,
                                                                orderId, shares,
                                                                price, side)
                    
        {}

    };

    template<typename Iterator>
    TradeMsg::trade_decoder<Iterator>::trade_decoder(timespec timestamp, char msgtype, int8_t side, int64_t order_id) :
        decoder_base(timestamp, msgtype, side, order_id),
        TradeMsg::trade_decoder<Iterator>::base_type(m_wire_msg)
    {
        m_wire_msg = ( qi::as_string[ +qi::alnum ] >> "," >>
                       qi::float_                  >> "," >>
                       qi::uint_                   >> "," >>
                       qi::as_string[ *qi::alpha ] >> "," >>  
                       qi::as_string[ *qi::alpha ] )  
                [qi::_val = phi::construct<TradeMsg> (
                        m_ts, m_mtype, m_orderId, qi::_3, qi::_2 * PRICE_MULTIPLIER, m_side )]; 
    }

} // namespace algoseek

#endif //ALGOSEEK_TRADEMSG_HPP
