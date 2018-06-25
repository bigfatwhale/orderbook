
#ifndef ORDERBOOK_NET_CLIENT_H
#define ORDERBOOK_NET_CLIENT_H

#include <boost/asio.hpp>

class OrderBookClient 
{

public:

    OrderBookClient(boost::asio::io_context &io_context, 
                    const boost::asio::ip::tcp::resolver::results_type& endpoints);
    void do_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints);
    void do_read_session_id();
private:
    boost::asio::io_context &m_io_context;
    boost::asio::ip::tcp::socket m_socket;
};


#endif