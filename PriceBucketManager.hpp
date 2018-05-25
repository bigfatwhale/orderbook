//
// Created by Uncle Chu on 9/5/18.
//

#ifndef ORDERBOOK_PRICEBUCKETMANAGER_HPP
#define ORDERBOOK_PRICEBUCKETMANAGER_HPP

#include <map>
#include <utility>
#include <memory>
#include <iostream>
#include <type_traits>
#include <boost/iterator/iterator_facade.hpp>
#include "Order.h"
#include "PriceBucket.h"

template <typename PriceBucketT = PriceBucket>
class default_bucket_set
{
    // wrapper class which forward calls to std::map, just to conform with
    // our api contract with PriceBucketManager
    using bucket_set_t = std::map<uint64_t, std::shared_ptr<PriceBucketT>>;

public:

    std::shared_ptr<PriceBucketT> find( uint64_t price )
    {
        auto item = m_map.find( price );
        if ( item != m_map.end() )
            return item->second;
        else
            return std::shared_ptr<PriceBucketT>();
    }

    std::shared_ptr<PriceBucketT> successor( uint64_t price )
    {
        auto item = m_map.upper_bound( price );
        if ( item != m_map.end() )
            return item->second;
        else
            return std::shared_ptr<PriceBucketT>();
    }

    std::shared_ptr<PriceBucketT> predecessor( uint64_t price )
    {
        auto item = m_map.find( price );
        if ( item != m_map.begin() )
        {
            item--;
            return item->second;
        }
        return std::shared_ptr<PriceBucketT>();
    }

    std::shared_ptr<PriceBucketT> insert( typename bucket_set_t::value_type keyValPair )
    {
        // use the convenience of structured bindings offered by C++17
        auto [item, ok] = m_map.insert( keyValPair );
        return ok ? item->second : std::shared_ptr<PriceBucketT>();
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

template < typename BucketSetT=default_bucket_set<>,
           typename PriceBucketT=PriceBucket >
class PriceBucketManager
{
public:
    // manages a set of price buckets and allows us to
    // 1. find some bucket for a specifed price-level
    // 2. find the successor bucket for the specified price-level
    // 3. find the predecessor bucket for the specified price-level
    // 4. add a new price-bucket for a given price-level

    std::shared_ptr<PriceBucketT>
    findBucket( uint64_t price ) { return m_buckets.find(price); }

    std::shared_ptr<PriceBucketT>
    nextBucket( uint64_t price ) { return m_buckets.successor(price); }

    std::shared_ptr<PriceBucketT>
    prevBucket( uint64_t price ) { return m_buckets.predecessor(price); }

    std::shared_ptr<PriceBucketT> addBucket( uint64_t price )
    {
        auto bucket = std::make_shared<PriceBucketT>(price);
        auto ret = m_buckets.insert(std::make_pair(price, bucket));
        return ret;
    }

    void removeBucket( uint64_t price ) { m_buckets.remove(price); }

    uint64_t minPrice() { return m_buckets.minPrice(); }
    uint64_t maxPrice() { return m_buckets.maxPrice(); }

    // BucketSetT must be a type which supports find/successor/predecessor/insert/min/max
    // By default we use a wrapper around std::map (log n lookups). We also can
    // switch in a veb-based set type (log(log(u)) lookups) and compare the performance.
    BucketSetT m_buckets;

    typedef decltype( &PriceBucketManager<BucketSetT,PriceBucketT>::nextBucket ) ptrIncDecMember;

    friend class iterator;

    template <typename T, typename U> using EnableIfAsk = std::enable_if_t< std::is_same_v<T, Ask>, U >;
    template <typename T, typename U> using EnableIfBid = std::enable_if_t< std::is_same_v<T, Bid>, U >;

    template <typename AskBidTrait>
    class iterator :
            public boost::iterator_facade<
                    iterator<AskBidTrait>,     // boost::iterator_facade uses CRTP.
                    PriceBucketT, // what we are iterating over
                    boost::bidirectional_traversal_tag > // type of iterator
    {
    public:
        iterator( PriceBucketManager& pbm, bool isEnd=false) : m_priceBucketManager{pbm}
        {
            m_price = isEnd ? 0 : initialPrice();
        }

    private:
        friend class boost::iterator_core_access;

        template <typename U = AskBidTrait>
        EnableIfAsk<U, uint64_t > initialPrice() { return m_priceBucketManager.minPrice(); }

        template <typename U = AskBidTrait>
        EnableIfBid<U, uint64_t > initialPrice() { return m_priceBucketManager.maxPrice(); }

        // increment/decrement function pair if we are in Ask book
        template <typename U = AskBidTrait>
        EnableIfAsk<U, void> increment()
        {
            auto bucket = m_priceBucketManager.nextBucket(m_price);
            m_price = bucket == nullptr ? 0 : bucket->m_pricePoint;
        }

        template <typename U = AskBidTrait>
        EnableIfAsk<U, void> decrement()
        {
            auto bucket = m_priceBucketManager.prevBucket(m_price);
            m_price = bucket == nullptr ? 0 : bucket->m_pricePoint;
        }
        // end increment/decrement function pair

        // increment/decrement function pairs if we are in Bid book
        template <typename U = AskBidTrait>
        EnableIfBid<U, void> increment()
        {
            auto bucket = m_priceBucketManager.prevBucket(m_price);
            m_price = bucket == nullptr ? 0 : bucket->m_pricePoint;
        }

        template <typename U = AskBidTrait>
        EnableIfBid<U, void> decrement()
        {
            auto bucket = m_priceBucketManager.nextBucket(m_price);
            m_price = bucket == nullptr ? 0 : bucket->m_pricePoint;
        }
        // end increment/decrement function pair

        bool equal( iterator const& other ) const
        {
            return this->m_price == other.m_price;
        }

        PriceBucketT& dereference() const
        {
            std::shared_ptr<PriceBucketT> p_bucket = m_priceBucketManager.findBucket(m_price);
            return *p_bucket;
        }

        uint32_t m_price; // which price point we are currently at.
        PriceBucketManager& m_priceBucketManager;

        ptrIncDecMember m_incFunc;
        ptrIncDecMember m_decFunc;

    };

    template <typename AskBidTrait>
    iterator<AskBidTrait> begin()
    {
        return iterator<AskBidTrait>( *this );
    }

    template <typename AskBidTrait>
    iterator<AskBidTrait> end()
    {
        return iterator<AskBidTrait>( *this, true );
    }

};

#endif //ORDERBOOK_PRICEBUCKETMANAGER_HPP
