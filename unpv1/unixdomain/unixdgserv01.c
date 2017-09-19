/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月18日 星期一 22时55分56秒
 @File Name: unixdgserv01.c
 @Description:
 ******************************************************/

#include "../unp.h"
#include "znet/znet.h"

#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXLINE 1024
#define	UNIXDG_PATH		"/tmp/unix.dg"	/* Unix domain datagram */

void dg_echo(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen) {
	char mesg[MAXLINE];
	for (;;) {
		socklen_t len = clilen;
		ssize_t n = recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
		if (n < 0) {
			perror("recvfrom");
			exit(-1);
		}

		if (sendto(sockfd, mesg, n, 0, pcliaddr, len) < 0) {
			perror("sendto");
			exit(-1);
		}
	}
}

int main() {
	struct sockaddr_un servaddr, cliaddr;

	int sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(-1);
	}

	unlink(UNIXDG_PATH);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path, UNIXDG_PATH);

	if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind");
		exit(-1);
	}

	dg_echo(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
}
