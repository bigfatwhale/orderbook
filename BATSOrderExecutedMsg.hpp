//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSORDEREXECUTEDMSG_HPP
#define PITCH_SPIRIT_BATSORDEREXECUTEDMSG_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/python.hpp>
#include <sstream>
#include "BATSMessageBase.h"
#include "BATSUtil.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

class BATSOrderExecutedMsg : public BATSMessageBase
{
public:
    // nested class for decoding the wire msg
    template<typename Iterator>
    struct order_executed_decoder : qi::grammar<Iterator, BATSOrderExecutedMsg()>
    {
        order_executed_decoder(char msgtype);
        qi::rule<Iterator, BATSOrderExecutedMsg()> m_wire_msg; // member variables
    };

public:
    BATSOrderExecutedMsg() : BATSMessageBase() {}
    BATSOrderExecutedMsg(int timestamp, char msgtype, uint64_t orderId,
                         uint32_t shares, uint64_t execId ):
        BATSMessageBase(timestamp, msgtype),
        m_orderId(orderId),
        m_shares(shares),
        m_execId(execId)
    {
    }

    std::string repr()
    {
        std::stringstream ss;
        ss  << "BATSOrderExecutedMsg(timestamp=" << m_timestamp << ", msgtype=" << m_msgtype << ", orderId=" 
            << m_orderId << ", shares=" << m_shares << ", execId=" << m_execId << ")";
        return ss.str();
    }

    static void export_to_python()
    {
        boost::python::class_<BATSOrderExecutedMsg, boost::python::bases<BATSMessageBase>>("BATSOrderExecutedMsg")
                .def(boost::python::init<>())
                .def(boost::python::init<int, char, uint64_t , uint32_t , uint64_t>())
                .def_readwrite("orderId", &BATSOrderExecutedMsg::m_orderId)
                .def_readwrite("shares", &BATSOrderExecutedMsg::m_shares)
                .def_readwrite("execId", &BATSOrderExecutedMsg::m_execId)
                .def("__repr__", &BATSOrderExecutedMsg::repr);
    }

    uint64_t    m_orderId;
    uint32_t    m_shares;
    uint64_t    m_execId;
};

template<typename Iterator>
BATSOrderExecutedMsg::order_executed_decoder<Iterator>::order_executed_decoder(char msgtype) :
        BATSOrderExecutedMsg::order_executed_decoder<Iterator>::base_type(m_wire_msg)
{
    // order and execution ids are 12 characters base 36
    qi::uint_parser< uint64_t, 36, 12, 12 > p_orderId;
    qi::uint_parser< uint32_t, 10,  6,  6 > p_shares;
    qi::uint_parser< uint64_t, 36, 12, 12 > p_execId;
    qi::uint_parser< uint32_t, 10,  8,  8 > p_ts;

    m_wire_msg = ( p_ts >> qi::char_(msgtype) >> p_orderId >> p_shares >> p_execId )
        [qi::_val = phi::construct<BATSOrderExecutedMsg>( qi::_1, qi::_2, qi::_3, qi::_4, qi::_5)];

}

#endif //PITCH_SPIRIT_BATSORDEREXECUTEDMSG_HPP
