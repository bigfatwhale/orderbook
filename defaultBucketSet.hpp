

#ifndef ORDERBOOK_DEFAULTBUCKETSET_H
#define ORDERBOOK_DEFAULTBUCKETSET_H

#include <cstdint>
#include <map>
#include <memory>
#include "PriceBucket.h"

template <typename PriceBucketT = PriceBucket>
class default_bucket_set
{
    // wrapper class which forward calls to std::map, just to conform with
    // our api contract with PriceBucketManager
    using bucket_set_t = std::map<uint64_t, std::shared_ptr<PriceBucketT>>;

public:

    std::shared_ptr<PriceBucketT> find( uint64_t price )
    { return m_map.find( price ) != m_map.end() ? m_map.find( price )->second : nullptr; }

    std::shared_ptr<PriceBucketT> successor( uint64_t price )
    { return m_map.upper_bound(price) != m_map.end() ? m_map.upper_bound(price)->second : nullptr; }

    std::shared_ptr<PriceBucketT> predecessor( uint64_t price )
    { return m_map.find(price) != m_map.begin() ? (--m_map.find(price))->second : nullptr; }

    std::shared_ptr<PriceBucketT> insert( typename bucket_set_t::value_type keyValPair )
    {
        // use the convenience of structured bindings offered by C++17
        auto [item, ok] = m_map.insert( keyValPair );
        return ok ? item->second : std::shared_ptr<PriceBucketT>();
    }

    void remove( uint64_t price ) { m_map.erase(price); }

    uint64_t minPrice() { return m_map.begin() == m_map.end() ? 0 : m_map.begin()->first; }
    uint64_t maxPrice() { return m_map.rbegin() == m_map.rend() ? 0 : m_map.rbegin()->first; }

private:
    bucket_set_t m_map;
};

#endif