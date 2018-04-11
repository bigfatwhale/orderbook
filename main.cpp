#include <iostream>
#include <string>
#include <boost/spirit/include/qi.hpp>
#include <memory>
#include <utility>
#include "BATSMsgFactory.h"

using namespace boost::spirit;
using namespace std;

#define BASE_OFFSET 1
#define FIELD_LENGTH_TIMESTAMP 8

class PitchMsgParser
{
public:
    shared_ptr<BATSMessageBase>  parse_msg( const string &input );

};

shared_ptr<BATSMessageBase> PitchMsgParser::parse_msg(const string &input)
{
    auto start = input.begin() + BASE_OFFSET;
    auto end   = input.begin() + BASE_OFFSET + FIELD_LENGTH_TIMESTAMP + 1;

    int timestamp = 0;
    char msgCode;

    // 1. parse the timestamp and message type
    auto fill_ts      = [&timestamp] (int &t) { timestamp=t; };
    auto fill_msgcode = [&msgCode]   (char c) { msgCode=c; };

    bool parse_ok = qi::parse(start, end,
                              qi::int_[fill_ts] >> qi::char_[fill_msgcode] );

    // instantiate the appropriate message type
    shared_ptr<BATSMessageBase> pMsg;
    if (!parse_ok)
        throw std::runtime_error("Error parsing message");
    else {
        auto data = input.substr(BASE_OFFSET + FIELD_LENGTH_TIMESTAMP + 1);
        pMsg = BATSMsgFactory::createMsg(timestamp, msgCode, data);
    }
    return pMsg;
}


int main() {

    auto parser = std::make_unique<PitchMsgParser>();

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