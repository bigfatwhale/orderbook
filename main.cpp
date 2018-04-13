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

    // test trade msg
    input = "S28800168P1K27GA00000YS000300AAPL  00018319001K27GA00000Z";
    msg = parser->parse_msg(input);

    return 0;
}