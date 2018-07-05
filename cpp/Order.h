//
// Created by Uncle Chu on 9/5/18.
//

#ifndef ORDERBOOK_ORDER_H
#define ORDERBOOK_ORDER_H

#include <string>

enum class BookType {BUY, SELL};

class Ask;
class Bid;

struct Order
{
	Order() {};
    Order(uint64_t id, uint64_t price, uint32_t volume, BookType side, uint32_t partId) :
            orderId{id}, price{price}, volume{volume}, partId{partId}, side{side} {};
            //, active{true} {};
    uint64_t orderId;
    uint64_t price; // 6.4 fixed point representation
    uint32_t volume;
    uint32_t partId;      // change to int from the above to use boost lock free queue.
    BookType  side; // ask or bid?
    //std::string partId; // the id of the participant this order originated from.
    //bool active; // for logical delete when in price bucket.
};

#endif //ORDERBOOK_ORDER_H
