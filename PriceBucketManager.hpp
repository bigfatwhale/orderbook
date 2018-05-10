//
// Created by Uncle Chu on 9/5/18.
//

#ifndef ORDERBOOK_PRICEBUCKETMANAGER_HPP
#define ORDERBOOK_PRICEBUCKETMANAGER_HPP

#include <map>
#include <utility>
#include <memory>
#include "PriceBucket.h"

class default_map
{
    // wrapper class which forward calls to std::map, just to conform with
    // our api contract with PriceBucketManager

    std::shared_ptr<PriceBucket> findBucket( uint64_t price )
    {
        auto item = m_map.find(price);
        if ( item != m_map.end() )
            return item->second;
        else
            return std::shared_ptr<PriceBucket>();
    }


    std::map<uint64_t, std::shared_ptr<PriceBucket>> m_map;
};

template <typename SetT>
class PriceBucketManager
{
public:
    // manages a set of price buckets and allows us to
    // 1. find some bucket for a specifed price-level
    // 2. find the successor bucket for the specified price-level
    // 3. find the predecessor bucket for the specified price-level
    // 4. add a new price-bucket for a given price-level

    std::shared_ptr<PriceBucket> findBucket( uint64_t price )
    {
        return m_buckets.find(price);
    }

    std::shared_ptr<PriceBucket> nextBucket( uint64_t price )
    {
        return m_buckets.successor(price);
    }

    std::shared_ptr<PriceBucket> prevBucket( uint64_t price )
    {
        return m_buckets.predecessor(price);
    }

    std::shared_ptr<PriceBucket> addBucket( uint64_t price )
    {
        auto bucket = std::make_shared<PriceBucket>(price);
        m_buckets.insert(std::make_pair(price, bucket));
        return bucket;
    }

    // SetT must be a type which supports find/successor/predecessor/insert
    SetT m_buckets;
};

#endif //ORDERBOOK_PRICEBUCKETMANAGER_HPP
