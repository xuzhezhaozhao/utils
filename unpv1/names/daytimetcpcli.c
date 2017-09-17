/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月16日 星期六 20时40分49秒
 @File Name: daytimetcpcli.c
 @Description:
 ******************************************************/

#include "znet/znet.h"
#include "../unp.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLINE 1024


int main(int argc, char **argv) {
	int sockfd;
	char recvline[MAXLINE + 1];
	socklen_t len;
	struct sockaddr_storage ss;

	if (argc != 3) {
		fprintf(stderr,
				"usage: daytimetcpcli <hostname/IPaddress> <service/port#>\n");
		exit(-1);
	}

	sockfd = tcp_connect(argv[1], argv[2]);
	if (sockfd < 0) {
		fprintf(stderr, "error: cannot connect any host.\n");
		exit(-1);
	}

	len = sizeof(ss);
	if (getpeername(sockfd, (struct sockaddr *)&ss, &len) < 0) {
		perror("getpeername");
		exit(-1);
	}
	const char *host = zsock_ntop((struct sockaddr *)&ss, len);
	if (!host) {
		perror("zsock_ntop");
		exit(-1);
	}
	printf("connected to %s\n", host);

	ssize_t n = 0;
	while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0; /* null terminate */
		fputs(recvline, stdout);
	}
	if (n < 0) {
		perror("read");
		exit(-1);
	}
	exit(0);
}
