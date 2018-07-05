
#ifndef ALGOSEEK_ADDORDERMSG_HPP
#define ALGOSEEK_ADDORDERMSG_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "MessageBase.h"
//#include "BATSUtil.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

namespace algoseek {

    // class AddOrderMsg : public MessageBase
    // {

    // public:
    //     template <typename Iterator>
    //     struct add_order_decoder : decoder_base, qi::grammar<Iterator, AddOrderMsg()>
    //     {
    //         add_order_decoder(timespec timestamp, char msgtype);

    //         qi::rule<Iterator, AddOrderMsg()> m_wire_msg; // member variables
    //     };

    // public:

    //     AddOrderMsg() : MessageBase() {}

    //     AddOrderMsg(timespec timestamp, char msgtype, uint64_t orderId, uint32_t shares,
    //                 uint64_t price, int8_t side) : MessageBase(timestamp, msgtype,
    //                                                             orderId, shares,
    //                                                             price, side)
    //     {}

    // };

    // template<typename Iterator>
    // AddOrderMsg::add_order_decoder<Iterator>::add_order_decoder(timespec timestamp, char msgtype) :
    //     decoder_base(timestamp, msgtype),
    //     AddOrderMsg::add_order_decoder<Iterator>::base_type(m_wire_msg)
    // {
    //     m_wire_msg = ( qi::ulong_long     >> qi::char_(",")
    //                     >> qi::int_       >> qi::char_(",")
    //                     >> qi::ulong_long >> qi::char_(",")
    //                     >> qi::int_ )
    //             [qi::_val = phi::construct<AddOrderMsg> (
    //                     m_ts, m_mtype, qi::_1, qi::_3, qi::_5, qi::_7 )]; 
    // }



} //algoseek

#endif