#include <cstdint>
#include <ctime>
#include <iostream>
#include <unordered_map>
#include <boost/bind.hpp>
#include <boost/date_time/date.hpp>
#include "boost/date_time/gregorian/gregorian_types.hpp"
#include "boost/date_time/date_parsing.hpp"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/conversion.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "MessageBase.h"
#include "MsgParser.h"
#include "MsgFactory.h"

using namespace std;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::fusion;

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

namespace algoseek
{

    std::map<std::string, char> MsgParser::s_msgcode = {
        {"ADD BID", 'B'},
        {"ADD ASK", 'S'},
        {"EXECUTE ASK", 'E'},
        {"EXECUTE BID", 'E'},
        {"CANCEL BID", 'C'},
        {"CANCEL ASK", 'C'},
        {"FILL BID", 'F'},
        {"FILL ASK", 'F'},
        {"DELETE BID", 'D'},
        {"DELETE ASK", 'D'},
        {"TRADE BID", 'T'},
        {"TRADE ASK", 'T'},
    };

    MsgParser::MsgParser(const string &base_date) : m_start_of_day{0}
    {
        // convert date to seconds since epoch.
        // the algoseek data format has first field as date, we setup stuff to 
        // parse it only once.
        auto d = boost::date_time::parse_undelimited_date<date>(base_date);
        m_start_of_day.tv_sec = to_time_t(ptime(d)); 
        
    };

    shared_ptr<MessageBase> MsgParser::parse_msg(const std::string &input)
    {

        timespec t{m_start_of_day.tv_sec, 0};
        char msgtype;
        string suffix;
        uint64_t order_id{0};

        auto fill_suffix   = [&suffix] (string &s) { suffix=s; };
        auto fill_msgtype  = [&msgtype] (string & s) {};
        auto fill_ts_nanos = [&t] (int h, int m, int s, int ms)
                             { t.tv_nsec = ( ( h * 3600 + m * 60 + s ) * 1000 + ms ) * 1000000L; };
        
        // "04:00:00.512,2251812698588658,ADD BID,IBM,176.33,400,ARCA,ARCA"
        auto parse_ok = qi::parse(input.begin(), input.end(),
                            ( qi::uint_ >> ":" >> qi::uint_ >> ":" >> qi::uint_ >> "." >> qi::uint_ )
                            [ phi::bind(fill_ts_nanos, qi::_1, qi::_2, qi::_3, qi::_4)]
                            >> "," >> qi::int_[phi::ref(order_id)=qi::_1] >> qi::as_string[*qi::char_][fill_msgtype] 
                            >> "," >> qi::as_string[*qi::char_][fill_suffix]
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
