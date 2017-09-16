/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月16日 星期六 12时54分26秒
 @File Name: tcp_connect.c
 @Description: 指定主机名和服务名，通过 getaddrinfo 选择一个地址进行 tcp 连接
 ******************************************************/

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// host: 主机名
// serv: 服务名
int tcp_connect(const char *host, const char *serv) {
	struct addrinfo hints, *res, *ressave;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int n = 0;
	if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
		fprintf(stderr, "tcp_connect error for %s, %s: %s\n", host, serv,
				gai_strerror(n));
		exit(-1);
	}
	ressave = res;

	int sockfd = -1;
	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0) {
			continue; /* ignore this one */
		}
		if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0) {
			break; /* success */
		}

		close(sockfd); /* ignore this one */
	} while ((res = res->ai_next) != NULL);

	if (res == NULL) {
		/* errno set from final connect() */
		fprintf(stderr, "tcp_connect error for %s, %s: %s\n", host, serv,
				strerror(errno));
		sockfd = -1;
	}

	freeaddrinfo(ressave);

	return sockfd;
}
