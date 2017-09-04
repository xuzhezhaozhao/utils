/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月04日 星期一 23时14分54秒
 @File Name: tcpcli01.c
 @Description:
 ******************************************************/

#include "../znet.h"
#include "def.h"

#include <stdlib.h>

extern ssize_t readline(int fd, void *vptr, size_t maxlen);  // readline.c

void str_cli(FILE *fp, int sockfd) {
	char sendline[MAXLINE], recvline[MAXLINE];

	while (fgets(sendline, MAXLINE, fp) != NULL) {
		if (zwriten(sockfd, sendline, strlen(sendline)) < 0) {
			perror("write");
			exit(-1);
		}

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
