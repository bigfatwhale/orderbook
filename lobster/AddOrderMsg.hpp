//
// Created by Uncle Chu on 11/5/18.
//

#ifndef ORDERBOOK_ADDORDERMSG_HPP
#define ORDERBOOK_ADDORDERMSG_HPP

#include <ctime>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "LobsterUtil.h"
#include "MessageBase.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

namespace lobster
{

    class AddOrderMsg : public MessageBase
    {

    public:
        template <typename Iterator>
        struct add_order_decoder : decoder_base, qi::grammar<Iterator, AddOrderMsg()>
        {
            add_order_decoder(timespec timestamp, char msgtype);

            qi::rule<Iterator, AddOrderMsg()> m_wire_msg; // member variables
        };

    public:

        AddOrderMsg() : MessageBase() {}
        AddOrderMsg(timespec timestamp, char msgtype, uint64_t orderId, uint32_t shares,
                    uint64_t price, uint8_t side) : MessageBase(timestamp, msgtype), m_orderId{orderId},
                                                 m_shares{shares}, m_price{price}, m_side{side}
        {}

        uint64_t m_orderId;
        uint32_t m_shares;
        uint64_t m_price;
        uint8_t  m_side;
    };

    template<typename Iterator>
    AddOrderMsg::add_order_decoder<Iterator>::add_order_decoder(timespec timestamp, char msgtype) :
        decoder_base(timestamp, msgtype),
        AddOrderMsg::add_order_decoder<Iterator>::base_type(m_wire_msg)
    {
        m_wire_msg = ( qi::ulong_long >> qi::char_(",")
                        >> qi::int_ >> qi::char_(",")
                        >> qi::ulong_long >> qi::char_(",")
                        >> qi::uint_ )
                [qi::_val = phi::construct<AddOrderMsg> (
                        m_ts, m_mtype, qi::_1, qi::_3, qi::_5, qi::_7 )];
    }


} // lobster

#endif //ORDERBOOK_ADDORDERMSG_HPP
