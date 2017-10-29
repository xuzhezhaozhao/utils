/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Fri 27 Oct 2017 11:24:46 PM CST
 @File Name: hello.c
 @Description:
 ******************************************************/

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static int l_hello(lua_State *L) {
	lua_pushliteral(L, "Hello World!");

	return 1;
}

static const struct luaL_Reg libhello[] = {{"hello", l_hello}, {NULL, NULL}};

extern int luaopen_libhello(lua_State *L) {
	luaL_newlib(L, libhello);

	return 1;
}
