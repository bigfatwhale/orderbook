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
    PriceBucket() : m_pricePoint{0} {}
    PriceBucket(uint64_t pricePoint);
    virtual ~PriceBucket() {}

    virtual void addOrder( Order const& order);
    virtual void removeOrder( Order const& order);
    void adjustOrderVolume( Order &order, int32_t volume );

    std::deque<Order>::iterator begin() { return m_orders.begin(); }
    std::deque<Order>::iterator end()   { return m_orders.end();   }

    uint32_t totalVolume();
    uint32_t numOrders();
    uint64_t m_pricePoint;

protected:
    std::deque<Order> m_orders;

};


#endif //ORDERBOOK_PRICEBUCKET_H
