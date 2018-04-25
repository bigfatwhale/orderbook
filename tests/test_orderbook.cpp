//
// Created by Uncle Chu on 21/4/18.
//


#define BOOST_TEST_MODULE test_orderbook
#define BOOST_TEST_DYN_LINK

#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <deque>
#include <boost/test/unit_test.hpp>
#include <boost/dynamic_bitset.hpp>
#include "OrderBook.h"
#include "veb.h"

using namespace std;

BOOST_AUTO_TEST_SUITE( test_orderbook_suite )

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
        veb v{5}; // universe of 2^5 = 32 possible integer values
        v.insert(19);
        v.insert(5);
        v.insert(30);
        BOOST_TEST( v.isMember(19) );
        BOOST_TEST( v.isMember(7) == false );

        // some simple successor queries
        BOOST_TEST( v.successor(5) == 19);
        BOOST_TEST( v.successor(19) == 30);

        BOOST_TEST( v.predecessor(30) == 19);
        BOOST_TEST( v.predecessor(19) == 5);

        v.remove(19);
        BOOST_TEST( v.isMember(19) == false );
        BOOST_TEST( v.isMember(5) );
        BOOST_TEST( v.isMember(30) );

        v.remove(30);
        BOOST_TEST( v.isMember(5) );
        BOOST_TEST( v.isMember(30) == false );

        v.remove(5);
        BOOST_TEST( v.isMember(5) == false );


    }

    BOOST_AUTO_TEST_CASE( test_veb_exhaustive )
    {
        auto runall = []()
        {
            int bits = 11;

            for (int j=1; j < 1 << bits; j++)
            {
                veb v{bits};
                boost::dynamic_bitset<> b(bits, j);
                deque<int> items;

                cout << "b="<< b << endl;

                auto p = b.find_first();

                // populate the veb
                v.insert(p);
                items.push_front(p);
                while ( true )
                {
                    p = b.find_next(p);
                    if (p != boost::dynamic_bitset<>::npos )
                    {
                        v.insert(p);
                        items.push_front(p);
                    }
                    else
                        break;
                }

                // now check the successor calls
                p = b.find_first();
                assert( v.isMember(p) );

                while ( true )
                {
                    auto s = b.find_next(p);
                    if (s != boost::dynamic_bitset<>::npos )
                    {
                        assert( v.successor(p) == s );
                        assert( v.isMember(s) );
                        p = s;
                    }
                    else
                        break;
                }

                // now check the predecessor calls
                p = items.front();
                items.pop_front();
                for ( auto x : items )
                {
                    auto s = v.predecessor(p);
                    cout << "p=" << p << " s="<< s << " x=" << x << endl;
                    if (s != -1)
                        assert( s == x );
                    p = s;
                }

            }


            return true;
        };
        BOOST_TEST( runall() == true );
    }

    BOOST_AUTO_TEST_CASE( test_veb_fail1 )
    {
        // one single failure case observed in previous testing
        veb v{5};

        v.insert(0);
        v.insert(1);
        v.insert(2);
        v.insert(3);
        v.insert(4);

        BOOST_TEST( v.isMember(0) );
        BOOST_TEST( v.isMember(1) );
        BOOST_TEST( v.isMember(2) );
        BOOST_TEST( v.isMember(3) );
        BOOST_TEST( v.isMember(4) );

        BOOST_TEST( v.predecessor(4) == 3 );
    }

BOOST_AUTO_TEST_SUITE_END()