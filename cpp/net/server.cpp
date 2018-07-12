
#include "protocol.h"
#include "server.h"
#include <utility>

using namespace boost::asio::ip;
using namespace boost::system;
using boost::asio::buffer;
using boost::asio::io_context;

ClientSession::ClientSession(tcp::socket socket) : m_socket(std::move(socket))
{
    // we get passed the socket that we should communicate on after tcp accept.
}

void ClientSession::start()
{
    read_header();
}

void ClientSession::read_header()
{

    async_read(m_socket, buffer(m_buffer, protocol::header_length),
        [this, self=shared_from_this() ]( error_code ec, std::size_t)
        {
            // for the captures above, this is for implicitly using m_buffer via
            // m_buffer, without the need for this->m_buffer. self is there for lifetime
            // extension via shared ptr mechanism.
            if (!ec)
            {
                uint16_t msg_type = *reinterpret_cast<uint16_t *>(m_buffer);
                uint16_t msg_len  = *reinterpret_cast<uint16_t *>(m_buffer + sizeof(uint16_t));
                self->read_body(msg_type, msg_len);
            }
            else
            {
                m_socket.close();
            }

        }
    );
}

void ClientSession::read_body(uint16_t msg_type, uint16_t msg_len)
{
    async_read(m_socket, buffer(m_buffer, msg_len),
        [this, msg_type, self=shared_from_this()] // self is for lifetime extension.
        ( error_code ec, std::size_t)
        {
            if (!ec)
            {
                switch(msg_type)
                {
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


OrderBookServer::OrderBookServer(io_context &io_context, 
                                 const tcp::endpoint& endpoint) :
                                 //m_io_context(io_context), m_socket(io_context), 
                                 m_acceptor(io_context, endpoint)
{
    do_accept();
}

void OrderBookServer::do_accept()
{
    m_acceptor.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket)
        {
            if (!ec) 
            {
                // hand off all subsequent comms to the session handler ClientSession.
                std::make_shared<ClientSession>(std::move(socket))->start();
            }

            do_accept(); // start listening for the next client again

        }

    );

}