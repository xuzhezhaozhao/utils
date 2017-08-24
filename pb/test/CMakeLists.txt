project(pb_helper)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

include_directories(../../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../deps/googletest/build/googlemock/gtest/)
include_directories(${CMAKE_BINARY_DIR}/test)


add_custom_command(
	COMMAND protoc ARGS -I=${CMAKE_SOURCE_DIR}/test/proto/ --cpp_out=${CMAKE_BINARY_DIR}/test/ 
	${CMAKE_SOURCE_DIR}/test/proto/person.proto
	OUTPUT person.pb.cc person.pb.h
	)

add_executable(pb_helper_test pb_helper_test.cpp person.pb.cc)
target_link_libraries(pb_helper_test pb_helper str_helper gtest gtest_main protobuf pthread)
link_directories(${CMAKE_SOURCE_DIR}/../strings/build/)

add_custom_target(unittest ./pb_helper_test
				DEPENDS pb_helper_test)
