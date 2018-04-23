//
// Created by Uncle Chu on 21/4/18.
//


#define BOOST_TEST_MODULE test_orderbook
#define BOOST_TEST_DYN_LINK

#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/dynamic_bitset.hpp>
#include "OrderBook.h"
#include "veb.h"

using namespace std;

BOOST_AUTO_TEST_SUITE( test_parse_suite )

    BOOST_AUTO_TEST_CASE( test_simple )
    {
        int price = 51234;
        auto b = LimitOrderBook();

        auto o = Order(2001, price, 100, BookType::BUY, "Acme Corp.");

        b.addOrder(o);
        BOOST_TEST( b.bestBid() == price );
        BOOST_TEST( b.volumeForPricePoint(price, o.side) == 100 );

        b.removeOrder(o);
        BOOST_TEST( b.volumeForPricePoint(price, o.side) == 0 );

    }

    BOOST_AUTO_TEST_CASE( test_veb )
    {
//        int n = 10000;
//        boost::dynamic_bitset<> x(n);
//        x[100] = 1;
//
//        cout << x[99] << x[100] << endl;

        veb v{18};
        int k = 200;
        //cout << v.m_clusters.size() << endl;
    }

BOOST_AUTO_TEST_SUITE_END()