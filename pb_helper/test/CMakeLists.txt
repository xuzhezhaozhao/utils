project(pb_helper_test)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

# gtest
include_directories(SYSTEM ../../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../deps/googletest/build/googlemock/gtest/)

# jsoncpp
set(JSONCPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/jsoncpp/)
include_directories(SYSTEM ${JSONCPP_DIR}/dist)

# protobuf output header
include_directories(${CMAKE_BINARY_DIR}/test)

# compile protobuf
set(PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto)
set(PROTO_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/)
add_custom_command(
	OUTPUT ${PROTO_OUTPUT_DIR}/person.pb.cc ${PROTO_OUTPUT_DIR}/person.pb.h
	DEPENDS ${PROTO_DIR}/person.proto
	COMMAND protoc ARGS -I=${PROTO_DIR} --cpp_out=${PROTO_OUTPUT_DIR} ${PROTO_DIR}/person.proto
	)

add_executable(pb_helper_test pb_helper_test.cpp 
	${PROTO_OUTPUT_DIR}/person.pb.cc
	${JSONCPP_DIR}/dist/jsoncpp.cpp)

# libstr_helper
link_directories(${CMAKE_SOURCE_DIR}/../str_helper/build/)
target_link_libraries(pb_helper_test pb_helper str_helper gtest protobuf pthread)


# unittest
add_custom_target(unittest ./pb_helper_test
				DEPENDS pb_helper_test)
