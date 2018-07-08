#ifndef ALGOSEEK_MSGPARSER_H
#define ALGOSEEK_MSGPARSER_H

#include <ctime>
#include <memory>
#include <map>
#include <string>
#include <boost/spirit/include/qi.hpp>
#include "MessageBase.h"

namespace algoseek {

    class MsgParser {

    public:
        MsgParser(const std::string &base_date);
        std::shared_ptr<MessageBase> parse_msg(const std::string &input);
    
    private:
        timespec m_start_of_day; 

        //static boost::spirit::qi::rule<std::string::const_iterator, time_t> s_timespec_rule;
        static std::map<std::string, char> s_msgcode; 
    };

}
#endif //ALGOSEEK_MSGPARSER_H