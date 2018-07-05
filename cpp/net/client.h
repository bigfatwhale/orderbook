
#ifndef ORDERBOOK_NET_CLIENT_H
#define ORDERBOOK_NET_CLIENT_H

#include <deque>
#include <boost/asio.hpp>
#include "Order.h"

class OrderBookClient 
{

public:

    OrderBookClient(boost::asio::io_context &io_context, 
                    const boost::asio::ip::tcp::resolver::results_type& endpoints);
    void do_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints);
    void read_header();
    void read_body(uint16_t msg_type, uint16_t msg_len);
    void write( const Order &order );
    void do_write();
    
private:
    boost::asio::io_context &m_io_context;
    boost::asio::ip::tcp::socket m_socket;

    char m_buffer[1024];
    uint32_t m_client_id;
    std::deque<Order> m_write_queue;
};


#endif