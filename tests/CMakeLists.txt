cmake_minimum_required(VERSION 3.7)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
include_directories(${BOOST_ROOT} )
#target_include_directories(pitch_parser_test ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(pitch_parser_test pitch_parser_test.cpp)

#target_link_libraries( pitch_parser_test bats_pitch_parser )
target_link_libraries(pitch_parser_test)
#include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})