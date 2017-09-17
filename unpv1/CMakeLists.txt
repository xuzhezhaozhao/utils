project(unp)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

set(CMAKE_CXX_FLAGS "-g -Wall -Werror")

include_directories(../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../deps/googletest/build/googlemock/gtest/)

include_directories(../)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../znet/build)

add_library(unp signal.c host_serv.c tcp_connect.c tcp_listen.c)

add_subdirectory(test)
add_subdirectory(udpcliserv)
add_subdirectory(tcpcliserv)
add_subdirectory(names)
