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
    int timestamp = 0;
    char msgCode;
    string suffix;

    // 1. parse the timestamp and message type
    auto fill_ts      = [&timestamp] (int &t) { timestamp=t; };
    auto fill_msgcode = [&msgCode]   (char c) { msgCode=c; };
    auto fill_suffix  = [&suffix] (string &s) { suffix=s; };

    bool parse_ok = qi::parse(input.begin(), input.end(),
                              qi::int_[fill_ts] >> qi::char_[fill_msgcode]
                              >> qi::as_string[*qi::char_][fill_suffix] );

    // instantiate the appropriate message type
    shared_ptr<BATSMessageBase> pMsg;
    if (!parse_ok)
        throw std::runtime_error("Error parsing message");
    else {
        pMsg = BATSMsgFactory::createMsg(timestamp, msgCode, suffix.c_str(), suffix.c_str() + suffix.size());
        if ( pMsg == nullptr )
            throw std::runtime_error("Error parsing message : " + input );
    }
    return pMsg;
}

void BATSPitchMsgParser::export_to_python()
{
    boost::python::class_<BATSPitchMsgParser>("BATSPitchMsgParser")
            .def("parse_msg", &BATSPitchMsgParser::parse_msg);

}
