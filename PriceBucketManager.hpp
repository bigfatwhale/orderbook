//
// Created by Uncle Chu on 9/5/18.
//

#ifndef ORDERBOOK_PRICEBUCKETMANAGER_HPP
#define ORDERBOOK_PRICEBUCKETMANAGER_HPP

#include <utility>
#include "PriceBucket.h"

template <typename SetT>
class PriceBucketManager
{
public:
    // manages a set of price buckets and
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

    SetT m_buckets;
};

#endif //ORDERBOOK_PRICEBUCKETMANAGER_HPP
