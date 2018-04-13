//
// Created by Uncle Chu on 12/4/18.
//
#define BOOST_TEST_MODULE example
#define BOOST_TEST_NO_LIB

#include <memory>
#include <string>
#include <boost/test/included/unit_test.hpp>
#include "../BATSPitchMsgParser.h"
//#include "../BATSAddOrderMsg.hpp"
#include "../BATSTradeBreakMsg.hpp"


using namespace std;

std::shared_ptr<BATSMessageBase> parse( string const& input )
{
    auto parser = std::make_unique<BATSPitchMsgParser>();
    return parser->parse_msg(input);
}

BOOST_AUTO_TEST_CASE( free_test_function )
/* Compare with void free_test_function() */
{
//    BOOST_TEST( dynamic_cast<BATSAddOrderMsg*>(
//                        parse( "S28800168A1K27GA00000YS000100AAPL  0001831900Y" ).get() )
//                != nullptr );

    BOOST_TEST(true);
}