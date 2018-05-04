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

message( STATUS "CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")

# python paths

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

#message( STATUS "PYTHON_LIBRARY=${PYTHON_LIBRARY}")
#message( STATUS "PYTHON_INCLUDE_DIR=${PYTHON_INCLUDE_DIR}")

#find_package(Boost)
#message( STATUS "Boost_INCLUDE_DIRS=${Boost_INCLUDE_DIRS}")

message( STATUS "Generating global includes")

find_package(PythonLibs REQUIRED)
find_package(PythonInterp REQUIRED)
find_package(Boost COMPONENTS python27 REQUIRED)
include_directories (${PYTHON_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} )

message( STATUS "Generating bats_pitch_parser library")
add_library(bats_pitch_parser SHARED ${SOURCE_FILES})
target_link_libraries(bats_pitch_parser ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})

message( STATUS "Generating bats_api library")
add_library(bats_api SHARED ${PYAPI_SOURCES})

# fix for osx. python only loads .so
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(bats_api PROPERTIES PREFIX "")
    set_target_properties(bats_api PROPERTIES OUTPUT_NAME "bats_api")
    set_target_properties(bats_api PROPERTIES SUFFIX ".so")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set_target_properties(bats_api PROPERTIES PREFIX "")
endif()

target_link_libraries(bats_api bats_pitch_parser)


enable_testing()

message( STATUS "Generating tests")
find_package(Boost COMPONENTS python27 iostreams unit_test_framework REQUIRED)
include_directories (${PYTHON_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} )

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

file(GLOB PY_TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} tests/*.py)
foreach(pytestSrc ${PY_TEST_SRCS})
    message(STATUS "pytestSrc=${pytestSrc}")
    get_filename_component(pytestName ${pytestSrc} NAME_WE)
    message(STATUS "pytestName=${pytestName}")
    message(STATUS "command=${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${pytestSrc}")
    add_test(NAME ${pytestName} 
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests
         COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${pytestSrc} )
endforeach(pytestSrc)

ADD_CUSTOM_COMMAND(
    TARGET bats_api
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_BINARY_DIR}/bats_api.so 
    ${CMAKE_CURRENT_BINARY_DIR}/tests/bats_api.so )

set(TEST_FILES tests/pitch_data_100k tests/pitch_example_data ${PY_TEST_SRCS})
foreach(testFile ${TEST_FILES})
        configure_file(${testFile} ${testFile} COPYONLY)
endforeach(testFile)
