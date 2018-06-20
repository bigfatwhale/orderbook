//
// Created by Uncle Chu on 11/5/18.
//

#ifndef ORDERBOOK_MESSAGEBASE_H
#define ORDERBOOK_MESSAGEBASE_H

#include <ctime>
#include <cstdint>

namespace lobster {

    class MessageBase
    {
    public:
        MessageBase();

        MessageBase(timespec timestamp, char msgtype);

        virtual ~MessageBase();

        timespec m_timestamp;
        char m_msgtype;
    };

    class OrderMsgBase : public MessageBase
    {
    public:

        OrderMsgBase();
        OrderMsgBase(timespec timestamp, char msgtype,
                     uint64_t orderId, uint32_t shares,
                     uint64_t price, int8_t side);

        uint64_t m_orderId;
        uint32_t m_shares;
        uint64_t m_price;
        int8_t   m_side;
    };

} // end namespace lobster

#endif //ORDERBOOK_MESSAGEBASE_H
