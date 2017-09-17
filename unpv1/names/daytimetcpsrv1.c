/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月17日 星期日 00时14分44秒
 @File Name: daytimetcpsrv1.c
 @Description:
 ******************************************************/

#include "../unp.h"
#include "znet/znet.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAXLINE 1024

int main(int argc, char **argv) {
	char buff[MAXLINE];
	struct sockaddr_storage cliaddr;

	if (argc != 2) {
		fprintf(stderr, "usage: daytimetcpsrv1 <service or port#>");
	}

	int listenfd = tcp_listen(NULL, argv[1], NULL);
	if (listenfd < 0) {
		fprintf(stderr, "tcp_listen error.\n");
		exit(-1);
	}

	for (;;) {
		socklen_t len = sizeof(cliaddr);
		int connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);
		if (connfd < 0) {
			perror("accept");
			exit(-1);
		}
		const char *addr = zsock_ntop((struct sockaddr *)&cliaddr, len);
		if (addr != NULL) {
			printf("connection from %s\n", addr);
		}

		time_t ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		size_t blen = strlen(buff);
		if (write(connfd, buff, blen) != (ssize_t)blen) {
			perror("write");
			exit(-1);
		}

		if (close(connfd) < 0) {
			perror("close");
			exit(-1);
		}
	}
}
