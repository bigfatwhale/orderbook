//
// Created by Chu Ming on 4/22/18.
//

#ifndef ORDERBOOK_VEB_H
#define ORDERBOOK_VEB_H

#include <memory>
#include <boost/core/noncopyable.hpp>

template <typename T>
struct ptr_array_deleter
{
    void operator()(T* arr)
    {
        delete [] arr;
    }
};

template <typename T> // template specialization for T*[]
struct ptr_array_deleter<T*[]>
{
    ptr_array_deleter(int size=0) : m_size{size} {}
    void operator()(T* arr[])
    {
        //for ( auto x : arr )
        for (int i=0; i<m_size; i++)
            delete arr[i];
        delete [] arr;
    }
    int m_size;
};

// van Emde Boas data structure. solves the fixed universe successor problem.
// we use this to find the next / previous non empty price bucket.
class veb : private boost::noncopyable
{
    /* we make this class noncopyable because it's a recursive data structure with
     * pointers to numerous smaller copies of itself maintained using list of pointers
     * to those smaller versions. copying won't make sense if we just copy the pointers,
     * and we also don't want to copy because the structure is quite big (usually).
     */

public:
    veb( int numBits ); // size of universe will be root_u ^ 2
    ~veb();

    int high( int x );
    int low( int x );
    int index(int i, int j);

    void insert(int x);
    void empty_insert( veb & v, int x);
    void remove(int x);
    bool isMember(int x);
    int  successor( int x );
    int  predecessor( int x );

public:
    int m_numBits;

    int m_min;
    int m_max;
    int m_lsb;
    int m_msb;

    int m_cluster_size; // cluster size is upper square root u
    //veb **m_cluster{nullptr};
    //veb *m_summary{nullptr};
    std::unique_ptr<veb*[], ptr_array_deleter<veb*[]>>
            m_cluster{nullptr, ptr_array_deleter<veb*[]>()}; // how to avoid this ugliness?
    std::unique_ptr<veb> m_summary{nullptr};


};


#endif //ORDERBOOK_VEB_H
