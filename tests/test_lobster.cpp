//
// Created by Uncle Chu on 11/5/18.
//

#define BOOST_TEST_MODULE test_orderbook
#define BOOST_TEST_DYN_LINK

#include <set>
#include <string>
#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include "AddOrderMsg.hpp"
#include "CancelOrderMsg.hpp"
#include "MsgParser.h"
#include "DeleteOrderMsg.hpp"
#include "OrderExecutedMsg.hpp"
#include "AuctionTradeMsg.hpp"
#include "TradeHaltMsg.hpp"
#include "PriceBucketManager.hpp"
#include "PriceBucket.h"
#include "vebBucketSet.hpp"
#include "OrderBook.hpp"
#include "Order.h"

using namespace std;
using namespace lobster;

using boost::iostreams::mapped_file_source;
using boost::iostreams::stream;

using tokenizer = boost::tokenizer<boost::char_separator<char>>;

class TestPriceBucket : public PriceBucket
{
public:
    TestPriceBucket() : PriceBucket() {}
    TestPriceBucket(uint64_t pricePoint) : PriceBucket(pricePoint) {}
    void removeOrder(Order const &order) override
    {
        if ( order.orderId == 0 ) {
            // TODO : only adding this for passing lobster load. rework it out later.
            auto it = std::find_if(m_orders.begin(), m_orders.end(), [](Order &o) { return o.orderId == 0; });
            if (it != m_orders.end()) {
                if (it->volume == order.volume)
                    m_orders.erase(it);
                else
                    it->volume -= order.volume;
            }
        }
        else
        {
            auto f = [&order]( Order &o1 ) { return o1.orderId == order.orderId; };
            auto it = std::find_if( m_orders.begin(), m_orders.end(), f);
            if ( it != m_orders.end() )
                m_orders.erase(it);
        }
    }
};

class LOBSim : public LimitOrderBook<
        PriceBucketManager< default_bucket_set<TestPriceBucket>, TestPriceBucket> >
{};

class LOBSimVeb : public LimitOrderBook<
        PriceBucketManager< vebBucketSet<TestPriceBucket, 25                                                    >, TestPriceBucket> >
{};

template<typename T>
void do_sample_load( T &orderbook )
{
    auto parser = lobster::MsgParser();
    
    string msgline, cur_order_line, prev_order_line;

    mapped_file_source msgfile("AAPL_2012-06-21_message_50.csv");
    mapped_file_source orderfile("AAPL_2012-06-21_orderbook_50.csv");

    stream<mapped_file_source> msg_ifs(msgfile, std::ios::binary);
    stream<mapped_file_source> order_ifs(orderfile, std::ios::binary);

    BOOST_TEST(msgfile.is_open());
    BOOST_TEST(orderfile.is_open());

    getline(order_ifs, prev_order_line);
    getline(msg_ifs, msgline); // skip first line of the message file.

    // take the first line from the order file, and initialize the orderbook with it.
    boost::char_separator<char> sep(",");
    tokenizer tokens(prev_order_line, sep);

    int c = 0;

    auto it = tokens.begin();
    while (it != tokens.end())
    {
        auto price  = boost::lexical_cast<uint64_t>(*it++);
        auto volume = boost::lexical_cast<uint64_t>(*it++);
        auto o = Order(0, price, volume, ( c % 2 == 0 ) ? BookType::SELL : BookType::BUY, 50001);
        orderbook.addOrder(o);
        c++;
    }

    set<uint32_t> added_price_levels;
    set<uint32_t> added_order_ids;

    while (getline(msg_ifs, msgline))
    {
        // process one line of message file, apply the actions onto the orderbook
        auto msg = parser.parse_msg(msgline);

        getline(order_ifs, cur_order_line);

        if ( msg->m_msgtype == '1' )
        {
            //uint64_t id, uint64_t price, uint32_t volume, BookType side, std::string const &partId
            auto dmsg = dynamic_pointer_cast<AddOrderMsg>(msg);
            Order o(dmsg->m_orderId, dmsg->m_price, dmsg->m_shares,
                    ( dmsg->m_side == 1 ) ? BookType::BUY : BookType::SELL, 300001);
            orderbook.addOrder(o);
            added_price_levels.insert(o.price);
            added_order_ids.insert(o.orderId);
        }
        else if (msg->m_msgtype == '2')
        {
            auto dmsg = dynamic_pointer_cast<CancelOrderMsg>(msg);
            Order o(dmsg->m_orderId, dmsg->m_price, dmsg->m_shares,
                    ( dmsg->m_side == 1 ) ? BookType::BUY : BookType::SELL, 300001);
            orderbook.removeOrder(o);
        }
        else if (msg->m_msgtype == '3')
        {
            auto dmsg = dynamic_pointer_cast<DeleteOrderMsg>(msg);

            uint32_t oid;
            if ( added_order_ids.find(dmsg->m_orderId) != added_order_ids.end())
                oid = dmsg->m_orderId;
            else
                oid = 0;
            Order o(oid, dmsg->m_price, dmsg->m_shares,
                    ( dmsg->m_side == 1 ) ? BookType::BUY : BookType::SELL, 300001);
            orderbook.removeOrder(o);
        }
        else if (msg->m_msgtype == '4')
        {
            // execution msg. this is a msg transmitted out because some order filled the exchange's
            // LOB and the exchange generated a msg to send to us. to process this in our local instance
            // of the LOB, we need to artificially create an appropriate buy/sell order on the opposite side
            // of the book. This order should immediately get filled by our orderbook and our orderbook state
            // will then match what's specified in the cur_order_line.
            auto dmsg = dynamic_pointer_cast<OrderExecutedMsg>(msg);
            if ( dmsg->m_side == 1 )
            {
                // if it's a execution in the buy book, then we need to generate a sell order, and vice versa.
                Order o(999999999, dmsg->m_price, dmsg->m_shares, BookType::SELL, 300001);
                orderbook.addOrder(o);
            }
            else
            {
                Order o(999999999, dmsg->m_price, dmsg->m_shares, BookType::BUY, 300001);
                orderbook.addOrder(o);
            }

        }
        // msgtype = '5' is to be ignored.

        // now take the corresponding line in the order book file. check that our order book
        // state is the same as what is given by the order book file line.

        tokens = tokenizer (cur_order_line, sep);

        c = 0;
        auto it = tokens.begin();
        while (it != tokens.end())
        {
            auto price = boost::lexical_cast<uint64_t>(*it++);
            auto volume = boost::lexical_cast<uint64_t>(*it++);

            if (c==0)
                BOOST_TEST( orderbook.bestAsk() == price);
            if (c==1)
                BOOST_TEST( orderbook.bestBid() == price);

            if (added_price_levels.find(price) != added_price_levels.end() )
            {
                auto type = (c % 2 == 0) ? BookType::SELL : BookType::BUY;

                auto orderbook_volume = orderbook.volumeForPricePoint(price, type);

                BOOST_TEST(orderbook_volume == volume);

                if (orderbook_volume != volume)
                    throw runtime_error("volume mismatch");

            }
            c++;
        }
    }
    msg_ifs.close();
    msgfile.close();
}

BOOST_AUTO_TEST_SUITE( test_lobster_suite )

    BOOST_AUTO_TEST_CASE( test_add_order )
    {
        auto parser = lobster::MsgParser();

        string data = "34200.18960767,1,11885113,21,2238100,1";
        
        auto msg = dynamic_pointer_cast<AddOrderMsg>(parser.parse_msg(data));
        BOOST_TEST( msg->m_timestamp.tv_sec == 34200);
        BOOST_TEST( msg->m_timestamp.tv_nsec == 18960767);
        BOOST_TEST( msg->m_msgtype == '1');
        BOOST_TEST( msg->m_orderId == 11885113);
        BOOST_TEST( msg->m_shares == 21);
        BOOST_TEST( msg->m_price == 2238100);
        BOOST_TEST( msg->m_side == 1);

    }

    BOOST_AUTO_TEST_CASE( test_cancel_order )
    {
        auto parser = lobster::MsgParser();

        string data = "34409.326018975,2,21866417,200,2239600,-1";

        auto msg = dynamic_pointer_cast<CancelOrderMsg>(parser.parse_msg(data));
        BOOST_TEST( msg->m_timestamp.tv_sec == 34409);
        BOOST_TEST( msg->m_timestamp.tv_nsec == 326018975);
        BOOST_TEST( msg->m_msgtype == '2');
        BOOST_TEST( msg->m_orderId == 21866417);
        BOOST_TEST( msg->m_shares == 200);
        BOOST_TEST( msg->m_price == 2239600);
        BOOST_TEST( msg->m_side == -1);

    }

    BOOST_AUTO_TEST_CASE( test_delete_order )
    {
        auto parser = lobster::MsgParser();

        string data = "34395.851552647,3,21138842,200,2239800,-1";

        auto msg = dynamic_pointer_cast<DeleteOrderMsg>(parser.parse_msg(data));
        BOOST_TEST( msg->m_timestamp.tv_sec == 34395);
        BOOST_TEST( msg->m_timestamp.tv_nsec == 851552647);
        BOOST_TEST( msg->m_msgtype == '3');
        BOOST_TEST( msg->m_orderId == 21138842);
        BOOST_TEST( msg->m_shares == 200);
        BOOST_TEST( msg->m_price == 2239800);
        BOOST_TEST( msg->m_side == -1);

    }

    BOOST_AUTO_TEST_CASE( test_order_executed )
    {
        auto parser = lobster::MsgParser();

        string data = "34414.765462735,4,21942984,100,2237600,1";

        auto msg = dynamic_pointer_cast<OrderExecutedMsg>(parser.parse_msg(data));
        BOOST_TEST( msg->m_timestamp.tv_sec == 34414);
        BOOST_TEST( msg->m_timestamp.tv_nsec == 765462735);
        BOOST_TEST( msg->m_msgtype == '4');
        BOOST_TEST( msg->m_orderId == 21942984);
        BOOST_TEST( msg->m_shares == 100);
        BOOST_TEST( msg->m_price == 2237600);
        BOOST_TEST( msg->m_side == 1);
        BOOST_TEST( msg->m_visible == true);


        string data2 = "34418.450176448,5,0,100,2238600,-1";
        auto msg2 = dynamic_pointer_cast<OrderExecutedMsg>(parser.parse_msg(data2));
        BOOST_TEST( msg2->m_timestamp.tv_sec == 34418);
        BOOST_TEST( msg2->m_timestamp.tv_nsec == 450176448);
        BOOST_TEST( msg2->m_msgtype == '5');
        BOOST_TEST( msg2->m_orderId == 0);
        BOOST_TEST( msg2->m_shares == 100);
        BOOST_TEST( msg2->m_price == 2238600);
        BOOST_TEST( msg2->m_side == -1);
        BOOST_TEST( msg2->m_visible == false);
    }

    BOOST_AUTO_TEST_CASE( test_auction_trade )
    {
        auto parser = lobster::MsgParser();

        string data = "34414.765462735,6,-2,100,2237600,1";
        auto msg = dynamic_pointer_cast<AuctionTradeMsg>(parser.parse_msg(data));
        BOOST_TEST( msg->m_timestamp.tv_sec == 34414);
        BOOST_TEST( msg->m_timestamp.tv_nsec == 765462735);
        BOOST_TEST( msg->m_msgtype == '6');
        BOOST_TEST( msg->m_orderId == -2);
        BOOST_TEST( msg->m_shares == 100);
        BOOST_TEST( msg->m_price == 2237600);
        BOOST_TEST( msg->m_side == 1);
    }

    BOOST_AUTO_TEST_CASE( test_trade_halt )
    {
        auto parser = lobster::MsgParser();

        string data = "34414.765462735,7,0,0,-1,-1";
        auto msg = dynamic_pointer_cast<TradeHaltMsg>(parser.parse_msg(data));
        BOOST_TEST( msg->m_timestamp.tv_sec == 34414);
        BOOST_TEST( msg->m_timestamp.tv_nsec == 765462735);
        BOOST_TEST( msg->m_msgtype == '7');
        BOOST_TEST( msg->m_price == -1);
    }

    BOOST_AUTO_TEST_CASE( test_lobster_sample_load_default )
    {
        auto orderbook = LOBSim();
        do_sample_load(orderbook);
    }

    BOOST_AUTO_TEST_CASE( test_lobster_sample_load_veb )
    {
        auto orderbook = LOBSimVeb();
        do_sample_load(orderbook);
    }

    BOOST_AUTO_TEST_CASE( test_lobster_sample_load_time )
    {   
        // same as test_lobster_sample_load, but no test assertions. just want the timing.
        auto parser = lobster::MsgParser();
        auto orderbook = LOBSim();
        string msgline, cur_order_line, prev_order_line;

        mapped_file_source msgfile("AAPL_2012-06-21_message_50.csv");
        mapped_file_source orderfile("AAPL_2012-06-21_orderbook_50.csv");

        stream<mapped_file_source> msg_ifs(msgfile, std::ios::binary);
        stream<mapped_file_source> order_ifs(orderfile, std::ios::binary);

        getline(order_ifs, prev_order_line);
        getline(msg_ifs, msgline); // skip first line of the message file.

        // take the first line from the order file, and initialize the orderbook with it.
        boost::char_separator<char> sep(",");
        tokenizer tokens(prev_order_line, sep);

        int c = 0;

        auto it = tokens.begin();
        while (it != tokens.end())
        {
            auto price = boost::lexical_cast<uint64_t>(*it++);
            auto volume = boost::lexical_cast<uint64_t>(*it++);
            auto o = Order(0, price, volume, ( c % 2 == 0 ) ? BookType::SELL : BookType::BUY, 300001);
            orderbook.addOrder(o);
            c++;
        }

        set<uint32_t> added_price_levels;
        set<uint32_t> added_order_ids;

        while (getline(msg_ifs, msgline))
        {
            // process one line of message file, apply the actions onto the orderbook
            auto msg = parser.parse_msg(msgline);

            getline(order_ifs, cur_order_line);

            if ( msg->m_msgtype == '1' )
            {
                //uint64_t id, uint64_t price, uint32_t volume, BookType side, std::string const &partId
                auto dmsg = dynamic_pointer_cast<AddOrderMsg>(msg);
                Order o(dmsg->m_orderId, dmsg->m_price, dmsg->m_shares,
                        ( dmsg->m_side == 1 ) ? BookType::BUY : BookType::SELL, 300001);
                orderbook.addOrder(o);
                added_price_levels.insert(o.price);
                added_order_ids.insert(o.orderId);
            }
            else if (msg->m_msgtype == '2')
            {
                auto dmsg = dynamic_pointer_cast<CancelOrderMsg>(msg);
                Order o(dmsg->m_orderId, dmsg->m_price, dmsg->m_shares,
                        ( dmsg->m_side == 1 ) ? BookType::BUY : BookType::SELL, 300001);
                orderbook.removeOrder(o);
            }
            else if (msg->m_msgtype == '3')
            {
                auto dmsg = dynamic_pointer_cast<DeleteOrderMsg>(msg);

                uint32_t oid;
                if ( added_order_ids.find(dmsg->m_orderId) != added_order_ids.end())
                    oid = dmsg->m_orderId;
                else
                    oid = 0;
                Order o(oid, dmsg->m_price, dmsg->m_shares,
                        ( dmsg->m_side == 1 ) ? BookType::BUY : BookType::SELL, 300001);
                orderbook.removeOrder(o);
            }
            else if (msg->m_msgtype == '4')
            {
                // execution msg. this is a msg transmitted out because some order filled the exchange's
                // LOB and the exchange generated a msg to send to us. to process this in our local instance
                // of the LOB, we need to artificially create an appropriate buy/sell order on the opposite side
                // of the book. This order should immediately get filled by our orderbook and our orderbook state
                // will then match what's specified in the cur_order_line.
                auto dmsg = dynamic_pointer_cast<OrderExecutedMsg>(msg);
                if ( dmsg->m_side == 1 )
                {
                    // if it's a execution in the buy book, then we need to generate a sell order, and vice versa.
                    Order o(999999999, dmsg->m_price, dmsg->m_shares, BookType::SELL, 300001);
                    orderbook.addOrder(o);
                }
                else
                {
                    Order o(999999999, dmsg->m_price, dmsg->m_shares, BookType::BUY, 300001);
                    orderbook.addOrder(o);
                }

            }
        }
        msg_ifs.close();
        msgfile.close();
    }


BOOST_AUTO_TEST_SUITE_END()