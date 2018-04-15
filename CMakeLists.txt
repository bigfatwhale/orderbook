cmake_minimum_required(VERSION 3.7)
project(bats_pitch_parser)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-O3 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")

set(SOURCE_FILES BATSUtil.h BATSTradeBreakMsg.hpp BATSTradeMsg.hpp BATSTradingStatusMsg.hpp
        BATSOrderCancelMsg.hpp BATSOrderExecutedMsg.hpp BATSRetailPriceImproveMsg.hpp BATSMessageBase.cpp
        BATSMessageBase.h BATSMsgFactory.cpp BATSMsgFactory.h BATSAddOrderMsg.hpp BATSAuctionSummaryMsg.hpp
        BATSAuctionUpdateMsg.hpp tests/pitch_parser_test.cpp BATSPitchMsgParser.cpp BATSPitchMsgParser.h)
add_executable(bats_pitch_parser ${SOURCE_FILES})
include_directories($ENV{BOOST_ROOT})