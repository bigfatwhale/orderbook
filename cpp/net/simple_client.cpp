
#include "client.h"
#include "Order.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>

using namespace std;
using namespace boost::asio;
using boost::asio::ip::tcp;


int main(int argc, char* argv[])
{

    if (argc != 3)
    {
      std::cerr << "Usage: simple_client <host> <port>\n";
      return 1;
    }

    io_context io_context;
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(argv[1], argv[2]);
    OrderBookClient c(io_context, endpoints);

    thread t{ [&io_context](){ io_context.run(); }};

    for (int i = 0; i < 10; i++)
    {
        Order order(2001 + i, 10000, 100, BookType::BUY, 20001);
        c.sendOrder(order);
        this_thread::sleep_for(std::chrono::seconds(1));
    }
    c.close();

    t.join();
}
