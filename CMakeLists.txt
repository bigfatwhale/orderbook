
cmake_minimum_required(VERSION 3.7)
project(orderbook VERSION 0.01 DESCRIPTION "Limit Order Book")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")

set(SOURCE_FILES cpp/OrderBook.hpp cpp/veb.cpp cpp/veb.h cpp/vebBucketSet.hpp cpp/defaultBucketSet.hpp 
				 cpp/PriceBucket.cpp cpp/PriceBucket.h cpp/Order.h cpp/PriceBucketManager.hpp )

set(LOBSTER_SOURCE_FILES cpp/lobster/AddOrderMsg.hpp cpp/lobster/MessageBase.cpp cpp/lobster/MessageBase.h 
						 cpp/lobster/MsgParser.cpp cpp/lobster/MsgParser.h cpp/lobster/MsgFactory.cpp 
						 cpp/lobster/MsgFactory.h cpp/lobster/CancelOrderMsg.hpp cpp/lobster/DeleteOrderMsg.hpp 
						 cpp/lobster/OrderExecutedMsg.hpp cpp/lobster/AuctionTradeMsg.hpp cpp/lobster/TradeHaltMsg.hpp)

set(BATS_SOURCE_FILES cpp/bats/BATSUtil.h cpp/bats/BATSTradeBreakMsg.hpp cpp/bats/BATSTradeMsg.hpp 
					  cpp/bats/BATSTradingStatusMsg.hpp cpp/bats/BATSOrderCancelMsg.hpp cpp/bats/BATSOrderExecutedMsg.hpp 
					  cpp/bats/BATSRetailPriceImproveMsg.hpp cpp/bats/BATSMessageBase.cpp cpp/bats/BATSMessageBase.h 
					  cpp/bats/BATSMsgFactory.cpp cpp/bats/BATSMsgFactory.h cpp/bats/BATSAddOrderMsg.hpp 
					  cpp/bats/BATSAuctionSummaryMsg.hpp cpp/bats/BATSAuctionUpdateMsg.hpp cpp/bats/BATSPitchMsgParser.cpp 
					  cpp/bats/BATSPitchMsgParser.h)

set(PYAPI_BATS_SOURCES cpp/bats/BATSPythonAPI.cpp)

set(PYAPI_LOB_SOURCES cpp/OrderBookPythonAPI.cpp)

message( STATUS "CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")

# figure out python paths
execute_process(COMMAND python-config "--prefix"
	OUTPUT_VARIABLE PYTHON_PREFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(PYTHON_INCLUDE_DIR "${PYTHON_PREFIX}/include/python2.7")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(PYTHON_LIBRARY "${PYTHON_PREFIX}/lib/libpython2.7.dylib")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(PYTHON_LIBRARY "${PYTHON_PREFIX}/lib/libpython2.7.so")
endif ()

message( STATUS "Generating global includes")

find_package(PythonLibs REQUIRED)
find_package(PythonInterp REQUIRED)
find_package(Boost COMPONENTS python27 iostreams unit_test_framework system thread REQUIRED)
include_directories (${PYTHON_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/cpp ${CMAKE_SOURCE_DIR}/cpp/bats 
					 ${CMAKE_SOURCE_DIR}/cpp/lobster)

message( STATUS "Boost Boost_LIBRARIES=${Boost_LIBRARIES}")
message( STATUS "Boost Boost_PYTHON27_LIBRARIES=${Boost_PYTHON27_LIBRARIES}")
message( STATUS "Generating bats_pitch_parser library")
add_library(bats_pitch_parser SHARED ${BATS_SOURCE_FILES})
target_link_libraries(bats_pitch_parser ${Boost_LIBRARIES})
#target_link_libraries(bats_pitch_parser ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})

message( STATUS "Generating Python bats api library")
add_library(bats_api SHARED ${PYAPI_BATS_SOURCES})

# fix for osx. python only loads .so
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(bats_api PROPERTIES PREFIX "")
    set_target_properties(bats_api PROPERTIES OUTPUT_NAME "bats_api")
    set_target_properties(bats_api PROPERTIES SUFFIX ".so")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set_target_properties(bats_api PROPERTIES PREFIX "")
endif()

target_link_libraries(bats_api bats_pitch_parser ${Boost_PYTHON27_LIBRARIES} ${PYTHON_LIBRARY})

message( STATUS "Generating Python orderbook api library")
add_library(orderbook_api SHARED ${PYAPI_LOB_SOURCES})

# fix for osx. python only loads .so
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(orderbook_api PROPERTIES PREFIX "")
    set_target_properties(orderbook_api PROPERTIES OUTPUT_NAME "orderbook_api")
    set_target_properties(orderbook_api PROPERTIES SUFFIX ".so")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set_target_properties(orderbook_api PROPERTIES PREFIX "")
endif()

target_link_libraries(orderbook_api orderbook boost_system ${Boost_PYTHON27_LIBRARIES} ${PYTHON_LIBRARY} )

add_library(lobster SHARED ${LOBSTER_SOURCE_FILES})
add_library(orderbook SHARED ${SOURCE_FILES})

enable_testing()

file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} tests/*.cpp)
foreach(testSrc ${TEST_SRCS})
    get_filename_component(testName ${testSrc} NAME_WE)
    add_executable(${testName} ${testSrc})
    target_link_libraries(${testName} ${Boost_LIBRARIES} ${PYTHON_LIBRARY} orderbook lobster bats_pitch_parser)
    set_target_properties(${testName} PROPERTIES
                            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests)

    add_test(NAME ${testName}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests
                COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tests/${testName} )
endforeach(testSrc)

file(GLOB PY_TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} tests/*.py)
foreach(pytestSrc ${PY_TEST_SRCS})
    get_filename_component(pytestName ${pytestSrc} NAME_WE)
    add_test(NAME ${pytestName} 
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests
         COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${pytestSrc} )
endforeach(pytestSrc)

set(PY_SO_FILES bats_api orderbook_api)

foreach(so_file ${PY_SO_FILES})
ADD_CUSTOM_COMMAND(
    TARGET ${so_file}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_BINARY_DIR}/${so_file}.so 
    ${CMAKE_CURRENT_BINARY_DIR}/tests/${so_file}.so )
endforeach(so_file)

set(TEST_FILES tests/AAPL_2012-06-21_message_50.csv tests/AAPL_2012-06-21_orderbook_50.csv
			   tests/pitch_data_100k tests/pitch_example_data ${PY_TEST_SRCS} )

foreach(testFile ${TEST_FILES})
    configure_file(${testFile} ${testFile} COPYONLY)
endforeach(testFile)