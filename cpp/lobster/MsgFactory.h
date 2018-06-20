//
// Created by Uncle Chu on 12/5/18.
//

#ifndef ORDERBOOK_MSGFACTORY_H
#define ORDERBOOK_MSGFACTORY_H

#include <ctime>
#include <memory>
#include "MessageBase.h"

namespace lobster {

    class MsgFactory {

    public:
        static std::shared_ptr<MessageBase> createMsg( timespec timestamp, char msgtype, std::string msg );
    };
}

#endif //ORDERBOOK_MSGFACTORY_H
