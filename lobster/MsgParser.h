//
// Created by Uncle Chu on 12/5/18.
//

#ifndef ORDERBOOK_MSGPARSER_H
#define ORDERBOOK_MSGPARSER_H

#include <memory>
#include "MessageBase.h"

namespace lobster {

    class MsgParser {

    public:
        std::shared_ptr<MessageBase> parse_msg(const std::string &input);

    };

}
#endif //ORDERBOOK_MSGPARSER_H
