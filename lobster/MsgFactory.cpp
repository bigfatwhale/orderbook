//
// Created by Uncle Chu on 12/5/18.
//

#include <ctime>
#include <memory>
#include "MsgFactory.h"
#include "MessageBase.h"
#include "AddOrderMsg.hpp"

using namespace std;

namespace lobster
{
    using AddOrderMsgDecoder           = AddOrderMsg::add_order_decoder<string::iterator>;

    template<typename DecodeT, typename MsgT>
    shared_ptr<MessageBase> decode(timespec timestamp, char msgtype, string msg)
    {
        shared_ptr<DecodeT>  decoder = std::make_shared<DecodeT>(timestamp, msgtype);
        auto data = make_shared<MsgT>();

        bool ret = qi::parse(msg.begin(), msg.end(), *decoder, *data);
        if (ret)
            return data;
        else
            return nullptr;
    }


    shared_ptr<MessageBase>
    MsgFactory::createMsg(timespec timestamp, char msgtype, std::string msg)
    {
        switch (msgtype)
        {
            case 1: {
                return decode<AddOrderMsgDecoder, AddOrderMsg>( timestamp, msgtype, msg);
                break;
            }
        }

    }

}