#include <boost/asio.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <iostream>
#include "client.h"
#include "protocol.h"

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive &ar, Order o, const unsigned int version)
{
    ar & o.orderId;
    ar & o.price;
    ar & o.volume;
    ar & o.partId;
    ar & o.side;
}

} // namespace serialization
} // namespace boost

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
                    // get the assigned client id from the server and keep it.
                    // we might want to tag all of our out-going messages with it, 
                    // although at the moment there's no code to do that yet.
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

void OrderBookClient::write( const Order& order )
{
    post(m_io_context, 
        [this, order]()
        {
            bool write_in_progress = !m_write_queue.empty();
            m_write_queue.push_back(order);
            if (!write_in_progress)
            {
                do_write();
            }
        }    
    );
}

void OrderBookClient::do_write()
{

    boost::asio::streambuf buf;
    std::ostream os(&buf);
    boost::archive::binary_oarchive out_archive(os);

    out_archive << protocol::message_type::ADD_ORDER // type
                << buf.size()                        // length
                << m_write_queue.front();            // value

    async_write(m_socket, buf, 
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                m_write_queue.pop_front();
                if (!m_write_queue.empty())
                {
                    do_write();
                }
            }
            else
                m_socket.close();
        }
    );   
}