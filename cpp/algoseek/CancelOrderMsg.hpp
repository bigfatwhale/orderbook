
#ifndef ALGOSEEK_CANCELORDERMSG_HPP
#define ALGOSEEK_CANCELORDERMSG_HPP

#include <ctime>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "MessageBase.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

namespace algoseek
{

    class CancelOrderMsg : public OrderMsgBase
    {

    public:
        template <typename Iterator>
        struct cancel_order_decoder : decoder_base, qi::grammar<Iterator, CancelOrderMsg()>
        {
            cancel_order_decoder(timespec timestamp, char msgtype, int8_t side, int64_t order_id);

            qi::rule<Iterator, CancelOrderMsg()> m_wire_msg; // member variables
        };

    public:

        CancelOrderMsg() : OrderMsgBase() {}

        CancelOrderMsg(timespec timestamp, char msgtype, int64_t orderId, uint32_t shares,
                    uint64_t price, int8_t side) : OrderMsgBase(timestamp, msgtype,
                                                                orderId, shares,
                                                                price, side)
                    
        {}

    };

    template<typename Iterator>
    CancelOrderMsg::cancel_order_decoder<Iterator>::cancel_order_decoder(timespec timestamp, char msgtype, int8_t side, int64_t order_id) :
        decoder_base(timestamp, msgtype, side, order_id),
        CancelOrderMsg::cancel_order_decoder<Iterator>::base_type(m_wire_msg)
    {
        m_wire_msg = ( qi::as_string[ +qi::alnum ] >> "," >>
                       qi::float_                  >> "," >>
                       qi::uint_                   >> "," >>
                       qi::as_string[ *qi::alpha ] >> "," >>  
                       qi::as_string[ *qi::alpha ] )  
                [qi::_val = phi::construct<CancelOrderMsg> (
                        m_ts, m_mtype, m_orderId, qi::_3, qi::_2 * PRICE_MULTIPLIER, m_side )]; 
    }

} // namespace algoseek

#endif //ALGOSEEK_CANCELORDERMSG_HPP
