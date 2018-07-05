
#ifndef ORDERBOOK_PROTOCOL_H
#define ORDERBOOK_PROTOCOL_H

#include <cstdint>

namespace protocol
{
    enum { header_length = 4    };
    enum { msg_length    = 1024 };

    struct message_type
    {
        static constexpr uint16_t CONNECT{1};
        static constexpr uint16_t CLIENT_ID{2};
        static constexpr uint16_t ADD_ORDER{3};
    };

    struct message_length
    {
        static constexpr uint16_t CONNECT{1};
        static constexpr uint16_t CLIENT_ID{4};
    };

}

#endif
