/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Sun 29 Oct 2017 10:27:35 PM CST
 @File Name: config_table.c
 @Description:
 ******************************************************/

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

int load(lua_State *L, const char *filename, int *width, int *height) {
	if (luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0)) {
		fprintf(stderr, "'load file %s failed. err msg: %s\n", filename,
				lua_tostring(L, -1));
		return -1;
	}

	lua_getglobal(L, "window");
	if (!lua_istable(L, -1)) {
		fprintf(stderr, "'window' should be a table.\n");
		return -1;
	}

	lua_getfield(L, -1, "width");
	lua_getfield(L, -2, "height");
	if (!lua_isinteger(L, -2)) {
		fprintf(stderr, "'window.width' should be an integer.\n");
		return -1;
	}
	if (!lua_isinteger(L, -1)) {
		fprintf(stderr, "'window.height' should be an integer.\n");
		return -1;
	}
	*width = (int)lua_tointeger(L, -2);
	*height = (int)lua_tointeger(L, -1);

	lua_pop(L, 2);

	return 0;
}

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	lua_State *L = luaL_newstate();
	int width = 0, height = 0;
	static const char *filename = "config_table.lua";
	printf("loading lua config file [%s] ...\n", filename);
	int rc = load(L, filename, &width, &height);
	if (rc == 0) {
		printf("window.width = %d, window.height = %d\n", width, height);
	}
	lua_close(L);
	return 0;
}
