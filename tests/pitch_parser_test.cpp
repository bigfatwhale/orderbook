//
// Created by Uncle Chu on 12/4/18.
//

#define BOOST_TEST_MODULE pitch_parser_test

#include <memory>
#include <string>
#include <boost/test/included/unit_test.hpp>
#include "../BATSPitchMsgParser.h"
#include "../BATSTradeBreakMsg.hpp"
#include "../BATSTradingStatusMsg.hpp"


using namespace std;

std::shared_ptr<BATSMessageBase> parse( string const& input )
{
    auto parser = std::make_unique<BATSPitchMsgParser>();
    return parser->parse_msg(input);
}
BOOST_AUTO_TEST_SUITE( test_parse_suite )

    BOOST_AUTO_TEST_CASE( test_parse_trade_break )
    {
        auto msg = parse( "S28800168B1K27GA00000Y" );

        BOOST_TEST( msg->m_timestamp == 28800168 );
        BOOST_TEST( msg->m_msgtype == 'B');

        auto addOrderMsg = dynamic_pointer_cast<BATSTradeBreakMsg>(msg);
        // int('1K27GA00000Y', 36) -> 204969015920664610 # from python.
        BOOST_TEST( addOrderMsg->m_execId == 204969015920664610 );
    }

    BOOST_AUTO_TEST_CASE( test_parse_trade_status )
    {
        auto msg = parse( "S28800168HAAPLSPOTT0XY");

        BOOST_TEST( msg->m_timestamp == 28800168 );
        BOOST_TEST( msg->m_msgtype == 'H');
        auto tradeStatusMsg = dynamic_pointer_cast<BATSTradingStatusMsg>(msg);

        BOOST_TEST( tradeStatusMsg->m_symbol == "AAPLSPOT"); // ticker symbol
        BOOST_TEST( tradeStatusMsg->m_halt_status == 'T'); //halted status
        BOOST_TEST( tradeStatusMsg->m_reserved1 == 'X');
        BOOST_TEST( tradeStatusMsg->m_reserved2 == 'Y');
    }

BOOST_AUTO_TEST_SUITE_END()