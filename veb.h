//
// Created by Chu Ming on 4/22/18.
//

#ifndef ORDERBOOK_VEB_H
#define ORDERBOOK_VEB_H

// van Emde Boas data structure. solves the fixed universe successor problem.
// we use this to find the next / previous non empty price bucket.
class veb
{

public:
    veb( int numBits ); // size of universe will be root_u ^ 2
    ~veb();

    int high( int x );
    int low( int x );
    int index(int i, int j);

    void insert(int x);
    void empty_insert( veb & v, int x);
//private:
public:
    int m_numBits;
    //std::vector< boost::dynamic_bitset<> > m_clusters;
    //boost::dynamic_bitset<> m_summary;

    int m_min;
    int m_max;
    int m_lsb;
    int m_msb;

    int m_cluster_size; // cluster size is upper square root u
    veb **m_cluster{nullptr};
    veb *m_summary{nullptr};
};


#endif //ORDERBOOK_VEB_H
