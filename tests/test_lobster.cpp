//
// Created by Uncle Chu on 11/5/18.
//

#define BOOST_TEST_MODULE test_orderbook
#define BOOST_TEST_DYN_LINK

#include <string>
#include <boost/test/unit_test.hpp>
#include "../lobster/AddOrderMsg.hpp"
#include "../lobster/MsgParser.h"

using namespace std;

BOOST_AUTO_TEST_SUITE( test_orderbook_suite )

    BOOST_AUTO_TEST_CASE( test_simple )
    {
        auto parser = lobster::MsgParser();

        string msg = "34200.017459617,5,0,1,2238200,-1";
        auto x = parser.parse_msg(msg);
    }

BOOST_AUTO_TEST_SUITE_END()