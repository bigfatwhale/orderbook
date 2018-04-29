cmake_minimum_required(VERSION 3.7)
project(bats_pitch_parser VERSION 0.01 DESCRIPTION "BATS PITCH parser")

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-O3 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")

set(SOURCE_FILES BATSUtil.h BATSTradeBreakMsg.hpp BATSTradeMsg.hpp BATSTradingStatusMsg.hpp
        BATSOrderCancelMsg.hpp BATSOrderExecutedMsg.hpp BATSRetailPriceImproveMsg.hpp BATSMessageBase.cpp
        BATSMessageBase.h BATSMsgFactory.cpp BATSMsgFactory.h BATSAddOrderMsg.hpp BATSAuctionSummaryMsg.hpp
        BATSAuctionUpdateMsg.hpp BATSPitchMsgParser.cpp BATSPitchMsgParser.h)
set(PYAPI_SOURCES BATSPythonAPI.cpp)
set(PYTHON_LIBRARY $ENV{PYTHON_LIBRARY})
set(PYTHON_INCLUDE_DIR $ENV{PYTHON_INCLUDE_DIR})
message( STATUS "PYTHON_LIBRARY=${PYTHON_LIBRARY}")
message( STATUS "PYTHON_INCLUDE_DIR=${PYTHON_INCLUDE_DIR}")

enable_testing()
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
find_package(Boost COMPONENTS python REQUIRED)
find_package(PythonLibs 2.7 REQUIRED)

include_directories (${PYTHON_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} )

add_library(bats_pitch_parser SHARED ${SOURCE_FILES})
target_link_libraries(bats_pitch_parser python2.7 boost_python)

add_library(bats_api SHARED ${PYAPI_SOURCES})

message( STATUS "CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
# fix for osx. python only loads .so
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(bats_api PROPERTIES PREFIX "")
    set_target_properties(bats_api PROPERTIES OUTPUT_NAME "bats_api")
    set_target_properties(bats_api PROPERTIES SUFFIX ".so")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set_target_properties(bats_api PROPERTIES PREFIX "")	
endif()

target_link_libraries(bats_api python2.7 boost_python bats_pitch_parser)


file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} tests/*.cpp)
foreach(testSrc ${TEST_SRCS})
        get_filename_component(testName ${testSrc} NAME_WE)
        add_executable(${testName} ${testSrc})
		target_link_libraries(${testName} ${Boost_LIBRARIES} bats_pitch_parser boost_unit_test_framework)
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
