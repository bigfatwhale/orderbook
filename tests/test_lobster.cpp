//
// Created by Uncle Chu on 11/5/18.
//

#define BOOST_TEST_MODULE test_orderbook
#define BOOST_TEST_DYN_LINK

#include <string>
#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include "../lobster/AddOrderMsg.hpp"
#include "../lobster/CancelOrderMsg.hpp"
#include "../lobster/MsgParser.h"
#include "../lobster/DeleteOrderMsg.hpp"
#include "../lobster/OrderExecutedMsg.hpp"
#include "../lobster/AuctionTradeMsg.hpp"
#include "../lobster/TradeHaltMsg.hpp"
#include "OrderBook.hpp"
#include "Order.h"

using namespace std;
using namespace lobster;

using boost::iostreams::mapped_file_source;
using boost::iostreams::stream;

using tokenizer = boost::tokenizer<boost::char_separator<char>>;


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

    BOOST_AUTO_TEST_CASE( test_lobster_sample_load )
    {
        auto parser = lobster::MsgParser();
        auto orderbook = LimitOrderBook<>();
        string msgline, cur_order_line, prev_order_line;

        mapped_file_source msgfile("AAPL_2012-06-21_message_5.csv");
        mapped_file_source orderfile("AAPL_2012-06-21_orderbook_5.csv");

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
            auto price = boost::lexical_cast<int>(*it++);
            auto volume = boost::lexical_cast<int>(*it++);
            auto o = Order(0, price, volume, ( c % 2 == 0 ) ? BookType::SELL : BookType::BUY, "");
            orderbook.addOrder(o);
            c++;
        }

        // below is the first row of the order book data file.
        // 5859400,200,5853300,18,  5859800,200,5853000,150,  5861000,200,5851000,5,
        // 5868900,300,5850100,89,  5869500,50,5849700,5
        BOOST_TEST(orderbook.volumeForPricePoint(5859400, BookType::SELL) == 200);
        BOOST_TEST(orderbook.volumeForPricePoint(5853300, BookType::BUY) == 18);
        BOOST_TEST(orderbook.volumeForPricePoint(5859800, BookType::SELL) == 200);
        BOOST_TEST(orderbook.volumeForPricePoint(5853000, BookType::BUY) == 150);
        BOOST_TEST(orderbook.volumeForPricePoint(5861000, BookType::SELL) == 200);
        BOOST_TEST(orderbook.volumeForPricePoint(5851000, BookType::BUY) == 5);
        BOOST_TEST(orderbook.volumeForPricePoint(5868900, BookType::SELL) == 300);
        BOOST_TEST(orderbook.volumeForPricePoint(5850100, BookType::BUY) == 89);
        BOOST_TEST(orderbook.volumeForPricePoint(5869500, BookType::SELL) == 50);
        BOOST_TEST(orderbook.volumeForPricePoint(5849700, BookType::BUY) == 5);

        while (getline(msg_ifs, msgline))
        {
            // process one line of message file, apply the actions onto the orderbook
            auto msg = parser.parse_msg(msgline);

            if ( msg->m_msgtype == '1' )
            {
                //uint64_t id, uint64_t price, uint32_t volume, BookType side, std::string const &partId
                auto dmsg = dynamic_pointer_cast<AddOrderMsg>(msg);
                Order o(dmsg->m_orderId, dmsg->m_price, dmsg->m_shares,
                        dmsg->m_side == 1 ? BookType::BUY : BookType::SELL, "NCM");
                orderbook.addOrder(o);
            }

            // now take the corresponding line in the order book file. check that our order book
            // state is the same as what is given by the order book file line.
            getline(order_ifs, cur_order_line);
            tokens = tokenizer (cur_order_line, sep);

            int c = 0;
            auto it = tokens.begin();
            while (it != tokens.end())
            {
                auto price = boost::lexical_cast<int>(*it++);
                auto volume = boost::lexical_cast<int>(*it++);

                auto type = ( c % 2 == 0 ) ? BookType::SELL : BookType::BUY;

                cout << "current msg line=" << msgline << endl;
                cout << "current order line=" << cur_order_line << endl;
                cout << "testing volume for price=" << price << " is " << volume << endl;

                auto orderbook_volume = orderbook.volumeForPricePoint(price, type);
                cout << ". Orderbook has volume=" << orderbook_volume << endl;

                BOOST_TEST(orderbook.volumeForPricePoint(price, type) == volume);

                c++;
            }

        }
        msg_ifs.close();
        msgfile.close();
    }

BOOST_AUTO_TEST_SUITE_END()