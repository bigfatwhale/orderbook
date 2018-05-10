//
// Created by Uncle Chu on 21/4/18.
//

#include <algorithm>
#include <iostream>
#include "OrderBook.h"

using namespace std;



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