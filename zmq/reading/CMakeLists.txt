project(zmq)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

set(CMAKE_C_FLAGS "-g -Wall -std=c99")

# gtest
include_directories(../../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../deps/googletest/build/googlemock/gtest/)

# libzmq
include_directories(../../deps/libzmq/include/)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../deps/libzmq/build/lib)

#cppzmq
include_directories(SYSTEM ../../deps/cppzmq/)

add_executable(test_inproc test_inproc.c)
target_link_libraries(test_inproc zmq)
