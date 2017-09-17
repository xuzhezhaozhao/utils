/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月17日 星期日 00时05分33秒
 @File Name: tcp_listen.c
 @Description:
 ******************************************************/

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define LISTENQ 1024


/**
 * host: 主机名
 * serv: 服务名
 * 成功返回监听套接字描述符，出错返回 -1
 * TODO 如何返回出错描述信息, 添加 errmsg 参数？
 */
int tcp_listen(const char *host, const char *serv, socklen_t *addrlenp) {
	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int n = 0;
	if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
		/*err_quit("tcp_listen error for %s, %s: %s", host, serv,*/
		/*gai_strerror(n));*/
		return -1;
	}
	ressave = res;

	int listenfd = 0;
	do {
		listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (listenfd < 0) {
			continue; /* error, try next one */
		}

		const int on = 1;
		if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) <
			0) {
			return -1;
		}
		if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0) {
			break; /* success */
		}

		close(listenfd); /* bind error, close and try next one */
	} while ((res = res->ai_next) != NULL);

	if (res == NULL) {
		/* errno from final socket() or bind() */
		return -1;
	}

	if (listen(listenfd, LISTENQ) < 0) {
		return -1;
	}

	if (addrlenp) {
		*addrlenp = res->ai_addrlen; /* return size of protocol address */
	}

	freeaddrinfo(ressave);

	return listenfd;
}
