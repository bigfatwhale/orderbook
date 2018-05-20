//
// Created by Uncle Chu on 9/5/18.
//

#ifndef ORDERBOOK_PRICEBUCKETMANAGER_HPP
#define ORDERBOOK_PRICEBUCKETMANAGER_HPP

#include <map>
#include <utility>
#include <memory>
#include <iostream>
#include <boost/function.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include "PriceBucket.h"

class default_bucket_set
{
    // wrapper class which forward calls to std::map, just to conform with
    // our api contract with PriceBucketManager
    using bucket_set_t = std::map<uint64_t, std::shared_ptr<PriceBucket>>;

public:

    std::shared_ptr<PriceBucket> find( uint64_t price )
    {
        auto item = m_map.find( price );
        if ( item != m_map.end() )
            return item->second;
        else
            return std::shared_ptr<PriceBucket>();
    }

    std::shared_ptr<PriceBucket> successor( uint64_t price )
    {
        auto item = m_map.upper_bound( price );
        if ( item != m_map.end() )
            return item->second;
        else
            return std::shared_ptr<PriceBucket>();
    }

    std::shared_ptr<PriceBucket> predecessor( uint64_t price )
    {
        auto item = m_map.find( price );
        if ( item != m_map.begin() )
        {
            item--;
            return item->second;
        }
        return std::shared_ptr<PriceBucket>();
    }

    std::shared_ptr<PriceBucket> insert( bucket_set_t::value_type keyValPair )
    {
        // use the convenience of structured bindings offered by C++17
        auto [item, ok] = m_map.insert( keyValPair );
        return ok ? item->second : std::shared_ptr<PriceBucket>();
    }

    void remove( uint64_t price ) { m_map.erase(price); }

    uint64_t minPrice()
    {
        auto it = m_map.begin();
        return it == m_map.end() ? 0 : it->first;
    }

    uint64_t maxPrice()
    {
        auto it = m_map.rbegin();
        return it == m_map.rend() ? 0 : it->first;
    }

private:
    bucket_set_t m_map;
};

template < typename BucketSetT=default_bucket_set >
class PriceBucketManager
{
public:
    // manages a set of price buckets and allows us to
    // 1. find some bucket for a specifed price-level
    // 2. find the successor bucket for the specified price-level
    // 3. find the predecessor bucket for the specified price-level
    // 4. add a new price-bucket for a given price-level

    //PriceBucketManager(BookType t) : m_bookType{t}
    //{
        // unfortunately we need to know book type (BUY or SELL) to do iterators.
        // if it's BUY, we iterate from highest to lowest, and the reverse for SELL.
    //}

    std::shared_ptr<PriceBucket>
    findBucket( uint64_t price ) { return m_buckets.find(price); }

    std::shared_ptr<PriceBucket>
    nextBucket( uint64_t price ) { return m_buckets.successor(price); }

    std::shared_ptr<PriceBucket>
    prevBucket( uint64_t price ) { return m_buckets.predecessor(price); }

    std::shared_ptr<PriceBucket> addBucket( uint64_t price )
    {
        auto bucket = std::make_shared<PriceBucket>(price);
        auto ret = m_buckets.insert(std::make_pair(price, bucket));
        return ret;
    }

    void removeBucket( uint64_t price ) { m_buckets.remove(price); }

    uint64_t minPrice() { return m_buckets.minPrice(); }
    uint64_t maxPrice() { return m_buckets.maxPrice(); }

    // SetT must be a type which supports find/successor/predecessor/insert/min/max
    // By default we use a wrapper around std::map (log n lookups). We also can
    // switch in a veb-based set type (log(log(u)) lookups) and compare the performance.
    BucketSetT m_buckets;
    //BookType   m_bookType;

    //
    //
    //
    friend class iterator;

    // boost::iterator_facade uses CRTP.
    class iterator :
            public boost::iterator_facade<
                    iterator,    // CRTP
                    PriceBucket, // what we are iterating over
                    boost::bidirectional_traversal_tag >// type of iterator
    {
    public:
        iterator() : m_price{0} {}
        iterator( PriceBucketManager& pbm, uint32_t price, BookType t) :
                 m_price{price}, m_priceBucketManager{pbm},
                 m_incFunc{t == BookType::BUY ?
                           &PriceBucketManager::prevBucket : &PriceBucketManager::nextBucket},
                 m_decFunc{t == BookType::BUY ?
                           &PriceBucketManager::nextBucket : &PriceBucketManager::prevBucket}
        {
        }

    private:
        friend class boost::iterator_core_access;

        void increment()
        {
            auto bucket = m_incFunc( &m_priceBucketManager, m_price);
            m_price = bucket == nullptr ? 0 : bucket->m_pricePoint;
        }

        void decrement()
        {
            auto bucket = m_decFunc( &m_priceBucketManager, m_price);
            m_price = bucket == nullptr ? 0 : bucket->m_pricePoint;
        }

        bool equal( iterator const& other ) const
        {
            return this->m_price == other.m_price;
        }

        PriceBucket& dereference() const
        {
            std::shared_ptr<PriceBucket> p_bucket = m_priceBucketManager.findBucket(m_price);
            return *p_bucket;
        }

        uint32_t m_price; // which price point we are currently at.
        PriceBucketManager& m_priceBucketManager;
        boost::function< std::shared_ptr<PriceBucket> (PriceBucketManager<BucketSetT>*, uint64_t) > m_incFunc;
        boost::function< std::shared_ptr<PriceBucket> (PriceBucketManager<BucketSetT>*, uint64_t) > m_decFunc;

    };

    iterator begin(BookType t = BookType::BUY)
    {
        auto bestPrice = t == BookType::BUY ? maxPrice() : minPrice();
        return iterator( *this, bestPrice, t );
    }

    iterator end(BookType t = BookType::BUY)
    {
        return iterator( *this, 0, t );
    }

};

#endif //ORDERBOOK_PRICEBUCKETMANAGER_HPP
