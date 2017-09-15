/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月15日 星期五 22时30分01秒
 @File Name: dg_echo.c
 @Description:
 ******************************************************/

#include "def.h"

#include <sys/types.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>


void dg_echo(int sockfd, struct sockaddr* pcliaddr, socklen_t clilen) {
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
