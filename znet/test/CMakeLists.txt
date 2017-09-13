project(znet_test)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()
# gtest
include_directories(../../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../deps/googletest/build/googlemock/gtest/)

# unp
include_directories(../../)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../unpv1/build)

add_executable(znet_test znet_test.cpp)
target_link_libraries(znet_test znet gtest gtest_main pthread)

# unittest
add_custom_target(unittest ./znet_test DEPENDS znet_test)

# tcpserv01
add_executable(tcpserv01 tcpserv01.c)
target_link_libraries(tcpserv01 znet unp)
# tcpcli01
add_executable(tcpcli01 tcpcli01.c readline.c)
target_link_libraries(tcpcli01 znet)

# tcpservselect01
add_executable(tcpservselect01 tcpservselect01.c)
target_link_libraries(tcpservselect01 znet)
