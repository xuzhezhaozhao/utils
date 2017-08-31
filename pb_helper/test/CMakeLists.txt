project(pb_helper_test)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

# gtest
include_directories(../../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../deps/googletest/build/googlemock/gtest/)

# jsoncpp
set(JSONCPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/jsoncpp/)
include_directories(${JSONCPP_DIR}/dist)

# protobuf output header
include_directories(${CMAKE_BINARY_DIR}/test)

# generate protobuf output file
add_custom_command(
	COMMAND protoc ARGS -I=${CMAKE_SOURCE_DIR}/test/proto/ --cpp_out=${CMAKE_BINARY_DIR}/test/ 
	${CMAKE_SOURCE_DIR}/test/proto/person.proto
	OUTPUT person.pb.cc person.pb.h
	)

add_executable(pb_helper_test pb_helper_test.cpp person.pb.cc ${JSONCPP_DIR}/dist/jsoncpp.cpp)
# libstr_helper
link_directories(${CMAKE_SOURCE_DIR}/../str_helper/build/)
target_link_libraries(pb_helper_test pb_helper str_helper gtest gtest_main protobuf pthread)


# unittest
add_custom_target(unittest ./pb_helper_test
				DEPENDS pb_helper_test)
