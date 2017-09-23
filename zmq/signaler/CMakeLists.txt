project(signaler)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

include_directories(../../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../deps/googletest/build/googlemock/gtest/)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../zmq/ip/build/)
include_directories(../../)
include_directories(../../zmq/)

add_library(signaler signaler.cpp)

add_subdirectory(test)
