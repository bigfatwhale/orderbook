//
// Created by Uncle Chu on 9/5/18.
//

#include "PriceBucket.h"

PriceBucket::PriceBucket(uint64_t pricePoint, Order const &order) :
        m_nextBucket{nullptr}, m_previousBucket{nullptr}, m_pricePoint{pricePoint}, m_deletedCount{0},
        m_volume{0}
{
    addOrder(order);
}

void PriceBucket::addOrder(Order const &order)
{
    m_orders.push_back(order);
    // this lets us go from orderId to item in the deque.
    // we use it for logical delete of orders.
    m_orderLookup[order.orderId] = m_orders.size() - 1;
    m_volume += order.volume;
}

void PriceBucket::removeOrder(Order const &order)
{
    if (m_orderLookup.find(order.orderId) != m_orderLookup.end() )
    {
        m_orders[m_orderLookup[order.orderId]].active = false;
        m_deletedCount++;
        m_volume -= order.volume;
    }
}

uint32_t PriceBucket::numOrders()
{
    return m_orders.size() - m_deletedCount;
}

uint32_t PriceBucket::totalVolume()
{
    return m_volume;
}
