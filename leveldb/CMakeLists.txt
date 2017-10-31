project(leveldb)

cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++0x")

include_directories(${CMAKE_SOURCE_DIR}/../deps/leveldb/include)
link_directories(${CMAKE_SOURCE_DIR}/../deps/leveldb/out-static/)

add_executable(playtoy playtoy.cpp)
target_link_libraries(playtoy leveldb pthread)
