project(clock)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

add_executable(clock_test clock_test.cpp)
target_link_libraries(clock_test clock gtest pthread)

add_custom_target(unittest ./clock_test DEPENDS clock_test)
