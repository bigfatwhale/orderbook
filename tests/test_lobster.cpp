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
using namespace lobster;

BOOST_AUTO_TEST_SUITE( test_lobster_suite )

    BOOST_AUTO_TEST_CASE( test_simple )
    {
        auto parser = lobster::MsgParser();

        string data = "34200.18960767,1,11885113,21,2238100,1";
        auto msg = parser.parse_msg(data);

        auto addOrderMsg = dynamic_pointer_cast<AddOrderMsg>(msg);
        BOOST_TEST( addOrderMsg->m_timestamp.tv_sec == 34200);
        BOOST_TEST( addOrderMsg->m_timestamp.tv_nsec == 18960767);
        BOOST_TEST( addOrderMsg->m_msgtype == '1');
        BOOST_TEST( addOrderMsg->m_orderId == 11885113);
        BOOST_TEST( addOrderMsg->m_shares == 21);
        BOOST_TEST( addOrderMsg->m_price == 2238100);
        BOOST_TEST( addOrderMsg->m_side == 1);

    }

BOOST_AUTO_TEST_SUITE_END()