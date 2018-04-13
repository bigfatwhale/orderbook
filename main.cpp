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

    // test trade msg
    input = "S28800168P1K27GA00000YS000300AAPL  00018319001K27GA00000Z";
    msg = parser->parse_msg(input);

    return 0;
}