//
// Created by Uncle Chu on 12/4/18.
//

#define BOOST_TEST_MODULE pitch_parser_test
#define BOOST_TEST_DYN_LINK

#include <memory>
#include <string>
#include <fstream>
#include <boost/test/included/unit_test.hpp>
#include "../BATSPitchMsgParser.h"
#include "../BATSTradeBreakMsg.hpp"
#include "../BATSTradingStatusMsg.hpp"
#include "../BATSRetailPriceImproveMsg.hpp"
#include "../BATSOrderCancelMsg.hpp"
#include "../BATSOrderExecutedMsg.hpp"
#include "../BATSAuctionSummaryMsg.hpp"
#include "../BATSAuctionUpdateMsg.hpp"
#include "../BATSAddOrderMsg.hpp"
#include "../BATSTradeMsg.hpp"

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

    BOOST_AUTO_TEST_CASE( test_parse_retail_price_improve )
    {
        auto msg = parse( "S28800168RAAPLSPOTS" );

        BOOST_TEST( msg->m_timestamp == 28800168 );
        BOOST_TEST( msg->m_msgtype == 'R');

        auto retailMsg = dynamic_pointer_cast<BATSRetailPriceImproveMsg>(msg);
        BOOST_TEST( retailMsg->m_symbol == "AAPLSPOT"); // ticker symbol
        BOOST_TEST( retailMsg->m_retail_price_improve == 'S'); // S : sell-side RPI

    }

    BOOST_AUTO_TEST_CASE( test_parse_order_cancel )
    {
        auto msg = parse("S28800168X1K27GA00000Y000500");

        BOOST_TEST( msg->m_timestamp == 28800168 );
        BOOST_TEST( msg->m_msgtype == 'X');

        auto cancelMsg = dynamic_pointer_cast<BATSOrderCancelMsg>(msg);

        BOOST_TEST( cancelMsg->m_orderId == 204969015920664610); // ticker symbol
        BOOST_TEST( cancelMsg->m_shares == 500); // S : sell-side RPI
    }

    BOOST_AUTO_TEST_CASE( test_parse_order_execute )
    {
        auto msg = parse("S28800168E1K27GA00000Y0001001K27GA00000K");

        BOOST_TEST( msg->m_timestamp == 28800168 );
        BOOST_TEST( msg->m_msgtype == 'E');

        auto execMsg = dynamic_pointer_cast<BATSOrderExecutedMsg>(msg);
        BOOST_TEST( execMsg->m_orderId == 204969015920664610); // ticker symbol
        BOOST_TEST( execMsg->m_shares == 100); // S : sell-side RPI
        BOOST_TEST( execMsg->m_execId == 204969015920664596); // S : sell-side RPI

    }

    BOOST_AUTO_TEST_CASE( test_parse_auction_summary, * boost::unit_test::tolerance(0.0001) )
    {
        auto msg = parse("S28800168JAAPLSPOTC00010068000000020000");

        BOOST_TEST( msg->m_timestamp == 28800168 );
        BOOST_TEST( msg->m_msgtype == 'J');

        auto auctionSummaryMsg = dynamic_pointer_cast<BATSAuctionSummaryMsg>(msg);

        BOOST_TEST( auctionSummaryMsg->m_symbol == "AAPLSPOT");
        BOOST_TEST( auctionSummaryMsg->m_auction_type == 'C');
        BOOST_TEST( auctionSummaryMsg->m_price == 100.68);
        BOOST_TEST( auctionSummaryMsg->m_shares == 20000);

    }

    BOOST_AUTO_TEST_CASE( test_parse_auction_update, * boost::unit_test::tolerance(0.0001)  )
    {
        auto msg = parse("S28800168IAAPLSPOTC00010068000000020000000001000000015034000001309800");

        BOOST_TEST( msg->m_timestamp == 28800168 );
        BOOST_TEST( msg->m_msgtype == 'I');

        auto auctionUpdateMsg = dynamic_pointer_cast<BATSAuctionUpdateMsg>(msg);

        BOOST_TEST( auctionUpdateMsg->m_symbol == "AAPLSPOT");
        BOOST_TEST( auctionUpdateMsg->m_auction_type == 'C');
        BOOST_TEST( auctionUpdateMsg->m_reference_price == 100.68);
        BOOST_TEST( auctionUpdateMsg->m_buyshares == 20000);
        BOOST_TEST( auctionUpdateMsg->m_sellshares == 10000);
        BOOST_TEST( auctionUpdateMsg->m_indicative_price == 150.3400);
        BOOST_TEST( auctionUpdateMsg->m_auction_only_price == 130.9800);
    }

    BOOST_AUTO_TEST_CASE( test_parse_add_order, * boost::unit_test::tolerance(0.0001)  )
    {
        auto msg = parse("S28800168A1K27GA00000YS000100AAPL  0001831900Y");

        BOOST_TEST( msg->m_timestamp == 28800168 );
        BOOST_TEST( msg->m_msgtype == 'A');

        auto addOrderMsg = dynamic_pointer_cast<BATSAddOrderMsg>(msg);

        BOOST_TEST( addOrderMsg->m_orderId == 204969015920664610);
        BOOST_TEST( addOrderMsg->m_side == 'S');
        BOOST_TEST( addOrderMsg->m_shares == 100);
        BOOST_TEST( addOrderMsg->m_symbol == "AAPL  ");
        BOOST_TEST( addOrderMsg->m_price == 183.19);
        BOOST_TEST( addOrderMsg->m_display == 'Y');

        msg = parse("S28800168d1K27GA00000YS000100AAPL  0001831900YBAML");

        BOOST_TEST( msg->m_timestamp == 28800168 );
        BOOST_TEST( msg->m_msgtype == 'd');

        addOrderMsg = dynamic_pointer_cast<BATSAddOrderMsg>(msg);

        BOOST_TEST( addOrderMsg->m_orderId == 204969015920664610);
        BOOST_TEST( addOrderMsg->m_side == 'S');
        BOOST_TEST( addOrderMsg->m_shares == 100);
        BOOST_TEST( addOrderMsg->m_symbol == "AAPL  ");
        BOOST_TEST( addOrderMsg->m_price == 183.19);
        BOOST_TEST( addOrderMsg->m_display == 'Y');
        BOOST_TEST( addOrderMsg->m_partId == "BAML");
    }

    BOOST_AUTO_TEST_CASE( test_parse_trade, * boost::unit_test::tolerance(0.0001)  )
    {
        auto msg = parse("S28800168P1K27GA00000YS000300AAPL  00018319001K27GA00000Z");

        BOOST_TEST( msg->m_timestamp == 28800168 );
        BOOST_TEST( msg->m_msgtype == 'P');

        auto tradeMsg = dynamic_pointer_cast<BATSTradeMsg>(msg);
        BOOST_TEST( tradeMsg->m_orderId == 204969015920664610);
        BOOST_TEST( tradeMsg->m_side == 'S');
        BOOST_TEST( tradeMsg->m_shares == 300);
        BOOST_TEST( tradeMsg->m_symbol == "AAPL  ");
        BOOST_TEST( tradeMsg->m_price == 183.19);
        BOOST_TEST( tradeMsg->m_execId == "1K27GA00000Z");

        msg = parse("S28800168r1K27GA00000YS000300AAPLSPOT00018319001K27GA00000Z");

        BOOST_TEST( msg->m_timestamp == 28800168 );
        BOOST_TEST( msg->m_msgtype == 'r');

        tradeMsg = dynamic_pointer_cast<BATSTradeMsg>(msg);
        BOOST_TEST( tradeMsg->m_orderId == 204969015920664610);
        BOOST_TEST( tradeMsg->m_side == 'S');
        BOOST_TEST( tradeMsg->m_shares == 300);
        BOOST_TEST( tradeMsg->m_symbol == "AAPLSPOT");
        BOOST_TEST( tradeMsg->m_price == 183.19);
        BOOST_TEST( tradeMsg->m_execId == "1K27GA00000Z");

    }

    BOOST_AUTO_TEST_CASE( test_load_datafile )
    {
        // this is like a smoke test. mass load a large number of messages from file.

        auto parser = std::make_unique<BATSPitchMsgParser>();
        string line;

        ifstream myfile("pitch_example_data");
        BOOST_TEST(myfile.is_open());

        while (getline(myfile, line))
        {
            parser->parse_msg(line);
        }
        myfile.close();
    }

    BOOST_AUTO_TEST_CASE( test_load_data_100k )
    {
        // this is like a smoke test. mass load a large number of messages from file.

        auto parser = std::make_unique<BATSPitchMsgParser>();
        string line;

        ifstream myfile("pitch_data_100k");
        BOOST_TEST(myfile.is_open());

        while (getline(myfile, line))
        {
            parser->parse_msg(line);
        }
        myfile.close();
    }

BOOST_AUTO_TEST_SUITE_END()
