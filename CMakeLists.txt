
cmake_minimum_required(VERSION 3.7)
project(orderbook)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES main.cpp)

add_executable(orderbook ${SOURCE_FILES})
include_directories($ENV{BOOST_ROOT})