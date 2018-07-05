#include <ctime>
#include <iostream>
#include <unordered_map>
#include <boost/bind.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "MessageBase.h"
#include "MsgParser.h"
#include "MsgFactory.h"

using namespace std;

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

namespace algoseek
{

    // struct tm t = {0};  // Initalize to all 0's
    // t.tm_year = 112;  // This is year-1900, so 112 = 2012
    // t.tm_mon = 8;
    // t.tm_mday = 15;
    // t.tm_hour = 21;
    // t.tm_min = 54;
    // t.tm_sec = 13;
    // time_t timeSinceEpoch = mktime(&t);
    // // Result: 1347764053

    MsgParser::MsgParser(const string &base_date)
    {
        // convert date to seconds since epoch.
        tm time_info;
    };

    shared_ptr<MessageBase> MsgParser::parse_msg(const std::string &input)
    {
        // memoize the date field. calling mktime takes a long time.
        //static day_start_secs = 0;
        


        timespec t;
        char msgCode;
        string suffix;

        auto fill_ts_secs  = [&t] (int secs)  { t.tv_sec  = secs; };
        auto fill_ts_nanos = [&t] (int nanos) { t.tv_nsec = nanos; };
        auto fill_msgcode  = [&msgCode] (char c) { msgCode=c; };
        auto fill_suffix   = [&suffix] (string &s) { suffix=s; };

        auto parse_ok = qi::parse(input.begin(), input.end(),
                                  qi::int_[fill_ts_secs] >> qi::char_(".")
                                     >> qi::int_[fill_ts_nanos] >> qi::char_(",")
                                     >> qi::char_[fill_msgcode] >> qi::char_(",")
                                     >> qi::as_string[*qi::char_][fill_suffix]
        );

        // shared_ptr<MessageBase> pMsg;
        // if (!parse_ok)
        //     throw std::runtime_error("Error parsing message");
        // else {
        //     pMsg = MsgFactory::createMsg(t, msgCode, suffix);
        // }
        // return pMsg;

        return nullptr;
    }


}
