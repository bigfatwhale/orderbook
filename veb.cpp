//
// Created by Chu Ming on 4/22/18.
//

#include <iostream>
#include <algorithm>
#include <boost/dynamic_bitset.hpp>
#include "veb.h"

using namespace std;

veb::veb(int numBits) : m_numBits{numBits},
                        m_min{-1},
                        m_max{-1},
                        m_lsb{m_numBits / 2},
                        m_msb{m_lsb + (m_numBits % 2)},
                        m_cluster_size{1 << m_msb}
{
    //cout << "creating veb(" << m_numBits << "), msb=" << m_msb << " lsb=" << m_lsb << endl;

    // usr(u) : upper square root = 2^( ceil( ( lg u / 2 ) ) )
    // lsr(u) : lower square root = 2^( floor( ( lg u / 2 ) ) )
    if (m_numBits > 1)
    {
        // initialize recursive data structure.
        m_cluster = new veb*[ m_cluster_size ];
        for (int i=0; i<m_cluster_size; i++)
            m_cluster[i] = new veb(m_lsb);
        m_summary = new veb(m_msb);
    }

}

veb::~veb()
{
    if (m_cluster != nullptr)
    {
        for (int i = 0; i < m_cluster_size; i++)
            delete m_cluster[i];
        delete[] m_cluster;
        delete m_summary;
    }
}

inline int veb::high(int x)
{
    // high(x) = floor( x / lsr(u) )
    return x >> m_lsb;
}

inline int veb::low(int x)
{
    return x & ( ( 1 << m_lsb ) - 1 );
}

inline int veb::index(int i, int j)
{
    return ( i << m_lsb ) + j;
}

inline void veb::empty_insert(veb &v, int x)
{
    v.m_min = v.m_max = x;
}

bool veb::isMember(int x) {
    if (m_min == x || m_max == x)
        return true;
    else if (m_numBits == 1)
        return false;
    else {
        int h = high(x);
        int l = low(x);
        return m_cluster[h]->isMember(l);
    }
}

void veb::insert(int x)
{
    if ( m_min == -1 ) // m_min == -1 means current veb structure is empty
        m_min = m_max = x; // inserting x just means setting its value the min and max element
    else
    {
        if (x < m_min)
            swap( x, m_min ); // x becomes the new min. after which we need to do insert for old min
        if (m_numBits > 1)
        {
            if ( m_cluster[ high(x) ]->m_min == -1 )
            {
                m_summary->insert( high(x) ); // mark dirty in summary bit vector
                empty_insert( *m_cluster[ high(x) ], low(x) );
            } else
                m_cluster[ high(x) ]->insert( low(x) );
        }
        if (x > m_max)
            m_max = x;
    }
}