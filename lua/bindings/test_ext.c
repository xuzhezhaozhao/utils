/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Sat 28 Oct 2017 06:46:59 PM CST
 @File Name: test_ext.c
 @Description:
 ******************************************************/

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "ext.h"

static int l_ext(lua_State *L) {
	lua_pushnumber(L, test_ext());

	return 1;
}

static const struct luaL_Reg libtest_ext[] = {{"ext", l_ext}, {NULL, NULL}};

extern int luaopen_libtest_ext(lua_State *L) {
	luaL_newlib(L, libtest_ext);

	return 1;
}
