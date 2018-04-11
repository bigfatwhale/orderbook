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
    void parse_msg( const string &input );

};

void PitchMsgParser::parse_msg(const string &input)
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

}


int main() {

    auto parser = std::make_unique<PitchMsgParser>();

//    // test add order short
//    string input = "S28800168A1K27GA00000YS000100AAPL  0001831900Y";
//    parser->parse_msg(input);
//
//    // test add order long
//    string input_long = "S28800168d1K27GA00000YS000100AAPL  0001831900YBAML";
//    parser->parse_msg(input_long);

//    // test order executed msg
//    string input = "S28800168E1K27GA00000Y0001001K27GA00000K";
//    parser->parse_msg(input);
//
//    // test order cancel msg
//    string input = "S28800168X1K27GA00000Y000500";
//    parser->parse_msg(input);
//
//    // test trade msg
//    string input = "S28800168P1K27GA00000YS000300AAPL  00018319001K27GA00000Z";
//    parser->parse_msg(input);
//
//    // test trade break msg
//    string input = "S28800168B1K27GA00000Y";
//    parser->parse_msg(input);

//    // test trading status msg
//    string input = "S28800168HAAPLSPOTT0XY";
//    parser->parse_msg(input);

//    // test auction update msg
//    string input = "S28800168IAAPLSPOTC00010068000000020000000001000000015034000001309800";
//    parser->parse_msg(input);

//    // test auction update msg
//    string input = "S28800168IAAPLSPOTC00010068000000020000000001000000015034000001309800";
//    parser->parse_msg(input);

    // test retail price improvement msg
    string input = "S28800168RAAPLSPOTS";
    parser->parse_msg(input);

    return 0;
}