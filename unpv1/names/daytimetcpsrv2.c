/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月17日 星期日 00时29分13秒
 @File Name: daytimetcpsrv2.c
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
	socklen_t  addrlen;
	char buff[MAXLINE];
	struct sockaddr_storage cliaddr;

	int listenfd = 0;
	if (argc == 2) {
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
		if (listenfd < 0) {
			fprintf(stderr, "tcp_listen error.\n");
			exit(-1);
		}
	} else if (argc == 3) {
		int listenfd = tcp_listen(argv[1], argv[2], &addrlen);
		if (listenfd < 0) {
			fprintf(stderr, "tcp_listen error.\n");
			exit(-1);
		}
	} else {
		fprintf(stderr, "usage: daytimetcpsrv2 [ <host> ] <service or port>");
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
