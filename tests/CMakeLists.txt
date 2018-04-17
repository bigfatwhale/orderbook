cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
include_directories(${BOOST_ROOT} )

add_executable(pitch_parser_test pitch_parser_test.cpp)

set(CMAKE_CXX_FLAGS "-O3 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")

target_link_libraries( pitch_parser_test bats_pitch_parser )
