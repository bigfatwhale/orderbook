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
#include <map>
#include <utility>
#include <boost/test/unit_test.hpp>
#include <boost/dynamic_bitset.hpp>
#include "OrderBook.h"
#include "PriceBucketManager.hpp"
#include "veb.h"

using namespace std;

BOOST_AUTO_TEST_SUITE( test_orderbook_suite )

    BOOST_AUTO_TEST_CASE( test_simple )
    {
        auto b = LimitOrderBook<PriceBucketManager<>>();

        auto o  = Order(2001, 10000, 100, BookType::BUY, "Acme Corp.");
        auto o2 = Order(2001, 10050, 200, BookType::BUY, "Acme Corp.");
        auto o3 = Order(2001, 10100, 300, BookType::BUY, "Acme Corp.");

        b.addOrder(o);
        b.addOrder(o2);
        b.addOrder(o3);
        BOOST_TEST( b.bestBid() == 10000 );
        BOOST_TEST( b.volumeForPricePoint(10000, o.side) == 100 );
        BOOST_TEST( b.volumeForPricePoint(10050, o.side) == 200 );
        BOOST_TEST( b.volumeForPricePoint(10100, o.side) == 300 );

        b.removeOrder(o);
        BOOST_TEST( b.volumeForPricePoint(10000, o.side) == 0 );
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

    BOOST_AUTO_TEST_CASE( test_veb_edge_cases )
    {
        veb v{5};
        BOOST_TEST( v.isEmpty() );
        BOOST_TEST( v.isMember(10) == false );
        BOOST_TEST( v.successor(0) == -1 );

        v.insert(0);
        BOOST_TEST( v.findMin() == 0 );
        v.insert(31);
        BOOST_TEST( v.successor(0) == 31 );
        BOOST_TEST( v.predecessor(31) == 0 );
        v.remove(0);
        BOOST_TEST( v.findMin() == 31 );
        v.insert(0);
        BOOST_TEST( v.findMin() == 0 );
        v.remove(31);
        BOOST_TEST( v.findMin() == 0 );
        v.remove(0);
        BOOST_TEST( v.isEmpty() );

    }

        BOOST_AUTO_TEST_CASE( test_veb_exhaustive )
    {
        auto runall = [](int numItems, int universe_size)
        {
            // use the 1s and 0s of a 11-bit bitset to model 11 items that
            // exists/not-exist in the set. dump it into the veb, then test
            // that all the isMember, successor, and predecessor calls work.
            // in the above, universe_size is the number of bits required.
            int bits = numItems;

            for (int j=1; j < 1 << bits; j++)
            {
                veb v{universe_size};
                boost::dynamic_bitset<> b(bits, j);
                deque<int> items;

                //cout << "bits = " << b << endl;
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
                    if (s != -1)
                        assert( s == x );
                    p = s;
                }

            }
            return true;
        };
        BOOST_TEST( runall(16, 4) == true ); // 16 items representable with 4-bit veb
        BOOST_TEST( runall(32, 5) == true ); // 32 items representable with 5-bit veb
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


    BOOST_AUTO_TEST_CASE( test_price_bucket_manager )
    {
        auto pm = PriceBucketManager<>();

        auto bucket = pm.addBucket(10);

        BOOST_TEST( bucket->m_pricePoint == 10 );
        BOOST_TEST( bucket->m_volume == 0 );

        pm.addBucket(5);
        pm.addBucket(20);

        auto b1 = pm.findBucket(5);
        BOOST_TEST( b1->m_pricePoint == 5 );
        BOOST_TEST( b1->m_volume == 0 );

        auto b2 = pm.findBucket(10);
        BOOST_TEST( b2->m_pricePoint == 10 );
        BOOST_TEST( b2->m_volume == 0 );

        auto b3 = pm.findBucket(20);
        BOOST_TEST( b3->m_pricePoint == 20 );
        BOOST_TEST( b3->m_volume == 0 );

        auto b4 = pm.nextBucket(10);
        BOOST_TEST( b3->m_pricePoint == 20 );
        BOOST_TEST( b3->m_volume == 0 );

        auto b5 = pm.prevBucket(10);
        BOOST_TEST( b5->m_pricePoint == 5 );
        BOOST_TEST( b5->m_volume == 0 );

    }

BOOST_AUTO_TEST_SUITE_END()