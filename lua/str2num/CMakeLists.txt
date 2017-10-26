project(lua_str2num)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()


include_directories(../../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../deps/googletest/build/googlemock/gtest/)

add_library(str2num str2num.c)
add_subdirectory(test)
