/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Mon 30 Oct 2017 10:50:14 PM CST
 @File Name: lbuffer.c
 @Description: from lua-5.3.4/src/lauxlib.c
 ******************************************************/

#include "lbuffer.h"

#include <string.h>
#include <stdlib.h>


void luaL_buffinit(luaL_Buffer *B) {
	B->b = B->initb;
	B->n = 0;
	B->size = LUAL_BUFFERSIZE;
}

/*
** check whether buffer is using a userdata on the stack as a temporary
** buffer
*/
#define buffonstack(B) ((B)->b != (B)->initb)

/*
** returns a pointer to a free area with at least 'sz' bytes
*/
char *luaL_prepbuffsize(luaL_Buffer *B, size_t sz) {
	if (B->size - B->n < sz) { /* not enough space? */
		char *newbuff;
		size_t newsize = B->size * 2; /* double buffer size */
		if (newsize - B->n < sz)	  /* not big enough? */
			newsize = B->n + sz;
		if (newsize < B->n || newsize - B->n < sz) {
			/*luaL_error(L, "buffer too large");*/
			return NULL;
		}
		/* create larger buffer */
		if (buffonstack(B)) {
			newbuff = (char *)resizebox(L, -1, newsize);
		} else { /* no buffer yet */
			newbuff = (char *)newbox(L, newsize);
			memcpy(newbuff, B->b,
				   B->n * sizeof(char)); /* copy original content */
		}
		B->b = newbuff;
		B->size = newsize;
	}
	return &B->b[B->n];
}

char *luaL_buffinitsize(luaL_Buffer *B, size_t sz) {
	luaL_buffinit(B);
	return luaL_prepbuffsize(B, sz);
}
