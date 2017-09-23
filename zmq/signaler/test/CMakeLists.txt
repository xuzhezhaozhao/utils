project(signaler)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

add_executable(signaler_test signaler_test.cpp)
target_link_libraries(signaler_test signaler ip gtest gtest_main pthread)

add_custom_target(unittest ./signaler_test DEPENDS signaler_test)
