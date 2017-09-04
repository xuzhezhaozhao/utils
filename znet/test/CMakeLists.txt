project(znet_test)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()
# gtest
include_directories(../../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../deps/googletest/build/googlemock/gtest/)

add_executable(znet_test znet_test.cpp)
target_link_libraries(znet_test znet gtest gtest_main pthread)

# unittest
add_custom_target(unittest ./znet_test DEPENDS znet_test)
