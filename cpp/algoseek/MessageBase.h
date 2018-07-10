
#ifndef ALGOSEEK_BATSMESSAGEBASE_H
#define ALGOSEEK_BATSMESSAGEBASE_H

#include <ctime>
#include <cstdint>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "AlgoseekUtils.h"

namespace qi  = boost::spirit::qi;
namespace phi = boost::phoenix;

const int PRICE_MULTIPLIER = 10000;

namespace algoseek {

    template <typename Iterator, typename MsgType>
    struct msg_decoder : qi::grammar<Iterator, MsgType()> 
    {
        msg_decoder(timespec timestamp, char msgtype, int8_t side, int64_t order_id) : 
            msg_decoder::base_type(m_wire_msg), 
                    m_ts(timestamp), m_mtype(msgtype), m_side{side}, m_orderId{order_id} 
            {
                m_wire_msg = (  qi::as_string[ +qi::alnum ] >> "," >>
                                qi::float_                  >> "," >>
                                qi::uint_                   >> "," >>
                                qi::as_string[ *qi::alpha ] >> "," >>  
                                qi::as_string[ *qi::alpha ] )  
                            [qi::_val = phi::construct<MsgType> (
                                m_ts, m_mtype, m_orderId, qi::_3, qi::_2 * PRICE_MULTIPLIER, m_side )];           
            }

        qi::rule<Iterator, MsgType()> m_wire_msg; // member variables
        timespec m_ts;
        char     m_mtype;
        int8_t  m_side;
        int64_t m_orderId;
    };

    class MessageBase
    {
    public:
        MessageBase();

        MessageBase(timespec timestamp, char msgtype);

        virtual ~MessageBase();

        timespec m_timestamp;
        char m_msgtype;
    };
    
    class OrderMsgBase : public MessageBase
    {
    public:

        OrderMsgBase();
        OrderMsgBase(timespec timestamp, char msgtype,
                     uint64_t orderId, uint32_t shares,
                     uint64_t price, int8_t side);

        uint64_t m_orderId;
        uint32_t m_shares;
        uint64_t m_price;
        int8_t   m_side;
    };

}

#endif //ALGOSEEK_BATSMESSAGEBASE_H