project(threadpool)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

set(CMAKE_CXX_FLAGS "-g -Wall -Werror -std=c++11 -pthread")

add_executable(threadpool_test threadpool_test.cpp)
target_link_libraries(threadpool_test pthread)
