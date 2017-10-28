project(lua_bindings)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()


include_directories(../../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../deps/googletest/build/googlemock/gtest/)

include_directories(../../deps/lua-5.3.4/src)

add_library(hello SHARED hello.c)
add_library(mylib SHARED mylib.c)

add_library(ext ext.c)
add_library(test_ext SHARED test_ext.c)
