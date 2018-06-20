//
// Created by Uncle Chu on 11/5/18.
//

#include "MessageBase.h"

namespace lobster
{
    MessageBase::MessageBase(timespec timestamp, char msgtype) :
            m_timestamp{timestamp}, m_msgtype(msgtype)
    {
    }

    MessageBase::MessageBase() : m_timestamp{0,0}, m_msgtype(0)
    {
    }

    MessageBase::~MessageBase()
    {
    }

    OrderMsgBase::OrderMsgBase() : MessageBase()
    {
    }

    OrderMsgBase::OrderMsgBase(timespec timestamp, char msgtype, uint64_t orderId,
                               uint32_t shares, uint64_t price, int8_t side) :
            MessageBase(timestamp, msgtype), m_orderId{orderId}, m_shares{shares},
            m_price{price}, m_side{side}
    {}

} // end namespace lobster