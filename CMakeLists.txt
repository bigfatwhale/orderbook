cmake_minimum_required(VERSION 3.7)
project(bats_pitch_parser VERSION 0.01 DESCRIPTION "BATS PITCH parser")

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-O3 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")

set(SOURCE_FILES BATSUtil.h BATSTradeBreakMsg.hpp BATSTradeMsg.hpp BATSTradingStatusMsg.hpp
        BATSOrderCancelMsg.hpp BATSOrderExecutedMsg.hpp BATSRetailPriceImproveMsg.hpp BATSMessageBase.cpp
        BATSMessageBase.h BATSMsgFactory.cpp BATSMsgFactory.h BATSAddOrderMsg.hpp BATSAuctionSummaryMsg.hpp
        BATSAuctionUpdateMsg.hpp BATSPitchMsgParser.cpp BATSPitchMsgParser.h)
#add_executable(bats_pitch_parser ${SOURCE_FILES})
add_library(bats_pitch_parser SHARED ${SOURCE_FILES})
#include_directories($ENV{BOOST_ROOT})

enable_testing()
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories (${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ) 

file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} tests/*.cpp)
foreach(testSrc ${TEST_SRCS})
        get_filename_component(testName ${testSrc} NAME_WE)
        add_executable(${testName} ${testSrc})
		target_link_libraries(${testName} ${Boost_LIBRARIES} bats_pitch_parser)
        set_target_properties(${testName} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests)

        add_test(NAME ${testName} 
                 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests
                 COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tests/${testName} )
endforeach(testSrc)

set(TEST_FILES tests/pitch_data_100k tests/pitch_example_data)
foreach(testFile ${TEST_FILES})
        configure_file(${testFile} ${testFile} COPYONLY)
endforeach(testFile)
