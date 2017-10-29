/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Sun 29 Oct 2017 09:49:31 PM CST
 @File Name: config.c
 @Description:
 ******************************************************/

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

int load(lua_State *L, const char *filename, int *width, int *height) {
	/* see also luaL_loadbuffer() and luaL_loadstring() */
	if (luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0)) {
		fprintf(stderr, "'load file %s failed. err msg: %s\n", filename,
				lua_tostring(L, -1));
		return -1;
	}

	lua_getglobal(L, "width");
	lua_getglobal(L, "height");
	if (!lua_isinteger(L, -2)) {
		fprintf(stderr, "'width' should be an integer.\n");
		return -1;
	}
	if (!lua_isnumber(L, -1)) {
		fprintf(stderr, "'height' should be an integer.\n");
		return -1;
	}

	*width = (int)lua_tointeger(L, -2);
	*height = (int)lua_tointeger(L, -1);

	return 0;
}

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	lua_State *L = luaL_newstate();
	int width = 0, height = 0;
	static const char *filename = "config.lua";
	printf("loading lua config file [%s] ...\n", filename);
	int rc = load(L, filename, &width, &height);
	if (rc == 0) {
		printf("width = %d, height = %d\n", width, height);
	}
	lua_close(L);
	return 0;
}
