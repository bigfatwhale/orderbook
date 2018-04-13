#include <iostream>
#include <string>
#include <boost/spirit/include/qi.hpp>
#include <memory>
#include <utility>
#include "BATSMsgFactory.h"
#include "BATSPitchMsgParser.h"

using namespace boost::spirit;
using namespace std;


int test() {

    auto parser = std::make_unique<BATSPitchMsgParser>();

    string input;
    shared_ptr<BATSMessageBase> msg;

    // test add order short
    input = "S28800168A1K27GA00000YS000100AAPL  0001831900Y";
    msg = parser->parse_msg(input);

    // test add order long
    input = "S28800168d1K27GA00000YS000100AAPL  0001831900YBAML";
    msg = parser->parse_msg(input);

    // test order executed msg
    input = "S28800168E1K27GA00000Y0001001K27GA00000K";
    msg = parser->parse_msg(input);

    // test order cancel msg
    input = "S28800168X1K27GA00000Y000500";
    msg = parser->parse_msg(input);

    // test trade msg
    input = "S28800168P1K27GA00000YS000300AAPL  00018319001K27GA00000Z";
    msg = parser->parse_msg(input);

    // test trade break msg
    input = "S28800168B1K27GA00000Y";
    msg = parser->parse_msg(input);

    // test trading status msg
    input = "S28800168HAAPLSPOTT0XY";
    msg = parser->parse_msg(input);

    // test auction update msg
    input = "S28800168IAAPLSPOTC00010068000000020000000001000000015034000001309800";
    msg = parser->parse_msg(input);

    // test auction summary msg
    input = "S28800168JAAPLSPOTC00010068000000020000";
    msg = parser->parse_msg(input);

    // test retail price improvement msg
    input = "S28800168RAAPLSPOTS";
    msg = parser->parse_msg(input);

    return 0;
}