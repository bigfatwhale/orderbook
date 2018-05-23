//
// Created by Uncle Chu on 9/5/18.
//
#include "PriceBucket.h"
#include <algorithm>
#include <iostream>
#include <numeric>

using namespace std;

PriceBucket::PriceBucket(uint64_t pricePoint, Order const &order) :
        m_pricePoint{pricePoint}

{
    addOrder(order);
}

PriceBucket::PriceBucket(uint64_t pricePoint) :
        m_pricePoint{pricePoint}

{
}

void PriceBucket::addOrder(Order const &order)
{
    m_orders.push_back(order);
}

void PriceBucket::removeOrder(Order const &order)
{
    if ( order.orderId == 0 ) {
        // TODO : only adding this for passing lobster load. rework it out later.
        auto it = std::find_if(m_orders.begin(), m_orders.end(), [](Order &o) { return o.orderId == 0; });
        if (it != m_orders.end()) {
            if (it->volume == order.volume)
                m_orders.erase(it);
            else
                it->volume -= order.volume;
        }
    }
    else
    {
        auto f = [&order]( Order &o1 ) { return o1.orderId == order.orderId; };
        auto it = std::find_if( m_orders.begin(), m_orders.end(), f);
        if ( it != m_orders.end() )
            m_orders.erase(it);
    }
}

void PriceBucket::adjustOrderVolume(Order &order, int32_t volume)
{
    // assumptions : order must exists in the price bucket.
    // After modifying +/- the volume field in the order, we need to
    // adjust the PriceBucket.m_volume field as well because it keeps
    // a summary of the volume

    order.volume += volume;
}

uint32_t PriceBucket::numOrders()
{
    return m_orders.size();
}

uint32_t PriceBucket::totalVolume()
{
    // count the total volume we have in this bucket.
    auto f = []( int a, Order const& b)
               { return a + b.volume; };
    auto sum = accumulate( m_orders.begin(), m_orders.end(), 0, f);
    return sum;
}
