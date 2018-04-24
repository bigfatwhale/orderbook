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
        int k = 100;
        veb v{5}; // universe of 2^5 = 32 possible integer values
        v.insert(19);
        BOOST_TEST( v.isMember(19) );

        boost::dynamic_bitset<> b(5, 7);
        cout << "bit string " << b << endl;

        BOOST_TEST( v.isMember(7)==false );
        cout << k << endl;
    }

BOOST_AUTO_TEST_SUITE_END()