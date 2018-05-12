//
// Created by Uncle Chu on 11/5/18.
//

#ifndef ORDERBOOK_MESSAGEBASE_H
#define ORDERBOOK_MESSAGEBASE_H

#include <ctime>

namespace lobster {

class MessageBase {
public:
    MessageBase();
    MessageBase(timespec timestamp, char msgtype);
    virtual ~MessageBase();

    timespec  m_timestamp;
    char m_msgtype;
};

} // end namespace lobster

#endif //ORDERBOOK_MESSAGEBASE_H
