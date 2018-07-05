
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

// struct decoder_base
// {
//     decoder_base(timespec timestamp, char msgtype) :
//             m_ts(timestamp), m_mtype(msgtype) {}

//     timespec m_ts;
//     char m_mtype;
// };

class MessageBase {

// public:
//     MessageBase();
//     MessageBase(int timestamp, char msgtype);
//     virtual ~MessageBase();

//     // uint32_t m_date  
//     // uint32_t m_timestamp;
//     // char m_msgtype;

//     std::chrono::high_resolution_clock::time_point m_timestamp;
//     int64_t m_orderId; // observed -ve values for order id.
    timespec m_timestamp;
    char m_msgtype;
};

}

#endif //PITCH_SPIRIT_BATSMESSAGEBASE_H