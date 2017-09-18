project(names)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

set(CMAKE_CXX_FLAGS "-g -Wall -Werror")

include_directories(../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../deps/googletest/build/googlemock/gtest/)

add_executable(unixbind unixbind.c)

add_executable(unixstrserv01 unixstrserv01.c)
target_link_libraries(unixstrserv01 znet unp)

add_executable(unixstrcli01 unixstrcli01.c)
target_link_libraries(unixstrcli01 znet)
