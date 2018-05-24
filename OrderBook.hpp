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
        if (bucket->totalVolume() == 0)
            m_priceBucketManager.removeBucket(order.price);
    }

    uint32_t volumeForPricePoint( uint64_t price )
    {
        auto bucket = m_priceBucketManager.findBucket(price);
        return bucket->totalVolume();
    }

    uint64_t bestPrice() { return m_bestPriceFunc( &m_priceBucketManager ); }

    // This is just a forwarding iterator which forwards all calls to a
    // PriceBucketManager::iterator.
    class iterator : public boost::iterator_facade<
            iterator,
            typename PriceBucketManagerT::iterator::value_type,
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

        typename PriceBucketManagerT::iterator::value_type& dereference() const { return *m_bucket_iter; }
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

    using IBookType = Book<PriceBucketManagerT>;

    LimitOrderBook() : m_buyBook{BookType::BUY}, m_sellBook{BookType::SELL} {}

    void doCrossSpread(Order &order, IBookType &book)
    {
        int32_t residual_volume;
        // iterate and walk through the prices, generating filled order msgs.
        bool price_condition = order.side == BookType::BUY ?
                                    order.price >= book.bestPrice() :
                                    order.price <= book.bestPrice();

        if ( ( book.bestPrice() > 0 ) && price_condition )
        {
            residual_volume = crossSpreadWalk(order, book);
            order.volume = residual_volume;
        }
    }

    void addOrder(Order &order)
    {
        // we need to check if this incoming order "crossed the spread", i.e. if the bid
        // for some particular stock is $100.00 and the ask is $100.10, and then some one puts
        // in a new order with bid at $100.10, then we have to match this with the sell book
        // and generate an execution msg.

        if ( order.side == BookType::BUY )
        {
            doCrossSpread(order, m_sellBook);

            // if order.volume is still +ve, the can be either there is no cross-spread walk done
            // or the cross-spread walk only filled part of the volume. In that case we continue to
            // add the left-over volume in a new order.
            if (order.volume > 0)
                m_buyBook.addOrder(order);
        }
        else
        {
            doCrossSpread(order, m_buyBook);
            // see explanation for if-clause.
            if (order.volume > 0)
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

    // for unittest convenience only
    void addOrderToBuyBook(  Order &order ) { m_buyBook.addOrder(order); }
    void addOrderToSellBook( Order &order ) { m_sellBook.addOrder(order); }

private:

    virtual uint32_t crossSpreadWalk( Order &order, Book<PriceBucketManagerT>& book )
    {
        // walks the order book match off orders, returns residual volume for
        // addition back into the LOB
        auto volume = order.volume;
        auto priceBucketIter = book.begin();
        auto orderIter = priceBucketIter->begin();

        bool price_condition;
        std::deque<Order> orders_to_remove;

        while (volume > 0) // && order_i != priceBucketIter->end() // this is always true on first entry
        {
            if ( book.bookType() == BookType::BUY ) // order is a sell order, orderIter is walking through
            {
                // order is a sell order, orderIter is walking through buy orders in the book.
                // so we process any order in the book where sell price is less than buy price.
                price_condition = order.price <= orderIter->price;
            }
            else
            {
                // order is a buy order, orderIter is walking through sell orders in the book.
                // so we process any order in the book where sell price is less than buy price.
                price_condition = order.price >= orderIter->price;
            }

            if (!price_condition)
                break;

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

            orderIter++; // walk orders in the same bucket
            if ( orderIter == priceBucketIter->end() )
            {
                priceBucketIter++;
                if ( priceBucketIter == asks_end() )
                    break;
            }

        }

        for ( auto &i : orders_to_remove )
            book.removeOrder(i);

        return volume;
    }

    IBookType m_buyBook;
    IBookType m_sellBook;

};


#endif //ORDERBOOK_ORDERBOOK_HPP
