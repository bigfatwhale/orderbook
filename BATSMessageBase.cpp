//
// Created by Uncle Chu on 9/4/18.
//

#include <iostream>
#include <memory>
#include <string>
#include <boost/spirit/include/qi.hpp>
#include <boost/python.hpp>
#include <sstream>
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

std::string BATSMessageBase::repr()
{
    std::stringstream ss;
    ss << "BATSMessageBase(timestamp=" << m_timestamp << ", msgtype=" << m_msgtype << ")";
    return ss.str();
}

void BATSMessageBase::export_to_python()
{
    boost::python::class_<BATSMessageBase>("BATSMessageBase")
            .def(boost::python::init<>())
            .def(boost::python::init<int, char>())
            .def_readwrite("timestamp", &BATSMessageBase::m_timestamp)
            .def_readwrite("msgtype", &BATSMessageBase::m_msgtype)
            .def("__repr__", &BATSMessageBase::repr);
}