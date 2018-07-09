
#define BOOST_TEST_MODULE test_algoseek
#define BOOST_TEST_DYN_LINK

#include "algoseek/AddOrderMsg.hpp"
#include "algoseek/OrderExecutedMsg.hpp"
#include "algoseek/MsgParser.h"
#include <boost/test/unit_test.hpp>
#include <string>

using namespace std;
using namespace algoseek;

BOOST_AUTO_TEST_SUITE( test_lobster_suite )

    BOOST_AUTO_TEST_CASE( test_add_order )
    {
        auto parser = algoseek::MsgParser("20140128");

        string data = "04:00:00.512,2251812698588658,ADD BID,IBM,176.33,400,ARCA,ARCA";

        auto msg = dynamic_pointer_cast<AddOrderMsg>(parser.parse_msg(data));

        BOOST_TEST( msg->m_timestamp.tv_sec  == 1390867200);
        BOOST_TEST( msg->m_timestamp.tv_nsec == 14400512000000);
        BOOST_TEST( msg->m_msgtype           == 'B');
        BOOST_TEST( msg->m_orderId           == 2251812698588658);
        BOOST_TEST( msg->m_price             == 1763300);
        BOOST_TEST( msg->m_shares            == 400);
        BOOST_TEST( msg->m_side              == 1);

        data = "04:35:41.695,2251812698590528,ADD ASK,IBM,179.40,100,ARCA,ARCA";
        msg  = dynamic_pointer_cast<AddOrderMsg>(parser.parse_msg(data));
        
        BOOST_TEST( msg->m_timestamp.tv_sec  == 1390867200);
        BOOST_TEST( msg->m_timestamp.tv_nsec == 16541695000000);
        BOOST_TEST( msg->m_msgtype           == 'S');
        BOOST_TEST( msg->m_orderId           == 2251812698590528);
        BOOST_TEST( msg->m_price             == 1794000);
        BOOST_TEST( msg->m_shares            == 100);
        BOOST_TEST( msg->m_side              == -1);

    }

    BOOST_AUTO_TEST_CASE( test_order_executed )
    {
        auto parser = algoseek::MsgParser("20140128");

        string data = "08:39:57.809,5465656,EXECUTE BID,IBM,0.00,20,,EDGX";

        auto msg = dynamic_pointer_cast<OrderExecutedMsg>(parser.parse_msg(data));

        BOOST_TEST( msg->m_timestamp.tv_sec  == 1390867200);
        BOOST_TEST( msg->m_timestamp.tv_nsec == 31197809000000);
        BOOST_TEST( msg->m_msgtype           == 'E');
        BOOST_TEST( msg->m_orderId           == 5465656);
        BOOST_TEST( msg->m_price             == 0);
        BOOST_TEST( msg->m_shares            == 20);
        BOOST_TEST( msg->m_side              == 1);

        data = "09:05:23.322,12116657,EXECUTE ASK,IBM,0.00,85,,EDGX";

        msg = dynamic_pointer_cast<OrderExecutedMsg>(parser.parse_msg(data));

        BOOST_TEST( msg->m_timestamp.tv_sec  == 1390867200);
        BOOST_TEST( msg->m_timestamp.tv_nsec == 32723322000000);
        BOOST_TEST( msg->m_msgtype           == 'E');
        BOOST_TEST( msg->m_orderId           == 12116657);
        BOOST_TEST( msg->m_price             == 0);
        BOOST_TEST( msg->m_shares            == 85);
        BOOST_TEST( msg->m_side              == -1);


    }

BOOST_AUTO_TEST_SUITE_END()