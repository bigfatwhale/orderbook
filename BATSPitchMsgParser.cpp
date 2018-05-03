//
// Created by Uncle Chu on 12/4/18.
//
#include <memory>
#include <string>
#include <boost/spirit/include/qi.hpp>
#include <boost/python.hpp>
#include "BATSPitchMsgParser.h"
#include "BATSMsgFactory.h"

using namespace std;
namespace qi = boost::spirit::qi;

shared_ptr<BATSMessageBase> BATSPitchMsgParser::parse_msg(const string &input)
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

void BATSPitchMsgParser::export_to_python()
{
    boost::python::class_<BATSPitchMsgParser>("BATSPitchMsgParser")
            .def("parse_msg", &BATSPitchMsgParser::parse_msg);

}