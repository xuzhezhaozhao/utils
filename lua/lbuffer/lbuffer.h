/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Mon 30 Oct 2017 10:55:10 PM CST
 @File Name: lbuffer.h
 @Description: from lua-5.3.4/src/lauxlib.h
 ******************************************************/

#ifndef UTILS_LUA_LBUFFER_H_
#define UTILS_LUA_LBUFFER_H_

#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 8192 */
#define LUAL_BUFFERSIZE ((int)(0x80 * sizeof(void *) * sizeof(long long)))

typedef struct luaL_Buffer {
	char *b;					 /* buffer address */
	size_t size;				 /* buffer size */
	size_t n;					 /* number of characters in buffer */
	char initb[LUAL_BUFFERSIZE]; /* initial buffer */
} luaL_Buffer;


#ifdef __cplusplus
}
#endif

#endif
