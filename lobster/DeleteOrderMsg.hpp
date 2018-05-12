//
// Created by Uncle Chu on 12/5/18.
//

#ifndef ORDERBOOK_DELETEORDERMSG_HPP
#define ORDERBOOK_DELETEORDERMSG_HPP

#include <ctime>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "LobsterUtil.h"
#include "MessageBase.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

namespace lobster
{

    class DeleteOrderMsg : public MessageBase
    {

    public:
        template <typename Iterator>
        struct delete_order_decoder : decoder_base, qi::grammar<Iterator, DeleteOrderMsg()>
        {
            delete_order_decoder(timespec timestamp, char msgtype);

            qi::rule<Iterator, DeleteOrderMsg()> m_wire_msg; // member variables
        };

    public:

        DeleteOrderMsg() : MessageBase() {}
        DeleteOrderMsg(timespec timestamp, char msgtype, uint64_t orderId, uint32_t shares,
                       uint64_t price, int8_t side) : MessageBase(timestamp, msgtype), m_orderId{orderId},
                                                      m_shares{shares}, m_price{price}, m_side{side}
        {}

        uint64_t m_orderId;
        uint32_t m_shares;
        uint64_t m_price;
        int8_t   m_side;
    };

    template<typename Iterator>
    DeleteOrderMsg::delete_order_decoder<Iterator>::delete_order_decoder(timespec timestamp, char msgtype) :
            decoder_base(timestamp, msgtype),
            DeleteOrderMsg::delete_order_decoder<Iterator>::base_type(m_wire_msg)
    {
        // "34395.851552647,3,21138842,200,2239800,-1"
        m_wire_msg = ( qi::ulong_long >> qi::char_(",")
                                      >> qi::int_ >> qi::char_(",")
                                      >> qi::ulong_long >> qi::char_(",")
                                      >> qi::int_ )
        [qi::_val = phi::construct<DeleteOrderMsg> (
                        m_ts, m_mtype, qi::_1, qi::_3, qi::_5, qi::_7 )];
    }


} // lobster

#endif //ORDERBOOK_DELETEORDERMSG_HPP
