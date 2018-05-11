//
// Created by Uncle Chu on 11/5/18.
//

#define BOOST_TEST_MODULE test_orderbook
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "../lobster/AddOrderMsg.hpp"

BOOST_AUTO_TEST_SUITE( test_orderbook_suite )

    BOOST_AUTO_TEST_CASE( test_simple )
    {
        lobster::AddOrderMsg x;
    }

BOOST_AUTO_TEST_SUITE_END()