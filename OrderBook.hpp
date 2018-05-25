//
// Created by Uncle Chu on 15/4/18.
//

#ifndef ORDERBOOK_ORDERBOOK_HPP
#define ORDERBOOK_ORDERBOOK_HPP

#include <algorithm>
#include <deque>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <memory>
#include <iostream>
#include <type_traits>
#include <boost/function.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include "Order.h"
#include "PriceBucket.h"
#include "PriceBucketManager.hpp"

// use template traits to wire buy/sell price direction related stuff
// during compile time.
template <typename P, typename AskBidT> struct BookTrait;

template <typename P>
struct BookTrait<P, Bid> { static uint64_t bestPrice(P &pbm) { return pbm.maxPrice(); } };

template <typename P>
struct BookTrait<P, Ask> { static uint64_t bestPrice(P &pbm) { return pbm.minPrice(); } };

template <typename PriceBucketManagerT, typename AskBidTrait>
class Book
{
public:

    Book(BookType t) : m_bookType{t} {}

    void addOrder( Order &order )
    {
        auto bucket = m_priceBucketManager.findBucket(order.price);
        if (bucket==nullptr)
            bucket = m_priceBucketManager.addBucket(order.price);
        bucket->addOrder(order);
    }

    void removeOrder( Order &order )
    {
        auto bucket = m_priceBucketManager.findBucket(order.price);
        bucket->removeOrder(order);
        if (bucket->totalVolume() == 0)
            m_priceBucketManager.removeBucket(order.price);
    }

    uint32_t volumeForPricePoint( uint64_t price )
    {
        auto bucket = m_priceBucketManager.findBucket(price);
        return bucket->totalVolume();
    }

    uint64_t bestPrice() { return BookTrait<PriceBucketManagerT, AskBidTrait>::bestPrice(m_priceBucketManager); }

    // This is just a forwarding iterator which forwards all calls to a
    // PriceBucketManager::iterator.
    class iterator : public boost::iterator_facade<
            iterator,
            typename PriceBucketManagerT:: template iterator<AskBidTrait>::value_type,
            boost::bidirectional_traversal_tag >
    {
        using PriceBucketManagerIter = typename PriceBucketManagerT:: template iterator<AskBidTrait>;

    public:
        iterator( PriceBucketManagerT& pbm, bool isEnd=false) : m_bucket_iter{pbm, isEnd} {}

    private:
        friend class boost::iterator_core_access;

        PriceBucketManagerIter m_bucket_iter;

        void increment() { m_bucket_iter++; }
        void decrement() { m_bucket_iter--; }

        bool equal( iterator const& other ) const { return this->m_bucket_iter == other.m_bucket_iter; }

        typename PriceBucketManagerT:: template iterator<AskBidTrait>::value_type&
        dereference() const { return *m_bucket_iter; }
    };

    iterator begin() { return iterator( m_priceBucketManager ); }
    iterator end()   { return iterator( m_priceBucketManager, true ); }

private:
    BookType m_bookType;
    PriceBucketManagerT m_priceBucketManager;
};

template <typename PriceBucketManagerT=PriceBucketManager<> >
class LimitOrderBook {
    // class implementing the facilities for a "continuous double auction"
    // see https://arxiv.org/abs/1012.0349 for general survey of limit order books.
public:

    LimitOrderBook() : m_buyBook{BookType::BUY}, m_sellBook{BookType::SELL} {}

    template <typename B1, typename B2, typename Comp>
    void doCrossSpread(Order &order, B1 &book, B2 &oppbook, Comp& f)
    {
        int32_t residual_volume;
        // iterate and walk through the prices, generating filled order msgs.
        if ( ( oppbook.bestPrice() > 0 ) && f(order.price, oppbook.bestPrice()) )
        {
            residual_volume = crossSpreadWalk(order, oppbook, f);
            order.volume = residual_volume;
        }

        // if order.volume is still +ve, the can be either there is no cross-spread walk done
        // or the cross-spread walk only filled part of the volume. In that case we continue to
        // add the left-over volume in a new order.
        if (order.volume > 0)
            book.addOrder(order);
    }

    void addOrder(Order &order)
    {
        // we need to check if this incoming order "crossed the spread", i.e. if the bid
        // for some particular stock is $100.00 and the ask is $100.10, and then some one puts
        // in a new order with bid at $100.10, then we have to match this with the sell book
        // and generate an execution msg.

        static std::greater_equal<uint64_t> ge;
        static std::less_equal<uint64_t>    le;

        if ( order.side == BookType::BUY )
            doCrossSpread(order, m_buyBook, m_sellBook, ge);
        else
            doCrossSpread(order, m_sellBook, m_buyBook, le);
    }

    void removeOrder( Order &order )
    {
        if ( order.side == BookType::BUY )
            m_buyBook.removeOrder(order);
        else
            m_sellBook.removeOrder(order);
    }

    uint32_t volumeForPricePoint( uint64_t price, BookType t )
    {
        if (t == BookType::BUY)
            return m_buyBook.volumeForPricePoint(price);
        else
            return m_sellBook.volumeForPricePoint(price);
    }

    uint64_t bestBid() { return m_buyBook.bestPrice();  }
    uint64_t bestAsk() { return m_sellBook.bestPrice(); }

    using BuyBookIter  = typename Book<PriceBucketManagerT, Bid>::iterator;
    using SellBookIter = typename Book<PriceBucketManagerT, Ask>::iterator;

    BuyBookIter  bids_begin()  { return m_buyBook.begin();  }
    BuyBookIter  bids_end()    { return m_buyBook.end();    }
    SellBookIter asks_begin()  { return m_sellBook.begin(); }
    SellBookIter asks_end()    { return m_sellBook.end();   }

private:

    template <typename B, typename Comp>
    uint32_t crossSpreadWalk( Order &order, B &book, Comp &f )
    {
        // walks the order book match off orders, returns residual volume for
        // addition back into the LOB
        auto volume = order.volume;
        auto priceBucketIter = book.begin();
        auto orderIter = priceBucketIter->begin();

        std::deque<Order> orders_to_remove;

        while (volume > 0 && f( order.price, orderIter->price)) // && order_i != priceBucketIter->end() // this is always true on first entry
        {
            if ( volume >= orderIter->volume )
            {
                volume -= orderIter->volume;
                orderIter->volume = 0;
                orders_to_remove.push_back(*orderIter);
                //TODO: generate execution msg, for both sides.
            }
            else
            {
                orderIter->volume -= volume;
                volume = 0;
                //TODO: generate execution msg, for both sides.
            }

            if ( ++orderIter == priceBucketIter->end() )
            {
                if ( ++priceBucketIter == book.end() )
                    break;
            }

        }

        for ( auto &i : orders_to_remove )
            book.removeOrder(i);

        return volume;
    }

    Book<PriceBucketManagerT, Bid> m_buyBook;
    Book<PriceBucketManagerT, Ask> m_sellBook;
};


#endif //ORDERBOOK_ORDERBOOK_HPP
