//
// Created by Uncle Chu on 12/4/18.
//
#define BOOST_TEST_MODULE example
#define BOOST_TEST_NO_LIB

#include <memory>
#include <string>
#include <boost/test/included/unit_test.hpp>
#include "../BATSPitchMsgParser.h"
#include "../BATSTradeBreakMsg.hpp"


using namespace std;

std::shared_ptr<BATSMessageBase> parse( string const& input )
{
    auto parser = std::make_unique<BATSPitchMsgParser>();
    return parser->parse_msg(input);
}

BOOST_AUTO_TEST_CASE( test_parse_trade_break )
{
    auto msg = parse( "S28800168B1K27GA00000Y" );

    BOOST_TEST( msg->m_timestamp == 28800168 );
    BOOST_TEST( msg->m_msgtype == 'B');

    auto addOrderMsg = dynamic_pointer_cast<BATSTradeBreakMsg>(msg);
    BOOST_TEST( addOrderMsg->m_execId == 204969015920664610 );
}