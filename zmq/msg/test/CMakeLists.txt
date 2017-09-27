project(msg)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

add_executable(msg_test msg_test.cpp)
target_link_libraries(msg_test gtest gtest_main pthread)

add_custom_target(unittest ./msg_test DEPENDS msg_test)
