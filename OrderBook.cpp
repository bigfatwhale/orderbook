//
// Created by Uncle Chu on 21/4/18.
//

#include <algorithm>
#include "OrderBook.hpp"

using namespace std;

void Book::addOrder(Order &order)
{
    if (m_priceBuckets[order.price] == nullptr)
    {
        auto bucket = new PriceBucket(order.price, order);
        m_priceBuckets[order.price] = bucket;
        if (order.side == BookType::BUY)
            m_bestPrice = max(order.price, m_bestPrice);
    }
}

void Book::removeOrder(Order &order)
{

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

}

uint64_t LimitOrderBook::bestBid()
{
    return m_buyBook.bestPrice();
}