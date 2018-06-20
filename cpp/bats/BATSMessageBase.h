//
// Created by Uncle Chu on 9/4/18.
//

#ifndef PITCH_SPIRIT_BATSMESSAGEBASE_H
#define PITCH_SPIRIT_BATSMESSAGEBASE_H

class BATSMessageBase {

public:
    BATSMessageBase();
    BATSMessageBase(int timestamp, char msgtype);
    virtual ~BATSMessageBase();

    int  m_timestamp;
    char m_msgtype;
};

#endif //PITCH_SPIRIT_BATSMESSAGEBASE_H