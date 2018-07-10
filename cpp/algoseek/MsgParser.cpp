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

    shared_ptr<MessageBase> MsgParser::parse_msg(const std::string &input)
    {
        timespec t{m_start_of_day.tv_sec, 0};
        pair<char, int8_t> msgtype;
        string suffix;
        uint64_t order_id{0};

        auto fill_suffix   = [&suffix] (string &s) { suffix=s; };
        auto fill_ts_nanos = [&t] (int h, int m, int s, int ms)
                             { t.tv_nsec = ( ( h * 3600 + m * 60 + s ) * 1000 + ms ) * 1000000L; };
        
        auto parse_ok = qi::parse(input.begin(), input.end(),
                            ( qi::uint_ >> ":" >> qi::uint_ >> ":" >> qi::uint_ >> "." >> qi::uint_ )
                            [ phi::bind(fill_ts_nanos, qi::_1, qi::_2, qi::_3, qi::_4) ]
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
