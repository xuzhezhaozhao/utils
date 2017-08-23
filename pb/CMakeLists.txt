project(pb_help)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

set(CMAKE_CXX_FLAGS "-g -Wall -Werror -std=c++11")

include_directories(../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../deps/googletest/build/googlemock/gtest/)

#add_library(pb_helper pb_helper.cpp)
add_executable(pb_helper_test ${CMAKE_CURRENT_SOURCE_DIR}/test/pb_helper_test.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/test/proto/person.pb.cc 
	${CMAKE_CURRENT_SOURCE_DIR}/pb_helper.cpp)
target_link_libraries(pb_helper_test gtest gtest_main protobuf pthread)
