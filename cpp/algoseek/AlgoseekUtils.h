
#ifndef ALGOSEEK_UTILS_HPP
#define ALGOSEEK_UTILS_HPP

#include <string>
#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;

template <typename It>
qi::rule<It, long()> timestr_to_nanos = 
       ( qi::uint_ >> ':' >> qi::uint_ >> ':' >> qi::uint_ >> '.' >> qi::uint_ ) 
       [ qi::_val = (( qi::_1  * 3600 + 
                       qi::_2  * 60 + 
                       qi::_3) * 1000 + 
                       qi::_4) * 1000000l ];

#endif // ALGOSEEK_UTILS_HPP