//
// Created by Uncle Chu on 9/4/18.
//

#include <iostream>
#include <memory>
#include <string>
#include <boost/spirit/include/qi.hpp>
#include "BATSMessageBase.h"

using namespace std;
using namespace boost::spirit;

BATSMessageBase::BATSMessageBase(int timestamp, char msgtype) :
        m_timestamp(timestamp), m_msgtype(msgtype)
{
}

BATSMessageBase::BATSMessageBase() : m_timestamp(0), m_msgtype(0)
{
}

BATSMessageBase::~BATSMessageBase()
{
}



