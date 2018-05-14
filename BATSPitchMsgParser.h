//
// Created by Uncle Chu on 12/4/18.
//

#ifndef BATS_PITCH_PARSER_BATSPITCHMSGPARSER_H
#define BATS_PITCH_PARSER_BATSPITCHMSGPARSER_H

//#define BASE_OFFSET 1
//#define FIELD_LENGTH_TIMESTAMP 8


#include "BATSMessageBase.h"

class BATSPitchMsgParser
{
private:
	static const unsigned BASE_OFFSET = 1;
	static const unsigned FIELD_LENGTH_TIMESTAMP = 8;
public:
    std::shared_ptr<BATSMessageBase>  parse_msg( const std::string &input );
    static void export_to_python();
};

#endif //BATS_PITCH_PARSER_BATSPITCHMSGPARSER_H
