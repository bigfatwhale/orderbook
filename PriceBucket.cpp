//
// Created by Uncle Chu on 9/5/18.
//
#include "PriceBucket.h"
#include <iostream>

using namespace std;

PriceBucket::PriceBucket(uint64_t pricePoint, Order const &order) :
        m_pricePoint{pricePoint}, m_volume{0}, m_deletedCount{0}

{
    addOrder(order);
}

PriceBucket::PriceBucket(uint64_t pricePoint) :
        m_pricePoint{pricePoint}, m_volume{0}, m_deletedCount{0}

{
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
        cout << "m_orderLookup has " << m_orderLookup.size() << " items" << endl;
        auto item_num = m_orderLookup[order.orderId];
        cout << "item_num=" << item_num << "orderId=" << order.orderId << endl;
        m_orders[item_num].active = false;
        m_deletedCount++;
        m_volume -= order.volume;
    }
}

void PriceBucket::adjustOrderVolume(Order &order, int32_t volume)
{
    // assumptions : order must exists in the price bucket.
    // After modifying +/- the volume field in the order, we need to
    // adjust the PriceBucket.m_volume field as well because it keeps
    // a summary of the volume

    order.volume += volume;
    m_volume += volume;
}

uint32_t PriceBucket::numOrders()
{
    return m_orders.size() - m_deletedCount;
}

uint32_t PriceBucket::totalVolume()
{
    return m_volume;
}
