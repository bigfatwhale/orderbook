
#include "Python.h"
#include "Order.h"
#include "OrderBook.hpp"
#include "PriceBucket.h"
#include <ostream>
#include <string>
#include <boost/python.hpp>

using namespace boost::python;

void export_orderbook()
{

	class_< LimitOrderBook<> >("LimitOrderBook")
		.def(init<>())
		.def("addOrder",            &LimitOrderBook<>::addOrder )
		.def("removeOrder",         &LimitOrderBook<>::removeOrder )
		.def("volumeForPricePoint", &LimitOrderBook<>::volumeForPricePoint )
		.def("bestBid",             &LimitOrderBook<>::bestBid )
		.def("bestAsk",             &LimitOrderBook<>::bestAsk )
		.add_property("bids", range(&LimitOrderBook<>::bids_begin, &LimitOrderBook<>::bids_end))
		.add_property("asks", range(&LimitOrderBook<>::asks_begin, &LimitOrderBook<>::asks_end))
		;

}

void export_booktype()
{
	enum_<BookType>("BookType")
        .value("BUY",  BookType::BUY)
        .value("SELL", BookType::SELL)
        ;
}

void export_price_bucket()
{
	class_<PriceBucket>("PriceBucket")
		.def(init<>())
		.def(init<uint64_t>())
		.def("__iter__", range(&PriceBucket::begin, &PriceBucket::end))
		.def_readwrite("pricePoint", &PriceBucket::m_pricePoint)
		;
}


void export_order()
{
	class_<Order>("Order")
		.def(init<>())
		.def(init<uint64_t, uint64_t, uint32_t, BookType, std::string>())
		.def_readwrite("orderId", &Order::orderId)
        .def_readwrite("price",   &Order::price)
        .def_readwrite("volume",  &Order::volume)
        .def_readwrite("side",    &Order::side)
        .def_readwrite("partId",  &Order::partId)
        ;
}

BOOST_PYTHON_MODULE(orderbook_api)
{
    PyEval_InitThreads(); // need this to mess around with GIL

	export_orderbook();
	export_order();
	export_booktype();
	export_price_bucket();
}