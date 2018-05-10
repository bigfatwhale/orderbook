//
// Created by Uncle Chu on 9/5/18.
//

#ifndef ORDERBOOK_PRICEBUCKET_H
#define ORDERBOOK_PRICEBUCKET_H

#include <deque>
#include <unordered_map>
#include "Order.h"

class PriceBucket
{
public:
//    PriceBucket() : m_nextBucket{nullptr}, m_previousBucket{nullptr}, m_deletedCount{0} {}
    PriceBucket() : m_deletedCount{0} {}
    PriceBucket(uint64_t pricePoint, Order const& order);
    PriceBucket(uint64_t pricePoint);
    void addOrder( Order const& order);
    void removeOrder( Order const& order);
    uint32_t totalVolume();
    uint32_t numOrders();
    uint64_t m_pricePoint;
    uint32_t m_volume;

private:
//    PriceBucket *m_nextBucket;     // brings us to the next/prev non-empty price bucket.
//    PriceBucket *m_previousBucket; // needs to maintain.
    std::deque<Order> m_orders;
    std::unordered_map<uint64_t, uint32_t> m_orderLookup;
    uint32_t m_deletedCount;

};


#endif //ORDERBOOK_PRICEBUCKET_H
