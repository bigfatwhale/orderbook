//
// Created by Uncle Chu on 21/4/18.
//


#define BOOST_TEST_MODULE test_orderbook
#define BOOST_TEST_DYN_LINK

#include <memory>
#include <string>
#include <fstream>
#include <boost/test/unit_test.hpp>
#include "OrderBook.hpp"

BOOST_AUTO_TEST_SUITE( test_parse_suite )

    BOOST_AUTO_TEST_CASE( test_simple )
    {
        auto b = LimitOrderBook();
        auto o = Order(2001, 51234, 100, BookType::BUY, "Acme Corp.");

        b.addOrder(o);
        BOOST_TEST( b.bestBid() == 51234 );
    }

BOOST_AUTO_TEST_SUITE_END()