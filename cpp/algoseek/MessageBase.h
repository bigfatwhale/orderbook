
#ifndef ALGOSEEK_BATSMESSAGEBASE_H
#define ALGOSEEK_BATSMESSAGEBASE_H

#include <time.h>
#include <ctime>
#include <chrono>
#include <cstdint>

// using nanosec_ts = std::chrono::time_point<std::chrono::high_resolution_clock, 
//                                            std::chrono::nanoseconds>;
// https://stackoverflow.com/questions/43451565/store-timestamps-in-nanoseconds-c

namespace algoseek {

    struct decoder_base
    {
        decoder_base(timespec timestamp, char msgtype, uint8_t side, uint64_t order_id) :
                m_ts(timestamp), m_mtype(msgtype), m_side{side}, m_orderId{order_id} {}

        timespec m_ts;
        char m_mtype;
        uint8_t m_side;
        uint64_t m_orderId;
    };

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

}

#endif //ALGOSEEK_BATSMESSAGEBASE_H