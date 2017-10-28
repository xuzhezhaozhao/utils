/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Fri 27 Oct 2017 11:24:46 PM CST
 @File Name: hello.c
 @Description:
 ******************************************************/

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static int hello(lua_State *L) {
	lua_pushliteral(L, "Hello World!");

	return 1;
}

static const luaL_Reg hellolib[] = {
	{"hello", hello},
};

LUAMOD_API int luaopen_hello(lua_State *L) {
	luaL_newlib(L, hellolib);

	return 1;
}
