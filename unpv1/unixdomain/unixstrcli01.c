/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月18日 星期一 22时41分41秒
 @File Name: unistrcli01.c
 @Description:
 ******************************************************/

#include "znet/znet.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/select.h>

#define MAXLINE 1024
#define UNIXSTR_PATH "/tmp/unix.str" /* Unix domain stream */
#define	min(a,b)	((a) < (b) ? (a) : (b))
#define	max(a,b)	((a) > (b) ? (a) : (b))

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

int main() {
	struct sockaddr_un servaddr;
	int sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path, UNIXSTR_PATH);
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect");
		exit(-1);
	}

	str_cli(stdin, sockfd);

	return 0;
}
