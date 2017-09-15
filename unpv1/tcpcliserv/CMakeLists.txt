project(znet_test)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()
# gtest
include_directories(../../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../deps/googletest/build/googlemock/gtest/)

# znet
include_directories(../../)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../znet/build)

# tcpserv01
add_executable(tcpserv01 tcpserv01.c)
target_link_libraries(tcpserv01 znet unp)

# tcpcli01
add_executable(tcpcli01 tcpcli01.c readline.c)
target_link_libraries(tcpcli01 znet)

# tcpservselect01
add_executable(tcpservselect01 tcpservselect01.c)
target_link_libraries(tcpservselect01 znet)

# tcpservpoll01
add_executable(tcpservpoll01 tcpservpoll01.c)
target_link_libraries(tcpservpoll01 znet)
