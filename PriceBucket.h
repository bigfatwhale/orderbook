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
    PriceBucket() : m_pricePoint{0}, m_deletedCount{0} {}
    PriceBucket(uint64_t pricePoint, Order const& order);
    PriceBucket(uint64_t pricePoint);
    void addOrder( Order const& order);
    void removeOrder( Order const& order);

    std::deque<Order>::iterator begin() { return m_orders.begin(); }
    std::deque<Order>::iterator end() { return m_orders.end(); }

    uint32_t totalVolume();
    uint32_t numOrders();
    uint64_t m_pricePoint;
    uint32_t m_volume;

private:
    std::deque<Order> m_orders;
    std::unordered_map<uint64_t, uint32_t> m_orderLookup;
    uint32_t m_deletedCount;

};


#endif //ORDERBOOK_PRICEBUCKET_H
