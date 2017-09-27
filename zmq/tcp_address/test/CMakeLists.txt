project(tcp_address)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

add_executable(tcp_address_test tcp_address_test.cpp)
target_link_libraries(tcp_address_test tcp_address gtest pthread)

add_custom_target(unittest ./tcp_address_test DEPENDS tcp_address_test)
