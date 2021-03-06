/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月04日 星期一 23时14分54秒
 @File Name: tcpcli01.c
 @Description:
 ******************************************************/

#include "znet/znet.h"
#include "def.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

extern ssize_t readline(int fd, void *vptr, size_t maxlen);  // readline.c

// 利用 select 可以及时检测到服务端异常关闭的情况
// 正确处理重定向情况的批量输入
void str_cli(FILE *fp, int sockfd) {
	char buf[MAXLINE];
	fd_set rset;

	int stdineof = 0;
	FD_ZERO(&rset);
	for (;;) {
		if (stdineof == 0) {
			FD_SET(fileno(fp), &rset);
		}
		FD_SET(sockfd, &rset);
		int maxfdp1 = max(fileno(fp), sockfd) + 1;
		if (select(maxfdp1, &rset, NULL, NULL, NULL) < 0) {
			perror("select");
			exit(-1);
		}
		if (FD_ISSET(sockfd, &rset)) {
			ssize_t cnt = read(sockfd, buf, MAXLINE);
			if (cnt == 0) {
				if (stdineof == 1) {
					return;  // normal termination
				} else {
					fprintf(stderr, "str_cli: server terminated prematurely\n");
					exit(-1);
				}
			} else if (cnt < 0) {
				perror("[str cli] read");
				exit(-1);
			}

			if (write(fileno(stdout), buf, cnt) < 0) {
				perror("[str cli] write");
				exit(-1);
			}
		}

		if (FD_ISSET(fileno(fp), &rset)) {
			ssize_t cnt = read(fileno(fp), buf, MAXLINE);
			if (cnt == 0) {
				stdineof = 1;
				if (shutdown(sockfd, SHUT_WR) < 0) {
					perror("shutdown");
					exit(-1);
				}
				FD_CLR(fileno(fp), &rset);
				continue;
			} else if (cnt < 0) {
				perror("read stdin");
				exit(-1);
			}

			if (zwriten(sockfd, buf, cnt) < 0) {
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
