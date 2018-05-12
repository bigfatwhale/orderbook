
cmake_minimum_required(VERSION 3.7)
project(orderbook VERSION 0.01 DESCRIPTION "Limit Order Book")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
set(SOURCE_FILES OrderBook.hpp veb.cpp veb.h PriceBucket.cpp PriceBucket.h Order.h PriceBucketManager.hpp )
set(LOBSTER_SOURCE_FILES lobster/AddOrderMsg.hpp lobster/MessageBase.cpp lobster/MessageBase.h lobster/MsgParser.cpp lobster/MsgParser.h lobster/MsgFactory.cpp lobster/MsgFactory.h lobster/CancelOrderMsg.hpp lobster/DeleteOrderMsg.hpp lobster/OrderExecutedMsg.hpp)

add_library(lobster SHARED ${LOBSTER_SOURCE_FILES})
add_library(orderbook SHARED ${SOURCE_FILES})

enable_testing()

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories (${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} )

file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} tests/*.cpp)
foreach(testSrc ${TEST_SRCS})
    get_filename_component(testName ${testSrc} NAME_WE)
    add_executable(${testName} ${testSrc})
    target_link_libraries(${testName} ${Boost_LIBRARIES} orderbook lobster)
    set_target_properties(${testName} PROPERTIES
                            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests)

    add_test(NAME ${testName}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests
                COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tests/${testName} )
endforeach(testSrc)