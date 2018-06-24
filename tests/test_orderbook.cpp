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
#include <chrono>
#include <thread>
#include <boost/test/unit_test.hpp>
#include <boost/dynamic_bitset.hpp>
#include "OrderBook.hpp"
#include "PriceBucketManager.hpp"
#include "PriceBucket.h"
#include "veb.h"
#include "vebBucketSet.hpp"

using namespace std;

BOOST_AUTO_TEST_SUITE( test_orderbook_suite )

    BOOST_AUTO_TEST_CASE( test_simple )
    {
        auto b = LimitOrderBook<PriceBucketManager<>>();

        auto o1 = Order(2001, 10000, 100, BookType::BUY, 20001);
        auto o2 = Order(2002, 10050, 200, BookType::BUY, 20001);
        auto o3 = Order(2003, 10100, 300, BookType::BUY, 20001);
        auto o4 = Order(2004, 10200, 400, BookType::SELL, 20001);
        auto o5 = Order(2005, 10250, 500, BookType::SELL, 20001);
        auto o6 = Order(2006, 10300, 600, BookType::SELL, 20001);

        b.addOrder(o1);
        b.addOrder(o2);
        b.addOrder(o3);
        b.addOrder(o4);
        b.addOrder(o5);
        b.addOrder(o6);

        BOOST_TEST( b.bestBid() == o3.price );
        BOOST_TEST( b.volumeForPricePoint(o1.price, o1.side) == o1.volume );
        BOOST_TEST( b.volumeForPricePoint(o2.price, o2.side) == o2.volume );
        BOOST_TEST( b.volumeForPricePoint(o3.price, o3.side) == o3.volume );

        BOOST_TEST( b.bestAsk() == o4.price );
        BOOST_TEST( b.volumeForPricePoint(o4.price, o4.side) == o4.volume );
        BOOST_TEST( b.volumeForPricePoint(o5.price, o5.side) == o5.volume );
        BOOST_TEST( b.volumeForPricePoint(o6.price, o6.side) == o6.volume );

        b.removeOrder(o3);
        BOOST_TEST( b.bestBid() == o2.price );
        b.removeOrder(o4);
        BOOST_TEST( b.bestAsk() == o5.price );
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

    BOOST_AUTO_TEST_CASE( test_veb_bucket_set )
    {
        vebBucketSet<PriceBucket, 16> s;
        BOOST_TEST( s.find(9999) == nullptr );

        auto p = std::make_shared<PriceBucket>(20);
        s.insert(std::make_pair(20, p));

        auto x = s.find(20);
        BOOST_TEST( x != nullptr );
        BOOST_TEST( x->m_pricePoint == 20 );

        BOOST_TEST( s.successor(20) == nullptr );

        auto p2 = std::make_shared<PriceBucket>(25);
        s.insert(std::make_pair(25, p2));

        BOOST_TEST( s.successor(20)->m_pricePoint == 25 );
        BOOST_TEST( s.predecessor(25)->m_pricePoint == 20 );
        BOOST_TEST( s.minPrice() == 20 );
        BOOST_TEST( s.maxPrice() == 25 );

        s.remove(20);
        BOOST_TEST( s.find(20) == nullptr );        
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
        BOOST_TEST( bucket->totalVolume() == 0 );

        pm.addBucket(5);
        pm.addBucket(20);

        auto b1 = pm.findBucket(5);
        BOOST_TEST( b1->m_pricePoint == 5 );
        BOOST_TEST( b1->totalVolume() == 0 );

        auto b2 = pm.findBucket(10);
        BOOST_TEST( b2->m_pricePoint == 10 );
        BOOST_TEST( b2->totalVolume() == 0 );

        auto b3 = pm.findBucket(20);
        BOOST_TEST( b3->m_pricePoint == 20 );
        BOOST_TEST( b3->totalVolume() == 0 );

        auto b4 = pm.nextBucket(10);
        BOOST_TEST( b3->m_pricePoint == 20 );
        BOOST_TEST( b3->totalVolume() == 0 );

        auto b5 = pm.prevBucket(10);
        BOOST_TEST( b5->m_pricePoint == 5 );
        BOOST_TEST( b5->totalVolume() == 0 );

    }

    BOOST_AUTO_TEST_CASE( test_price_bucket_iter )
    {
        auto pm = PriceBucketManager<>();



        pm.addBucket(10);
        pm.addBucket(5);
        pm.addBucket(20);

        auto it = pm.begin<Bid>();
        BOOST_TEST( it->m_pricePoint == 20 );

        it++;
        BOOST_TEST( it->m_pricePoint == 10 );

        it++;
        BOOST_TEST( it->m_pricePoint == 5 );

        it++;
        bool isEnd = it == pm.end<Bid>();
        BOOST_TEST( isEnd );

        int cnt = 0;
        int arr[] = {20, 10, 5};

//      temporary disable.
//        for ( auto iter : pm ) // test range based for loop.
//        {
//            BOOST_TEST( iter.m_pricePoint == arr[cnt++] );
//        }

        cnt = 0;
        for ( auto iter = pm.begin<Bid>(); iter != pm.end<Bid>(); iter++ ) // test range based for loop.
        {
            BOOST_TEST( iter->m_pricePoint == arr[cnt++] );
        }

    }

    BOOST_AUTO_TEST_CASE( test_book_iter )
    {
        auto buybook  = Book<PriceBucketManager<>, Bid>(BookType::BUY);
        auto sellbook = Book<PriceBucketManager<>, Ask>(BookType::SELL);

        auto o1 = Order(2001, 10000, 100, BookType::BUY, 20001);
        auto o2 = Order(2002, 10050, 200, BookType::BUY, 20001);
        auto o3 = Order(2003, 10100, 300, BookType::BUY, 20001);
        auto o4 = Order(2004, 10200, 400, BookType::SELL, 20001);
        auto o5 = Order(2005, 10250, 500, BookType::SELL, 20001);
        auto o6 = Order(2006, 10300, 600, BookType::SELL, 20001);

        buybook.addOrder(o1);
        buybook.addOrder(o2);
        buybook.addOrder(o3);
        sellbook.addOrder(o4);
        sellbook.addOrder(o5);
        sellbook.addOrder(o6);

        auto it = buybook.begin();
        BOOST_TEST( it->m_pricePoint == 10100 );

        it++;
        BOOST_TEST( it->m_pricePoint == 10050 );

        it++;
        BOOST_TEST( it->m_pricePoint == 10000 );

        it++;
        bool isEnd = it == buybook.end();
        BOOST_TEST( isEnd );

        auto it2 = sellbook.begin();
        BOOST_TEST( it2->m_pricePoint == 10200 );

        it2++;
        BOOST_TEST( it2->m_pricePoint == 10250 );

        it2++;
        BOOST_TEST( it2->m_pricePoint == 10300 );

        it2++;
        isEnd = it2 == sellbook.end();
        BOOST_TEST( isEnd );

    }

    BOOST_AUTO_TEST_CASE( test_lob_iter )
    {
        auto b = LimitOrderBook<PriceBucketManager<>>();

        auto o1 = Order(2001, 10000, 100, BookType::BUY, 20001);
        auto o2 = Order(2002, 10050, 200, BookType::BUY, 20001);
        auto o3 = Order(2003, 10100, 300, BookType::BUY, 20001);
        auto o4 = Order(2004, 10200, 400, BookType::SELL, 20001);
        auto o5 = Order(2005, 10250, 500, BookType::SELL, 20001);
        auto o6 = Order(2006, 10300, 600, BookType::SELL, 20001);

        b.addOrder(o1);
        b.addOrder(o2);
        b.addOrder(o3);
        b.addOrder(o4);
        b.addOrder(o5);
        b.addOrder(o6);

        auto it = b.bids_begin();
        BOOST_TEST( it->m_pricePoint == 10100 );

        it++;
        BOOST_TEST( it->m_pricePoint == 10050 );

        it++;
        BOOST_TEST( it->m_pricePoint == 10000 );

        it++;
        bool isEnd = it == b.bids_end();
        BOOST_TEST( isEnd );

        auto it2 = b.asks_begin();
        BOOST_TEST( it2->m_pricePoint == 10200 );

        it2++;
        BOOST_TEST( it2->m_pricePoint == 10250 );

        it2++;
        BOOST_TEST( it2->m_pricePoint == 10300 );

        it2++;
        isEnd = it2 == b.asks_end();
        BOOST_TEST( isEnd );
    }

    BOOST_AUTO_TEST_CASE( test_lob_cross_spread )
    {
        auto b = LimitOrderBook<PriceBucketManager<>>();

        auto o1 = Order(2001, 10200, 400, BookType::SELL, 20001);
        auto o2 = Order(2002, 10250, 500, BookType::SELL, 20001);
        auto o3 = Order(2003, 10300, 600, BookType::SELL, 20001);

        b.addOrder(o1);
        b.addOrder(o2);
        b.addOrder(o3);

        auto o4 = Order(2004, 10225, 300, BookType::BUY, 20001);
        b.addOrder(o4);

        BOOST_TEST( b.volumeForPricePoint(10200, BookType::SELL) == 100 );

        auto o5 = Order(2005, 10000, 100, BookType::BUY, 20001);
        auto o6 = Order(2006, 10050, 200, BookType::BUY, 20001);
        auto o7 = Order(2007, 10100, 300, BookType::BUY, 20001);

        b.addOrder(o5);
        b.addOrder(o6);
        b.addOrder(o7);

        auto o8 = Order(2008, 10075, 150, BookType::SELL, 20001);
        b.addOrder(o8);

        BOOST_TEST( b.volumeForPricePoint(10100, BookType::BUY) == 150 );
    }

    BOOST_AUTO_TEST_CASE( test_lob_threading )
    {
        auto b = LimitOrderBook<PriceBucketManager<>>();
        b.startWorkers();

        auto start = std::chrono::high_resolution_clock::now();

        int rep = 4;
        int n_orders = 30000;
        for (int k = 0; k < rep; k++) {
            for (int i = 0; i < n_orders; i++) {
                auto o = Order(200000 + i, 100000 + i,  100+i, BookType::SELL, 20001);
                b.queueOrder(o);
                //std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
        }

        while(!b.emptyRequestQueue()) {} //heuristic wait for things to finish.

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::micro> fp_ms = end - start;

        std::this_thread::sleep_for(std::chrono::seconds(5));

        b.shutDown();

        BOOST_TEST_MESSAGE( "Time taken : " << fp_ms.count() << " us.");
        BOOST_TEST_MESSAGE( "Time per msg : " << fp_ms.count() / (rep * n_orders) << " us.");

        for (int i = 0; i < n_orders; i++)
        {
            BOOST_TEST(b.volumeForPricePoint(100000+i, BookType::SELL) == (100+i)*rep);
        }


    }

BOOST_AUTO_TEST_SUITE_END()