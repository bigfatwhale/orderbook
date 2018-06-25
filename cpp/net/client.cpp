
#include "client.h"

OrderBookClient::OrderBookClient(boost::asio::io_context &io_context, 
                                 const boost::asio::ip::tcp::resolver::results_type& endpoints) : 
    m_io_context{io_context}, m_socket{io_context}
{
    do_connect(endpoints);
}

void OrderBookClient::do_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints)
{

    
}