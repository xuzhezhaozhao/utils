project(unp_test)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

set(CMAKE_C_FLAGS "-g -Wall")

# gtest
include_directories(../../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../deps/googletest/build/googlemock/gtest/)

# signal_test 
add_executable(signal_test ../signal.c signal_test.c)
target_link_libraries(signal_test unp)

# checksockopt.c
add_executable(checksockopt checksockopt.c)
