/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月04日 星期一 23时14分54秒
 @File Name: tcpcli01.c
 @Description:
 ******************************************************/

#include "../znet.h"
#include "def.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>

extern ssize_t readline(int fd, void *vptr, size_t maxlen);  // readline.c

void str_cli(FILE *fp, int sockfd) {
	char sendline[MAXLINE], recvline[MAXLINE];
	fd_set rset;

	FD_ZERO(&rset);
	for (;;) {
		FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		int maxfdp1 = max(fileno(fp), sockfd) + 1;
		if (select(maxfdp1, &rset, NULL, NULL, NULL) < 0) {
			perror("select");
			exit(-1);
		}
		if (FD_ISSET(sockfd, &rset)) {
			ssize_t cnt = readline(sockfd, recvline, MAXLINE);
			if (cnt == 0) {
				fprintf(stderr, "str_cli: server terminated prematurely\n");
				exit(-1);
			} else if (cnt < 0) {
				perror("readline");
				exit(-1);
			}

			fputs(recvline, stdout);
		}
		if (FD_ISSET(fileno(fp), &rset)) {
			if (fgets(sendline, MAXLINE, fp) == NULL) {
				return;
			}
			if (zwriten(sockfd, sendline, strlen(sendline)) < 0) {
				perror("write");
				exit(-1);
			}
		}
	}
}

int main(int argc, char **argv) {
	struct sockaddr_in servaddr;

	if (argc != 2) {
		fprintf(stderr, "usage: tcpcli <IPaddress>\n");
		exit(-1);
	}
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(-1);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(LISTEN_PORT);
	int ret = inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	if (ret == 0) {
		fprintf(stderr, "inet_pton: not a valid network address.\n");
		exit(-1);
	} else if (ret == -1) {
		perror("inet_pton");
		exit(-1);
	}

	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect");
		exit(-1);
	}

	str_cli(stdin, sockfd);

	return 0;
}
