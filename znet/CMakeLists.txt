project(znet)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

set(CMAKE_C_FLAGS "-g -Wall -std=c99")

add_library(znet znet.c)
add_subdirectory(test)
