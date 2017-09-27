project(ypipe)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

add_executable(ypipe_test ypipe_test.cpp)
target_link_libraries(ypipe_test gtest pthread)

add_custom_target(unittest ./ypipe_test DEPENDS ypipe_test)
