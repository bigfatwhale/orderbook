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


} // end namespace lobster