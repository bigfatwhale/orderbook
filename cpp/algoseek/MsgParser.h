#ifndef ALGOSEEK_MSGPARSER_H
#define ALGOSEEK_MSGPARSER_H

#include <ctime>
#include <memory>
#include <map>
#include <string>
#include <utility>
#include <boost/spirit/include/qi.hpp>
#include "MessageBase.h"

namespace algoseek {

    class MsgParser {

    public:
        MsgParser(const std::string &base_date);
        std::shared_ptr<MessageBase> parse_msg(const std::string &input);
    
    private:
        timespec m_start_of_day; 

        static boost::spirit::qi::symbols<char, std::pair<char, int8_t>> s_msgcode;
        static constexpr int s_dateskip{9}; 
    };

}
#endif //ALGOSEEK_MSGPARSER_H