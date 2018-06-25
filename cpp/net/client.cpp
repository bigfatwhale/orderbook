
#include <boost/asio.hpp>
#include "client.h"

using namespace boost::asio;
using namespace boost::asio::ip;

OrderBookClient::OrderBookClient(io_context &io_context, 
                                 const tcp::resolver::results_type& endpoints) : 
    m_io_context{io_context}, m_socket{io_context}
{
    do_connect(endpoints);
}

void OrderBookClient::do_connect(const tcp::resolver::results_type& endpoints)
{
    async_connect(m_socket, endpoints, 
        [this](boost::system::error_code ec, tcp::endpoint)
        {
            if (!ec)
            {
                do_read_session_id();
            }
        }
    );
}

void OrderBookClient::do_read_session_id()
{

}