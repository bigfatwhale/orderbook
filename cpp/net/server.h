

#ifndef ORDERBOOK_NET_SERVER_H
#define ORDERBOOK_NET_SERVER_H

#include "OrderBook.hpp"
#include <memory>
#include <boost/asio.hpp>

class ClientSession : std::enable_shared_from_this<ClientSession> // CRTP
{
public:
    ClientSession(boost::asio::ip::tcp::socket socket);

    void start();

private:
    void read_header();
    void read_body(uint16_t msg_type, uint16_t msg_len);

    char m_buffer[1024];
    boost::asio::ip::tcp::socket m_socket;
    
};


class OrderBookServer
{
public:

    OrderBookServer( boost::asio::io_context &io_context, 
                     const boost::asio::ip::tcp::endpoint& endpoint ); 

private:

    void do_accept();

    boost::asio::ip::tcp::acceptor m_acceptor;

    LimitOrderBook<PriceBucketManager<>> m_lob;
};

#endif //ORDERBOOK_NET_CLIENT_H 