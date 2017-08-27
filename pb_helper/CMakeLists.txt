project(pb_helper)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")

include_directories(${CMAKE_SOURCE_DIR}/../)
link_directories(${CMAKE_SOURCE_DIR}/../str_helper/build/)

add_library(pb_helper pb_helper.cpp)
add_subdirectory(test)
