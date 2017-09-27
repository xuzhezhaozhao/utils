project(examples)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

set(CMAKE_C_FLAGS "-g -Wall -std=gnu99 -Wno-unused-function")

# libzmq
include_directories(../../deps/libzmq/include/)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../deps/libzmq/build/lib)

#cppzmq
include_directories(SYSTEM ../../deps/cppzmq/)

add_subdirectory(hwcliserv)
add_subdirectory(identity)
add_subdirectory(router_req)
add_subdirectory(router_dealer)
add_subdirectory(load_balance_broker)
