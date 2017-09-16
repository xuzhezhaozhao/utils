project(udpcliserv)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

set(CMAKE_CXX_FLAGS "-g -Wall -Werror")

include_directories(../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../deps/googletest/build/googlemock/gtest/)

add_executable(udpserv01 udpserv01.c dg_echo.c)
add_executable(udpcli01 udpcli01.c dg_cli.c)
target_link_libraries(udpcli01 znet)

add_executable(udpservselect01 udpservselect01.c)
target_link_libraries(udpservselect01 znet unp)
