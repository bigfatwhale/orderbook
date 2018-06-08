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
    Order(uint64_t id, uint64_t price, uint32_t volume, BookType side, std::string const &partId) :
            orderId{id}, price{price}, volume{volume}, side{side}, partId{partId}, active{true} {};
    uint64_t orderId;
    uint64_t price; // 6.4 fixed point representation
    uint32_t volume;
    BookType  side; // ask or bid?
    std::string partId; // the id of the participant this order originated from.
    bool active; // for logical delete when in price bucket.
};

#endif //ORDERBOOK_ORDER_H
