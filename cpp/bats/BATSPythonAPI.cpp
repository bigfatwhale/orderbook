//
// Created by Uncle Chu on 28/4/18.
//

#include "Python.h"
#include "BATSTradingStatusMsg.hpp"
#include "BATSTradeMsg.hpp"
#include "BATSRetailPriceImproveMsg.hpp"
#include "BATSTradeBreakMsg.hpp"
#include "BATSOrderExecutedMsg.hpp"
#include "BATSOrderCancelMsg.hpp"
#include "BATSAuctionUpdateMsg.hpp"
#include "BATSAuctionSummaryMsg.hpp"
#include "BATSAddOrderMsg.hpp"
#include "BATSMessageBase.h"
#include "BATSPitchMsgParser.h"
#include <boost/python.hpp>
#include <ostream>
#include <string>

using namespace boost::python;

void export_add_order_to_python()
{
    class_<BATSAddOrderMsg, bases<BATSMessageBase>>("BATSAddOrderMsg")
            .def(init<>())
            .def(init<int, char, uint64_t, char, uint32_t,
                    std::string, uint64_t, char, std::string>())
            .def_readwrite("orderId", &BATSAddOrderMsg::m_orderId)
            .def_readwrite("side",    &BATSAddOrderMsg::m_side)
            .def_readwrite("shares",  &BATSAddOrderMsg::m_shares)
            .def_readwrite("symbol",  &BATSAddOrderMsg::m_symbol)
            .def_readwrite("price",   &BATSAddOrderMsg::m_price)
            .def_readwrite("display", &BATSAddOrderMsg::m_display)
            .def_readwrite("partId",  &BATSAddOrderMsg::m_partId)
            .def(self_ns::str(self_ns::self));
}

void export_auction_summary_to_python()
{
    class_<BATSAuctionSummaryMsg, bases<BATSMessageBase>>("BATSAuctionSummaryMsg")
            .def(init<>())
            .def(init<int, char, std::string, char, uint64_t, uint32_t>())
            .def_readwrite("symbol", &BATSAuctionSummaryMsg::m_symbol)
            .def_readwrite("auction_type", &BATSAuctionSummaryMsg::m_auction_type)
            .def_readwrite("price", &BATSAuctionSummaryMsg::m_price)
            .def_readwrite("shares", &BATSAuctionSummaryMsg::m_shares)
            .def(self_ns::str(self_ns::self));
}

void export_auction_update_to_python()
{
    class_<BATSAuctionUpdateMsg, bases<BATSMessageBase>>("BATSAuctionUpdateMsg")
            .def(init<>())
            .def(init<int, char, std::string, char, uint64_t, uint32_t, uint32_t, uint64_t, uint64_t>())
            .def_readwrite("symbol", &BATSAuctionUpdateMsg::m_symbol)
            .def_readwrite("auction_type", &BATSAuctionUpdateMsg::m_auction_type)
            .def_readwrite("reference_price", &BATSAuctionUpdateMsg::m_reference_price)
            .def_readwrite("buyshares", &BATSAuctionUpdateMsg::m_buyshares)
            .def_readwrite("sellshares", &BATSAuctionUpdateMsg::m_sellshares)
            .def_readwrite("indicative_price", &BATSAuctionUpdateMsg::m_indicative_price)
            .def_readwrite("auction_only_price", &BATSAuctionUpdateMsg::m_auction_only_price)
            .def(self_ns::str(self_ns::self));
}

void export_message_base_to_python()
{
    //register_ptr_to_python<std::shared_ptr<BATSMessageBase>>();
    class_<BATSMessageBase,
                          std::shared_ptr<BATSMessageBase>,
                          boost::noncopyable >("BATSMessageBase")
            .def(init<>())
            .def(init<int, char>())
            .def_readwrite("timestamp", &BATSMessageBase::m_timestamp)
            .def_readwrite("msgtype", &BATSMessageBase::m_msgtype)
            .def(self_ns::str(self_ns::self));
}

void export_order_cancel_to_python()
{
    class_<BATSOrderCancelMsg, bases<BATSMessageBase>>("BATSOrderCancelMsg")
            .def(init<>())
            .def(init<int, char, uint64_t , uint32_t>())
            .def_readwrite("orderId", &BATSOrderCancelMsg::m_orderId)
            .def_readwrite("shares", &BATSOrderCancelMsg::m_shares)
            .def(self_ns::str(self_ns::self));
}

void export_order_execute_to_python()
{
    class_<BATSOrderExecutedMsg, bases<BATSMessageBase>>("BATSOrderExecutedMsg")
            .def(init<>())
            .def(init<int, char, uint64_t , uint32_t , uint64_t>())
            .def_readwrite("orderId", &BATSOrderExecutedMsg::m_orderId)
            .def_readwrite("shares", &BATSOrderExecutedMsg::m_shares)
            .def_readwrite("execId", &BATSOrderExecutedMsg::m_execId)
            .def(self_ns::str(self_ns::self));
}

void export_msg_parser_to_python()
{
    class_<BATSPitchMsgParser>("BATSPitchMsgParser")
            .def("parse_msg", &BATSPitchMsgParser::parse_msg);

}

void export_retail_price_improve_to_python()
{
    class_<BATSRetailPriceImproveMsg, bases<BATSMessageBase>>("BATSRetailPriceImproveMsg")
            .def(init<>())
            .def(init<int, char, std::string, char>())
            .def_readwrite("symbol", &BATSRetailPriceImproveMsg::m_symbol)
            .def_readwrite("retail_price_improve", &BATSRetailPriceImproveMsg::m_retail_price_improve)
            .def(self_ns::str(self_ns::self));
}

void export_trade_break_to_python()
{
    class_<BATSTradeBreakMsg, bases<BATSMessageBase>>("BATSTradeBreakMsg")
            .def(init<>())
            .def(init<int, char, uint64_t>())
            .def_readwrite("execId", &BATSTradeBreakMsg::m_execId)
            .def(self_ns::str(self_ns::self));
}

void export_trade_msg_to_python()
{
    class_<BATSTradeMsg, bases<BATSMessageBase>>("BATSTradeMsg")
            .def(init<>())
            .def(init<int, char, uint64_t, char, uint32_t, std::string, uint64_t, uint64_t>())
            .def_readwrite("orderId", &BATSTradeMsg::m_orderId)
            .def_readwrite("side", &BATSTradeMsg::m_side)
            .def_readwrite("shares", &BATSTradeMsg::m_shares)
            .def_readwrite("symbol", &BATSTradeMsg::m_symbol)
            .def_readwrite("price", &BATSTradeMsg::m_price)
            .def_readwrite("execId", &BATSTradeMsg::m_execId)
            .def(self_ns::str(self_ns::self));
}


void export_trading_status_to_python()
{
    class_<BATSTradingStatusMsg, bases<BATSMessageBase>>("BATSTradingStatusMsg")
            .def(init<>())
            .def(init<int, char, std::string, char, uint8_t, char, char>())
            .def_readwrite("symbol", &BATSTradingStatusMsg::m_symbol)
            .def_readwrite("halt_status", &BATSTradingStatusMsg::m_halt_status)
            .def_readwrite("reg_sho_action", &BATSTradingStatusMsg::m_reg_sho_action)
            .def_readwrite("reserved1", &BATSTradingStatusMsg::m_reserved1)
            .def_readwrite("reserved2", &BATSTradingStatusMsg::m_reserved2)
            .def(self_ns::str(self_ns::self));
}

std::ostream& operator<<(std::ostream& out, const BATSAddOrderMsg& o)
{
    out << "BATSAddOrderMsg(timestamp=" << o.m_timestamp << ", msgtype=" << o.m_msgtype
       << ", orderId=" << o.m_orderId << ", side=" << o.m_side
       << ", shares=" << o.m_shares << ", symbol=" << o.m_symbol << ", price="
       << o.m_price << ", display=" << o.m_display << ", partId=" << o.m_partId << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const BATSAuctionSummaryMsg& o)
{   
    out << "BATSAuctionSummaryMsg(timestamp=" << o.m_timestamp << ", msgtype=" << o.m_msgtype << ", symbol=" 
        << o.m_symbol << ", auction_type=" << o.m_auction_type
        << ", price=" << o.m_price << ", shares=" << o.m_shares << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const BATSAuctionUpdateMsg& o)
{
    out << "BATSAuctionUpdateMsg(timestamp=" << o.m_timestamp << ", msgtype=" << o.m_msgtype << ", symbol="
       << o.m_symbol << ", auction_type=" << o.m_auction_type
       << ", reference_price=" << o.m_reference_price << ", buyshares=" << o.m_buyshares << ", sellshares="
       << o.m_sellshares << ", indicative_price=" << o.m_indicative_price
       << ", m_auction_only_price=" << o.m_auction_only_price << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const BATSMessageBase& o)
{
    out << "BATSMessageBase(timestamp=" << o.m_timestamp << ", msgtype=" << o.m_msgtype << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const BATSOrderCancelMsg& o)
{
    out  << "BATSOrderCancelMsg(timestamp=" << o.m_timestamp << ", msgtype=" << o.m_msgtype << ", orderId=" 
        << o.m_orderId << ", shares=" << o.m_shares << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const BATSRetailPriceImproveMsg& o)
{
    out  << "BATSRetailPriceImproveMsg(timestamp=" << o.m_timestamp << ", msgtype=" << o.m_msgtype << ", orderId=" 
        << ", symbol=" << o.m_symbol << ", retail_price_improve" << o.m_retail_price_improve << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const BATSTradeBreakMsg& o)
{
    out << "BATSTradeBreakMsg(timestamp=" << o.m_timestamp << ", msgtype=" << o.m_msgtype
       << ", oexecId=" << o.m_execId << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const BATSTradeMsg& o)
{
    out  << "BATSTradeMsg(timestamp=" << o.m_timestamp << ", msgtype=" << o.m_msgtype << ", orderId=" 
        << o.m_orderId << ", side=" << o.m_side << ", shares=" << o.m_shares << ", symbol=" << o.m_symbol
        << ", price" << o.m_price << ", execId" << o.m_execId << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const BATSTradingStatusMsg& o)
{
    out << "BATSTradingStatusMsg(timestamp=" << o.m_timestamp << ", msgtype=" << o.m_msgtype << ", symbol=" 
        << o.m_symbol << ", halt_status=" << o.m_halt_status
        << ", reg_sho_action=" << o.m_reg_sho_action
        << ", reserved1=" << o.m_reserved1 << ", reserved2="
        << o.m_reserved2 << ")";
    return out;
}

BOOST_PYTHON_MODULE(bats_api)
{
    PyEval_InitThreads(); // need this to mess around with GIL

    export_message_base_to_python(); // the base class needs to be registered first, otherwise python complains
    export_trading_status_to_python();
	export_trade_msg_to_python();
	export_retail_price_improve_to_python();
	export_trade_break_to_python();
	export_order_execute_to_python();
    export_order_cancel_to_python();
    export_auction_update_to_python();
    export_auction_summary_to_python();
    export_add_order_to_python();
    export_msg_parser_to_python();
}
