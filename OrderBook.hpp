//
// Created by Uncle Chu on 15/4/18.
//

#ifndef ORDERBOOK_ORDERBOOK_HPP
#define ORDERBOOK_ORDERBOOK_HPP

#include <algorithm>
#include <deque>
#include <string>
#include <unordered_map>
#include <utility>
#include <memory>
#include <iostream>
#include <boost/function.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include "Order.h"
#include "PriceBucket.h"
#include "PriceBucketManager.hpp"

template <typename PriceBucketManagerT>
class Book
{
public:
    Book(BookType t) : m_bookType{t},
                       m_bestPriceFunc{t == BookType::BUY ?
                                       &PriceBucketManagerT::maxPrice :
                                       &PriceBucketManagerT::minPrice} {
    }

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
        if (bucket->m_volume == 0)
            m_priceBucketManager.removeBucket(order.price);
    }

    uint32_t volumeForPricePoint( uint64_t price )
    {
        auto bucket = m_priceBucketManager.findBucket(price);
        return bucket->m_volume;
    }

    uint64_t bestPrice() { return m_bestPriceFunc( &m_priceBucketManager ); }

    // This is just a forwarding iterator which forwards all calls to a
    // PriceBucketManager::iterator.
    class iterator : public boost::iterator_facade<
            iterator,
            PriceBucket,
            boost::bidirectional_traversal_tag >
    {

        using PriceBucketManagerIter = typename PriceBucketManagerT::iterator;
    public:
        iterator( PriceBucketManagerT& pbm, uint32_t price, BookType t) :
                m_bucket_iter{pbm, price, t} {}

    private:
        friend class boost::iterator_core_access;

        PriceBucketManagerIter m_bucket_iter;

        void increment() { m_bucket_iter++; }
        void decrement() { m_bucket_iter--; }

        bool equal( iterator const& other ) const
        {
            return this->m_bucket_iter == other.m_bucket_iter;
        }

        PriceBucket& dereference() const { return *m_bucket_iter; }
    };

    iterator begin() { return iterator( m_priceBucketManager, bestPrice(), m_bookType ); }
    iterator end()   { return iterator(m_priceBucketManager, 0, m_bookType ); }

    BookType bookType() { return m_bookType; }

private:
    BookType m_bookType;
    PriceBucketManagerT m_priceBucketManager;
    boost::function<uint64_t (PriceBucketManagerT*) > m_bestPriceFunc;

};

template <typename PriceBucketManagerT=PriceBucketManager<> >
class LimitOrderBook {
    // class implementing the facilities for a "continuous double auction"
    // see https://arxiv.org/abs/1012.0349 for general survey of limit order books.
public:
    LimitOrderBook() : m_buyBook{BookType::BUY}, m_sellBook{BookType::SELL} {}

    void addOrder(Order &order)
    {
        // we need to check if this incoming order "crossed the spread", i.e. if the bid
        // for some particular stock is $100.00 and the ask is $100.10, and then some one puts
        // in a new order with bid at $100.10, then we have to match this with the sell book
        // and generate an execution msg.
        if ( order.side == BookType::BUY )
        {
            // iterate and walk through the prices, generating filled order msgs.
            if ( ( m_sellBook.bestPrice() > 0 ) && ( order.price >= m_sellBook.bestPrice() ) )
            {
                crossSpreadWalk(order, m_sellBook);
                // TODO: add logic for handling residual volume. An order still needs to be added.
            }
            else
                m_buyBook.addOrder(order);
        }
        else
        {
            // iterate and walk through the prices, generating filled order msgs.
            if ( ( m_buyBook.bestPrice() > 0 ) && ( order.price <= m_buyBook.bestPrice() ) )
            {
                crossSpreadWalk(order, m_buyBook);
                // TODO: add logic for handling residual volume. An order still needs to be added.
            }
            else
                m_sellBook.addOrder(order);
        }
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

    using BookIter = typename Book<PriceBucketManagerT>::iterator;

    BookIter bids_begin() { return m_buyBook.begin();  }
    BookIter asks_begin() { return m_sellBook.begin(); }
    BookIter bids_end()   { return m_buyBook.end();    }
    BookIter asks_end()   { return m_sellBook.end();   }

private:

    void crossSpreadWalk( Order &order, Book<PriceBucketManagerT>& book )
    {
        auto volume = order.volume;
        auto priceBucketIter = book.begin();
        auto order_i = priceBucketIter->begin();

        //bool price_condition = book.bookType() == BookType::BUY ?

        while (volume > 0) // && order_i != priceBucketIter->end() // this is always true on first entry
        {
            if ( book.bookType() == BookType::BUY )
                order_i->price < order.price


            if ( volume >= order_i->volume )
            {
                volume -= order_i->volume;
                priceBucketIter->adjustOrderVolume(*order_i, -order_i->volume);
                //TODO: generate execution msg, for both sides.
            }
            else
            {
                priceBucketIter->adjustOrderVolume(*order_i, -volume);
                volume = 0;
                //TODO: generate execution msg, for both sides.
            }
            order_i++; // walk orders in the same bucket
            if ( order_i == priceBucketIter->end() )
            {
                priceBucketIter++;
                if ( priceBucketIter == asks_end() )
                    break;
            }

        }

    }

    Book<PriceBucketManagerT> m_buyBook;
    Book<PriceBucketManagerT> m_sellBook;

};


#endif //ORDERBOOK_ORDERBOOK_HPP
