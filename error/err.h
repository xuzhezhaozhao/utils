/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Tue 19 Sep 2017 07:45:44 PM CST
 @File Name: err.h
 @Description: from libzmq/src/err.hpp
 ******************************************************/

#ifndef UTILS_MUTEX_ERR_H_
#define UTILS_MUTEX_ERR_H_

#include "likely/likely.h"

#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO abort() 之前打印调用栈信息

//  This macro works in exactly the same way as the normal assert. It is used
//  in its stead because standard assert on Win32 in broken - it prints nothing
//  when used within the scope of JNI library.
#define utils_assert(x)                                                     \
	do {                                                                    \
		if (unlikely(!(x))) {                                               \
			fprintf(stderr, "Assertion failed: %s (%s:%d)\n", #x, __FILE__, \
					__LINE__);                                              \
			fflush(stderr);                                                 \
			abort();                                                        \
		}                                                                   \
	} while (false)

//  Provides convenient way to check for errno-style errors.
#define errno_assert(x)                                                  \
	do {                                                                 \
		if (unlikely(!(x))) {                                            \
			const char *errstr = strerror(errno);                        \
			fprintf(stderr, "%s (%s:%d)\n", errstr, __FILE__, __LINE__); \
			fflush(stderr);                                              \
			abort();                                                     \
		}                                                                \
	} while (false)

//  Provides convenient way to check for POSIX errors.
#define posix_assert(x)                                                  \
	do {                                                                 \
		if (unlikely(x)) {                                               \
			const char *errstr = strerror(x);                            \
			fprintf(stderr, "%s (%s:%d)\n", errstr, __FILE__, __LINE__); \
			fflush(stderr);                                              \
			abort();                                                     \
		}                                                                \
	} while (false)

//  Provides convenient way to check for errors from getaddrinfo.
#define gai_assert(x)                                                    \
	do {                                                                 \
		if (unlikely(x)) {                                               \
			const char *errstr = gai_strerror(x);                        \
			fprintf(stderr, "%s (%s:%d)\n", errstr, __FILE__, __LINE__); \
			fflush(stderr);                                              \
			abort();                                                     \
		}                                                                \
	} while (false)

//  Provides convenient way to check whether memory allocation have succeeded.
#define alloc_assert(x)                                                       \
	do {                                                                      \
		if (unlikely(!x)) {                                                   \
			fprintf(stderr, "FATAL ERROR: OUT OF MEMORY (%s:%d)\n", __FILE__, \
					__LINE__);                                                \
			fflush(stderr);                                                   \
			abort();                                                          \
		}                                                                     \
	} while (false)

#endif
