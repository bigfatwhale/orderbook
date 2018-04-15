//
// Created by Uncle Chu on 15/4/18.
//

#ifndef ORDERBOOK_ORDERBOOK_HPP
#define ORDERBOOK_ORDERBOOK_HPP

#include <list>
#include <string>

#define MAX_PRICE 100000000 // 10000.0000 in fixed point format
                            // reasonable for most stocks except berkshire

// 1. need to have order struct
// 2. order book struct

struct Order
{
    uint64_t orderId;
    uint64_t price; // 6.4 fixed point representation
    uint32_t volume;
    std::string origId; // the id of the participant this order originated from.
};

class PriceBucket
{
public:
    PriceBucket() {}

private:
    PriceBucket *m_nextBucket;
    PriceBucket *m_previousBucket;
    std::list<Order> m_orders;
    double pricePoint;
};


class OrderBook
{
public:
    OrderBook() : m_priceBuckets(new PriceBucket[MAX_PRICE + 1]),
                  m_maxBid{0}, m_minAsk{MAX_PRICE} {}
    ~OrderBook() { delete [] m_priceBuckets; }
    void addOrder( Order &order );
    void removeOrder( Order &order );

private:
    PriceBucket *m_priceBuckets;
    uint64_t m_minAsk;
    uint64_t m_maxBid;
};


#endif //ORDERBOOK_ORDERBOOK_HPP
