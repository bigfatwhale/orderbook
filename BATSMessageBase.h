//
// Created by Uncle Chu on 9/4/18.
//

#ifndef PITCH_SPIRIT_BATSMESSAGEBASE_H
#define PITCH_SPIRIT_BATSMESSAGEBASE_H

//#include <boost/fusion/adapted/struct/adapt_struct.hpp>
//#include <boost/phoenix/function.hpp>

//namespace qi = boost::spirit::qi;

class BATSMessageBase {

public:
    BATSMessageBase(int timestamp, char msgtype);
    virtual ~BATSMessageBase();

    int  m_timestamp;
    char m_msgtype;
};



#endif //PITCH_SPIRIT_BATSMESSAGEBASE_H