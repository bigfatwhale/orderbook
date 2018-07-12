
#include "Order.h"
#include "server.h"
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
      std::cerr << "Usage: simple_server <port>\n";
      return 1;
    }

    io_context io_context;
    
    tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[1])); 

    OrderBookServer server( io_context, endpoint );
    // OrderBookClient c(io_context, endpoints);

    // thread t{ [&io_context](){ io_context.run(); }};

    // for (int i = 0; i < 10; i++)
    // {
    //     Order order(2001 + i, 10000, 100, BookType::BUY, 20001);
    //     c.sendOrder(order);
    //     this_thread::sleep_for(std::chrono::seconds(1));
    // }
    // c.close();

    // t.join();
}
