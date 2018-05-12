//
// Created by Uncle Chu on 12/5/18.
//
#include <ctime>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "MessageBase.h"
#include "MsgParser.h"

using namespace std;

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

namespace lobster
{

    shared_ptr<MessageBase> parse_msg(const std::string &input)
    {

        timespec t;
        char msgCode;

        auto fill_ts_secs  = [&t] (int secs)  { t.tv_sec  = secs; };
        auto fill_ts_nanos = [&t] (int nanos) { t.tv_nsec = nanos; };
        auto fill_msgcode  = [&msgCode] (char c) { msgCode=c; };

        auto grammar = qi::int_[fill_ts_secs] >> qi::char_(".")
                        >> qi::int_[fill_ts_nanos] >> qi::char_(",")
                        >> qi::char_[fill_msgcode];

        auto parse_ok = qi::parse(input.begin(), input.end(), grammar );

        shared_ptr<MessageBase> pMsg;

        if (parse_ok)
        {
           cout << "time.sec=" << t.tv_sec << "time.nanos=" << t.tv_nsec << endl;
        }
        return pMsg;
    }


}

