project(names)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

set(CMAKE_CXX_FLAGS "-g -Wall -Werror")

include_directories(../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../deps/googletest/build/googlemock/gtest/)

add_executable(hostent hostent.c)
add_executable(daytimetcpcli daytimetcpcli.c)
target_link_libraries(daytimetcpcli unp znet)

add_executable(daytimetcpsrv1 daytimetcpsrv1.c)
target_link_libraries(daytimetcpsrv1 unp znet)

add_executable(daytimetcpsrv2 daytimetcpsrv2.c)
target_link_libraries(daytimetcpsrv2 unp znet)
