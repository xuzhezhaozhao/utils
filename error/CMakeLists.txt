project(herror)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

set(CMAKE_CXX_FLAGS "-g -Wall -Werror -std=c++11")

add_library(herror herror.cpp)
add_executable(herror_test herror.cpp herror_test.cpp)
