
#define BOOST_TEST_MODULE test_algoseek
#define BOOST_TEST_DYN_LINK

#include "AddOrderMsg.hpp"
#include "algoseek/MsgParser.h"
#include <boost/test/unit_test.hpp>
#include <string>

using namespace std;
using namespace algoseek;

BOOST_AUTO_TEST_SUITE( test_lobster_suite )


    BOOST_AUTO_TEST_CASE( test_add_order )
    {
        auto parser = algoseek::MsgParser("20141011");

        string data = "04:00:00.512,2251812698588658,ADD BID,IBM,176.33,400,ARCA,ARCA";

        parser.parse_msg(data);
        // auto msg = dynamic_pointer_cast<TradeHaltMsg>(parser.parse_msg(data));
        // BOOST_TEST( msg->m_timestamp.tv_sec == 34414);
        // BOOST_TEST( msg->m_timestamp.tv_nsec == 765462735);
        // BOOST_TEST( msg->m_msgtype == '7');
        // BOOST_TEST( msg->m_price == -1);
    }



BOOST_AUTO_TEST_SUITE_END()