//
// Created by Uncle Chu on 15/4/18.
//

#ifndef ORDERBOOK_ORDERBOOK_HPP
#define ORDERBOOK_ORDERBOOK_HPP

#include <deque>
#include <string>
#include <unordered_map>

//#define MAX_PRICE 100000000 // 10000.0000 in fixed point format
// reasonable for most stocks except berkshire
#define MAX_PRICE 100000

enum class BookType {BUY, SELL};

// 1. need to have order struct
// 2. order book struct

struct Order
{
    Order(uint64_t id, uint64_t price, uint32_t volume, BookType side, std::string const &partId) :
            orderId{id}, price{price}, volume{volume}, side{side}, partId{partId}, active{true} {};
    uint64_t orderId;
    uint64_t price; // 6.4 fixed point representation
    uint32_t volume;
    BookType  side; // ask or bid?
    std::string partId; // the id of the participant this order originated from.
    bool active; // for logical delete when in price bucket.
};

class PriceBucket
{
public:
    PriceBucket() : m_nextBucket{nullptr}, m_previousBucket{nullptr} {}
    PriceBucket(uint64_t pricePoint, Order const& order);
    void addOrder( Order const& order);
    void removeOrder( Order const& order);
    uint32_t totalVolume();
    uint32_t numOrders();

private:
    PriceBucket *m_nextBucket;     // brings us to the next/prev non-empty price bucket.
    PriceBucket *m_previousBucket; // needs to maintain.
    std::deque<Order> m_orders;
    std::unordered_map<uint64_t, uint32_t> m_orderLookup;
    uint64_t m_pricePoint;
    uint32_t m_deletedCount;
    uint32_t m_volume;
};

class Book
{
public:
    Book(BookType t) : m_bookType{t},
                       m_priceBuckets(new PriceBucket*[MAX_PRICE + 1]),
                       m_bestPrice(m_bookType == BookType::BUY ? 0 : MAX_PRICE)
    {
        for( int i=0; i < MAX_PRICE+1; i++ )
            m_priceBuckets[i] = nullptr;
    }
    ~Book()
    {
        for( int i=0; i < MAX_PRICE+1; i++ )
            delete m_priceBuckets[i];
        delete [] m_priceBuckets;
    }
    void addOrder( Order &order );
    void removeOrder( Order &order );
    uint64_t bestPrice() { return m_bestPrice; }
    uint32_t volumeForPricePoint( uint64_t price );

private:
    BookType m_bookType;
    PriceBucket **m_priceBuckets;

//    uint64_t m_minAsk;
//    uint64_t m_maxBid;

//  Replace the above with best price. if it's a BUY book, then the best price is
//  the highest pricePoint among all the order currently in the book, i.e. best bid
//  - the highest price someone is willing to buy an item. If is the SELL book,
//  then it is the best ask, i.e. lowest of price some is willing to sell an item.

    uint64_t m_bestPrice;

};

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

    Book m_buyBook;
    Book m_sellBook;

};

#endif //ORDERBOOK_ORDERBOOK_HPP
