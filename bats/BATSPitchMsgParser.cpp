//
// Created by Uncle Chu on 12/4/18.
//
#include <memory>
#include <string>
#include <boost/spirit/include/qi.hpp>
#include <boost/python.hpp>
#include "BATSPitchMsgParser.h"
#include "BATSMsgFactory.h"

using namespace std;
namespace qi = boost::spirit::qi;

shared_ptr<BATSMessageBase> BATSPitchMsgParser::parse_msg(const string &input)
{
    shared_ptr<BATSMessageBase> pMsg;
    pMsg = BATSMsgFactory::createMsg(input[msgtype_idx], input.c_str(), input.c_str() + input.size());

    if ( pMsg == nullptr )
        throw std::runtime_error("Error parsing message : " + input );

    return pMsg;
}

void BATSPitchMsgParser::export_to_python()
{
    boost::python::class_<BATSPitchMsgParser>("BATSPitchMsgParser")
            .def("parse_msg", &BATSPitchMsgParser::parse_msg);

}
