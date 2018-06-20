//
// Created by Uncle Chu on 11/5/18.
//

#ifndef ORDERBOOK_LOBSTERUTIL_H
#define ORDERBOOK_LOBSTERUTIL_H

#include <ctime>

namespace lobster
{

    struct decoder_base
    {
        decoder_base(timespec timestamp, char msgtype) :
                m_ts(timestamp), m_mtype(msgtype) {}

        timespec m_ts;
        char m_mtype;
    };

} // end lobster namespace

#endif //ORDERBOOK_LOBSTERUTIL_H
