project(str_helper)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

include_directories(../../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../deps/googletest/build/googlemock/gtest/)

add_executable(str_helper_test str_helper_test.cpp)
target_link_libraries(str_helper_test str_helper gtest gtest_main pthread)

add_custom_target(unittest ./str_helper_test
				DEPENDS str_helper_test)
