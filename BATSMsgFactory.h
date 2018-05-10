//
// Created by Uncle Chu on 10/4/18.
//

#ifndef PITCH_SPIRIT_BATSMSGFACTORY_H
#define PITCH_SPIRIT_BATSMSGFACTORY_H

#include <string>
#include <memory>

class BATSMessageBase;

class BATSMsgFactory
{
public:

    static std::shared_ptr<BATSMessageBase> createMsg( int timestamp, char msgtype, std::string msg );
    static std::shared_ptr<BATSMessageBase> createMsg( int timestamp, char msgtype, char* start, char* end );
};


#endif //PITCH_SPIRIT_BATSMSGFACTORY_H
