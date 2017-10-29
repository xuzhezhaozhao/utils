project(lua_c_call_lua)
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in source build is not allowed.")
endif()


include_directories(../../deps/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../deps/googletest/build/googlemock/gtest/)

include_directories(../../deps/lua-5.3.4/src)
link_directories(${CMAKE_SOURCE_DIR}/../../deps/lua-5.3.4/src)

set(config_lua_link ${CMAKE_BINARY_DIR}/config.lua)
add_custom_command(
	OUTPUT ${config_lua_link}
	COMMAND ln -s ${CMAKE_SOURCE_DIR}/config.lua ${config_lua_link}
	COMMENT "create soft link config.lua ..."
	VERBATIM 
	)

add_custom_target(create_config_lua_link
	DEPENDS ${config_lua_link}
	)

add_executable(config config.c)
target_link_libraries(config lua m)
add_dependencies(config create_config_lua_link)



set(config_table_lua_link ${CMAKE_BINARY_DIR}/config_table.lua)
add_custom_command(
	OUTPUT ${config_table_lua_link}
	COMMAND ln -s ${CMAKE_SOURCE_DIR}/config_table.lua ${config_table_lua_link}
	COMMENT "create soft link config_table.lua ..."
	VERBATIM 
	)

add_custom_target(create_config_table_lua_link
	DEPENDS ${config_table_lua_link}
	)

add_executable(config_table config_table.c)
target_link_libraries(config_table lua m)
add_dependencies(config_table create_config_table_lua_link)
