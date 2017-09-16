/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月16日 星期六 14时38分03秒
 @File Name: udpservselect01.c
 @Description: udp, tcp 混合服务器
 ******************************************************/

#include "../unp.h"
#include "def.h"
#include "znet/znet.h"

#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


// 处理子进程结束信号 (SIGCHLD)
void sig_chld(int signo) {
	(void)(signo);
	pid_t pid;
	int stat;
	// 不能用 wait, 因为 Unix 信号不排队
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
		printf("child %d terminated.\n", pid);
	}
}

static void str_echo(int sockfd) {
	char buf[MAXLINE] = "[srv] ";
	ssize_t n = 0;

again:
	while ((n = read(sockfd, buf + 6, MAXLINE)) > 0) {
		if (zwriten(sockfd, buf, n + 6) != n + 6) {
			perror("write");
			exit(-1);
		}
	}
	if (n < 0 && errno == EINTR) {
		goto again;
	} else if (n < 0) {
		perror("[str_echo] read");
		exit(-1);
	}
}

int main() {
	int listenfd, connfd, udpfd, nready, maxfdp1;
	char mesg[MAXLINE];
	pid_t childpid;
	fd_set rset;
	ssize_t n;
	socklen_t len;
	const int on = 1;
	struct sockaddr_in cliaddr, servaddr;

	/* 4create listening TCP socket */
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0) {
		perror("socket");
		exit(-1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		perror("setsockopt");
		exit(-1);
	}
	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind");
		exit(-1);
	}

	if (listen(listenfd, LISTENQ) < 0) {
		perror("listen");
		exit(-1);
	}

	/* 4create UDP socket */
	udpfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (udpfd < 0) {
		perror("socket");
		exit(-1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	if (bind(udpfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind");
		exit(-1);
	}

	unp_signal(SIGCHLD, sig_chld); /* must call waitpid() */

	FD_ZERO(&rset);
	maxfdp1 = max(listenfd, udpfd) + 1;
	for (;;) {
		FD_SET(listenfd, &rset);
		FD_SET(udpfd, &rset);
		if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) {
			if (errno == EINTR) {
				continue;
			} else {
				perror("select");
				exit(-1);
			}
		}

		if (FD_ISSET(listenfd, &rset)) {
			len = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);
			if (connfd < 0) {
				perror("accept");
				exit(-1);
			}

			if ((childpid = fork()) == 0) { /* child process */
				if (close(listenfd) < 0) {
					perror("close");
					exit(-1);
				}
				str_echo(connfd); /* process the request */
				exit(0);
			} else if (childpid < 0) {
				perror("fork");
				exit(-1);
			}
			if (close(connfd) < 0) {
				perror("close");
				exit(-1);
			}
		}

		if (FD_ISSET(udpfd, &rset)) {
			len = sizeof(cliaddr);
			n = recvfrom(udpfd, mesg, MAXLINE, 0, (struct sockaddr *)&cliaddr,
						 &len);
			if (n < 0) {
				perror("recvfrom");
				exit(-1);
			}

			if (sendto(udpfd, mesg, n, 0, (struct sockaddr *)&cliaddr, len) !=
				n) {
				perror("sendto");
				exit(-1);
			}
		}
	}
}
