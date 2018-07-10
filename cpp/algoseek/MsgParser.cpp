#include <cstdint>
#include <ctime>
#include <iostream>
#include <list>
#include <unordered_map>
#include <utility>
#include <string>
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
#include "AlgoseekUtils.h"

using namespace std;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::fusion;

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

namespace algoseek
{
    qi::symbols<char, pair<char, int8_t>> MsgParser::s_msgcode( 
        std::list<string>{ "ADD BID", 
                           "ADD ASK",
                           "EXECUTE BID",
                           "EXECUTE ASK",
                           "CANCEL BID", 
                           "CANCEL ASK", 
                           "FILL BID", 
                           "FILL ASK", 
                           "DELETE BID",
                           "DELETE ASK", 
                           "TRADE BID", 
                           "TRADE ASK", 
                           "CROSS"
                         }, 
        std::list<pair<char, int8_t>>{ make_pair('B',  1), 
                                       make_pair('S', -1),
                                       make_pair('E',  1),
                                       make_pair('E', -1), 
                                       make_pair('C',  1),
                                       make_pair('C', -1), 
                                       make_pair('F',  1),
                                       make_pair('F', -1),     
                                       make_pair('D',  1),
                                       make_pair('D', -1),
                                       make_pair('T',  1),
                                       make_pair('T', -1), 
                                       make_pair('X',  0)
                                     }
    );

    MsgParser::MsgParser(const string &base_date) : m_start_of_day{0}
    {
        // convert date to seconds since epoch.
        // the algoseek data format has first field as date, we setup stuff to 
        // parse it only once. i went to look at mktime source code and wanted to faint.
        auto d = boost::date_time::parse_undelimited_date<date>(base_date);
        m_start_of_day.tv_sec = to_time_t(ptime(d)); 
        
    };

    shared_ptr<MessageBase> MsgParser::parse_msg(const string &input)
    {
        timespec t{m_start_of_day.tv_sec, 0};
        pair<char, int8_t> msgtype;
        string suffix;
        uint64_t order_id{0};

        auto fill_suffix    = [&suffix] (string &s) { suffix=s; };
        auto fill_ts_nanos = [&t]      (auto &x)   { t.tv_nsec = x; };
        
        auto parse_ok = qi::parse(input.begin() + s_dateskip, input.end(),
                            timestr_to_nanos<string::const_iterator>[fill_ts_nanos]
                            >> "," >> qi::long_[phi::ref(order_id) = qi::_1] 
                            >> "," >> s_msgcode[phi::ref(msgtype)  = qi::_1] 
                            >> "," >> qi::as_string[*qi::char_][fill_suffix]
                        );
        
        shared_ptr<MessageBase> pMsg;
        if (!parse_ok)
            throw std::runtime_error("Error parsing message");
        else {
            pMsg = MsgFactory::createMsg(t, msgtype.first, msgtype.second, order_id, suffix);
        }
        return pMsg;

    }

}
