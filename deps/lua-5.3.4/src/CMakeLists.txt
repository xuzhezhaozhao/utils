project(lua_bindings)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()

aux_source_directory(. SRCS)
list(REMOVE_ITEM SRCS "./lua.c")
list(REMOVE_ITEM SRCS "./luac.c")

add_definitions(-DLUA_COMPAT_5_2 -DLUA_USE_LINUX)

# linux
set(CMAKE_C_FLAGS "-std=gnu99 -O2 -Wall -Wextra")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-E")

add_library(lua ${SRCS})

add_executable(slua lua.c)
target_link_libraries(slua lua m dl readline)

add_executable(luac luac.c)
target_link_libraries(luac lua m dl readline)
