project(uclock)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

add_executable(str2num_test str2num_test.cpp)
target_link_libraries(str2num_test str2num gtest pthread)

add_custom_target(unittest ./str2num_test DEPENDS str2num_test)
