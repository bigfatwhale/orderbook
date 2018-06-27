
#include <boost/asio.hpp>
#include "client.h"
#include "protocol.h"

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;

OrderBookClient::OrderBookClient(io_context &io_context, 
                                 const tcp::resolver::results_type& endpoints) : 
    m_io_context{io_context}, m_socket{io_context}
{
    do_connect(endpoints);
}

void OrderBookClient::do_connect(const tcp::resolver::results_type& endpoints)
{
    async_connect(m_socket, endpoints, 
        [this](error_code ec, tcp::endpoint)
        {
            if (!ec)
            {
                read_header();
            }
        }
    );
}

void OrderBookClient::read_header()
{

    async_read(m_socket, buffer(m_buffer, protocol::header_length),
        [this]( error_code ec, std::size_t)
        {
            if (!ec)
            {
                uint16_t msg_type = *reinterpret_cast<uint16_t *>(m_buffer);
                uint16_t msg_len  = *reinterpret_cast<uint16_t *>(m_buffer + sizeof(uint16_t));
                read_body(msg_type, msg_len);
            }
            else
            {
                m_socket.close();
            }

        }
    );
}

void OrderBookClient::read_body(uint16_t msg_type, uint16_t msg_len)
{
    async_read(m_socket, buffer(m_buffer, msg_len),
        [this, msg_type]( error_code ec, std::size_t)
        {
            if (!ec)
            {
                switch(msg_type)
                {
                    case protocol::message_type::CLIENT_ID:
                        m_client_id = *reinterpret_cast<uint32_t *>(m_buffer);
                        break;
                }
                read_header();
            }
            else
            {
                m_socket.close();
            }

        }
    );
}