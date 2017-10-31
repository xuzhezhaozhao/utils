project(buffer_test)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

# gtest
include_directories(SYSTEM ../../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../deps/googletest/build/googlemock/gtest/)

add_executable(buffer_test buffer_test.cpp)
target_link_libraries(buffer_test buffer gtest pthread)

# unittest
add_custom_target(unittest ./buffer_test
				DEPENDS buffer_test)
