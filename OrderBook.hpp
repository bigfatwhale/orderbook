//
// Created by Uncle Chu on 15/4/18.
//

#ifndef ORDERBOOK_ORDERBOOK_HPP
#define ORDERBOOK_ORDERBOOK_HPP

#include <list>
#include <string>

// 1. need to have order struct
// 2. order book struct

struct Order
{
    uint64_t orderId;
    double price;
    uint32_t volume;
    std::string origId; // the id of the participant this order originated from.
};

class PriceBucket
{
    PriceBucket() {}

private:
    PriceBucket *m_nextBucket;
    PriceBucket *m_previousBucket;
    std::list<Order> m_orders;
    double pricePoint;
};

class OrderBook
{
    void addOrder( Order &order );
    void removeOrder( Order &order );
};


#endif //ORDERBOOK_ORDERBOOK_HPP
