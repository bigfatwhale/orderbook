//
// Created by Uncle Chu on 21/4/18.
//

#include <algorithm>
#include <iostream>
#include "OrderBook.hpp"

using namespace std;

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

void Book::addOrder(Order &order)
{
    if (m_priceBuckets[order.price] == nullptr)
        m_priceBuckets[order.price] = new PriceBucket(order.price, order);
    else
        m_priceBuckets[order.price]->addOrder(order);

    if (order.side == BookType::BUY)
        m_bestPrice = max(order.price, m_bestPrice);
    else
        m_bestPrice = min(order.price, m_bestPrice);

}

void Book::removeOrder(Order &order)
{
    if (m_priceBuckets[order.price] != nullptr)
        m_priceBuckets[order.price]->removeOrder(order);
}

uint32_t Book::volumeForPricePoint( uint64_t price )
{
    if (m_priceBuckets[price] == nullptr)
        return 0;
    else
    {
        return m_priceBuckets[price]->totalVolume();
    }

}

void LimitOrderBook::addOrder(Order &order)
{
    if ( order.side == BookType::BUY )
        m_buyBook.addOrder(order);
    else
        m_sellBook.addOrder(order);
}

void LimitOrderBook::removeOrder(Order &order)
{
    if ( order.side == BookType::BUY )
        m_buyBook.removeOrder(order);
    else
        m_sellBook.removeOrder(order);

}

uint64_t LimitOrderBook::bestBid()
{
    return m_buyBook.bestPrice();
}

uint32_t LimitOrderBook::volumeForPricePoint(uint64_t price, BookType t)
{
    if (t == BookType::BUY)
    {
        return m_buyBook.volumeForPricePoint(price);
    }
}