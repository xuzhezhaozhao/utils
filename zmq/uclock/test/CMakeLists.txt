project(uclock)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

add_executable(uclock_test uclock_test.cpp)
target_link_libraries(uclock_test uclock gtest pthread)

add_custom_target(unittest ./uclock_test DEPENDS uclock_test)
