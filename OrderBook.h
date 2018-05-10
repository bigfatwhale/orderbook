//
// Created by Uncle Chu on 15/4/18.
//

#ifndef ORDERBOOK_ORDERBOOK_HPP
#define ORDERBOOK_ORDERBOOK_HPP

#include <deque>
#include <string>
#include <unordered_map>
#include <utility>
#include <memory>
#include "Order.h"
#include "PriceBucket.h"
#include "PriceBucketManager.hpp"

//#define MAX_PRICE 100000000 // 10000.0000 in fixed point format
// reasonable for most stocks except berkshire
#define MAX_PRICE 100000



// 1. need to have order struct
// 2. order book struct
template <typename PriceBucketManagerT>
class Book
{
public:
    Book(BookType t) : m_bookType{t},
                       //m_priceBuckets(new PriceBucket*[MAX_PRICE + 1]),
                       m_bestPrice(m_bookType == BookType::BUY ? 0 : MAX_PRICE)
    {
//        for( int i=0; i < MAX_PRICE+1; i++ )
//            m_priceBuckets[i] = nullptr;
    }
    ~Book()
    {
//        for( int i=0; i < MAX_PRICE+1; i++ )
//            delete m_priceBuckets[i];
//        delete [] m_priceBuckets;
    }
    void addOrder( Order &order )
    {
        m_priceBucketManager.
    }

    void removeOrder( Order &order );
    uint64_t bestPrice() { return m_bestPrice; }
    uint32_t volumeForPricePoint( uint64_t price );

private:
    BookType m_bookType;
    //PriceBucket **m_priceBuckets;
    PriceBucketManagerT m_priceBucketManager;

//    uint64_t m_minAsk;
//    uint64_t m_maxBid;

//  Replace the above with best price. if it's a BUY book, then the best price is
//  the highest pricePoint among all the order currently in the book, i.e. best bid
//  - the highest price someone is willing to buy an item. If is the SELL book,
//  then it is the best ask, i.e. lowest of price some is willing to sell an item.

    uint64_t m_bestPrice;

};

//void Book::addOrder(Order &order)
//{
//    if (m_priceBuckets[order.price] == nullptr)
//        m_priceBuckets[order.price] = new PriceBucket(order.price, order);
//    else
//        m_priceBuckets[order.price]->addOrder(order);
//
//    if (order.side == BookType::BUY)
//        m_bestPrice = max(order.price, m_bestPrice);
//    else
//        m_bestPrice = min(order.price, m_bestPrice);
//
//}

template <typename PriceBucketManagerT>
class LimitOrderBook
{
    // class implementing the facilities for a "continuous double auction"
    // see https://arxiv.org/abs/1012.0349 for general survey of limit order books.
public:
    LimitOrderBook() : m_buyBook{BookType::BUY}, m_sellBook{BookType::SELL} {}
    void addOrder( Order &order );
    void removeOrder( Order &order );
    uint32_t volumeForPricePoint( uint64_t price, BookType t);
    uint64_t bestAsk();
    uint64_t bestBid();

private:

    Book<PriceBucketManagerT> m_buyBook;
    Book<PriceBucketManagerT> m_sellBook;

};

#endif //ORDERBOOK_ORDERBOOK_HPP
