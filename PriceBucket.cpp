//
// Created by Uncle Chu on 9/5/18.
//
#include "PriceBucket.h"
#include <algorithm>
#include <iostream>
#include <numeric>

using namespace std;

PriceBucket::PriceBucket(uint64_t pricePoint) : m_pricePoint{pricePoint} {}

void PriceBucket::addOrder(Order const &order)
{
    m_orders.push_back(order);
}

void PriceBucket::removeOrder(Order const &order)
{
    auto f = [&order](Order &o1) { return o1.orderId == order.orderId; };
    auto it = std::find_if(m_orders.begin(), m_orders.end(), f);
    if (it != m_orders.end())
        m_orders.erase(it);
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
