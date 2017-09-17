/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月16日 星期六 23时33分49秒
 @File Name: host_serv.c
 @Description: unpv1 p254
 ******************************************************/

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

/* family 和 socktype 作为 hints 传递给 getaddrinfo 函数, 出错返回 NULL */
struct addrinfo *host_serv(const char *host, const char *serv, int family,
						   int socktype) {
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME; /* always return canonical name */
	hints.ai_family = family;	  /* AF_UNSPEC, AF_INET, AF_INET6, etc. */
	hints.ai_socktype = socktype;  /* 0, SOCK_STREAM, SOCK_DGRAM, etc. */

	if (getaddrinfo(host, serv, &hints, &res) != 0) {
		return NULL;
	}

	return res; /* return pointer to first on linked list */
}

/*
 * There is no easy way to pass back the integer return code from
 * getaddrinfo() in the function above, short of adding another argument
 * that is a pointer, so the easiest way to provide the wrapper function
 * is just to duplicate the simple function as we do here.
 */
struct addrinfo *Host_serv(const char *host, const char *serv, int family,
						   int socktype) {
	int n;
	struct addrinfo hints, *res;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME; /* always return canonical name */
	hints.ai_family = family;	  /* 0, AF_INET, AF_INET6, etc. */
	hints.ai_socktype = socktype;  /* 0, SOCK_STREAM, SOCK_DGRAM, etc. */

	if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
		fprintf(stderr, "host_serv error for %s, %s: %s",
				(host == NULL) ? "(no hostname)" : host,
				(serv == NULL) ? "(no service name)" : serv, gai_strerror(n));
		exit(-1);
	}

	return res; /* return pointer to first on linked list */
}
