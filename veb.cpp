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

bool veb::isMember(int x)
{
    if (m_min == x || m_max == x)
        return true;
    else if (m_numBits == 1)
        return false;
    else
        return m_cluster[high(x)]->isMember(low(x));
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

void veb::remove(int x)
{
    // pre-condition : x is a member of the veb
    if ( m_min == m_max ) // the member pre-cond, plus the fact that min and max are same,
        m_min = m_max = -1; // it means x must be the value of min and max. so we remove it (set to -1)
    else if ( m_numBits == 1 )
    {
        // if we are here, it means min and max values are different, and there are two values in veb
        if (x == 0)
            m_min = 1;
        else
            m_min = 0;
        m_max = m_min; // now there's only 1 value in veb, so min and max have to be the same.
    }
    else
    {
        if ( x == m_min )
        {
            // if x is the min of the current veb, then we need to find the new min (call it k)
            // to take its place, then delete k instead, and set m_min = k
            int first_cluster = m_summary->m_min;

            // get the lower significant bits of the new minimum value from the first cluster
            int lower_bits = m_cluster[first_cluster]->m_min;

            // next get the new value for x, which is the new min. the actual number is assembled
            // from the msb value (=first_cluster) * lower sq root + lower_bits. this is what the
            // veb::index method does.
            x = index( first_cluster, lower_bits );
            m_min = x; // update the new min the newly found min
        }

        // recursively delete x. at this point x can be the original value passed to remove, or the
        // new minimum that was found.
        m_cluster[ high(x) ]->remove( low(x) );

        // now check if our remove operation on high(x) cluster above created an empty veb. if so we need
        // to do more book-keeping.
        if ( m_cluster[ high(x) ]->m_min == -1 )
        {
            m_summary->remove( high(x) ); // remove high(x) from the summary structure.

            // after updating the summary in the above, we need to check if we deleted the max of the
            // current veb, if so, more book keeping updates.
            if ( x == m_max )
            {
                int summary_max = m_summary->m_max;
                if ( summary_max == -1 )
                    m_max = m_min;
                else
                    m_max = index( summary_max, m_cluster[summary_max]->m_max );
            }
        }
        else if ( x == m_max )
            m_max = index( high(x), m_cluster[ high(x) ]->m_max );
    }

}

int veb::successor(int x)
{
    // find the successor to x. returns -1 if there's no successor.
    if (m_numBits == 1)
        return ( x == 0 && m_max == 1 ) ? 1 : -1;
    else if ( m_min != -1 && x < m_min )
        return m_min;
    else
    {
        // max_low is the max element in x's cluster
        int max_low = m_cluster[ high(x) ]->m_max;
        if ( max_low != -1 && low(x) < max_low )
        {
            // if we are in here, we know that x's successor must be in the same cluster
            // as x. we recursively search inside that cluster to get the offset.
            int offset = m_cluster[ high(x) ]->successor( low(x) );
            return index( high(x), offset );
        }
        else
        {
            // else the successor for x must be in another successor-cluster. we consult the
            // summary veb to find it.
            int successor_cluster = m_summary->successor( high(x) );
            if ( successor_cluster == -1 )
                return -1;
            else
            {
                int offset = m_cluster[ successor_cluster ]->m_min;
                return index( successor_cluster, offset );
            }
        }
    }
}

int veb::predecessor(int x)
{
    if ( m_numBits == 1 )
        return ( x == 1 && m_min == 0 ) ? 0 : -1;
    else if ( m_max != -1 && x > m_max )
        return m_max;
    else
    {
        int min_low = m_cluster[ high(x) ]->m_min;
        if ( min_low != -1 && low(x) > min_low )
        {
            // if we are in here, we know that x's predecessor must be in the same cluster
            // as x. we recursively search inside that cluster to get the offset.
            int offset = m_cluster[ high(x) ]->predecessor( low(x) );
            return index( high(x), offset );
        }
        else
        {
            int pred_cluster = m_summary->predecessor( high(x) );
            if ( pred_cluster == -1 )
            {
                if ( m_min != -1 && x > m_min )
                    return m_min;
                else
                    return -1;
            }
            else
            {
                int offset = m_cluster[ pred_cluster ]->m_max;
                return index( pred_cluster, offset );
            }
        }
    }
}