#ifndef ALGOSEEK_MSGPARSER_H
#define ALGOSEEK_MSGPARSER_H

#include <memory>
#include "MessageBase.h"

namespace algoseek {

    class MsgParser {

    public:
        MsgParser(int base_date);
        std::shared_ptr<MessageBase> parse_msg(const std::string &input);
    
    };

}
#endif //ALGOSEEK_MSGPARSER_H