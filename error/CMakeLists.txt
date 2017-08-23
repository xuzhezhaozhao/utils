project(herror)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

set(CMAKE_CXX_FLAGS "-g -Wall -Werror -std=c++11")

include_directories(../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../deps/googletest/build/googlemock/gtest/)

add_library(herror herror.cpp)
add_executable(herror_test herror.cpp herror_test.cpp)
target_link_libraries(herror_test gtest gtest_main pthread)
