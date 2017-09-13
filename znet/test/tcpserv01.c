/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月04日 星期一 22时39分19秒
 @File Name: tcpserv01.c
 @Description:
 ******************************************************/

#include "../znet.h"
#include "def.h"
#include "unpv1/unp.h"

#include <bits/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

// 处理子进程结束信号 (SIGCHLD)
void sig_chld(int signo) {
	UNUSED(signo);
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
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0) {
		perror("socket");
		exit(-1);
	}

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(LISTEN_PORT);

	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind");
		exit(-1);
	}

	if (listen(listenfd, MAX_BACKLOG) < 0) {
		perror("listen");
		exit(-1);
	}

	if (unp_signal(SIGCHLD, sig_chld) == SIG_ERR) {
		perror("unp_signal");
		exit(-1);
	}

	for (;;) {
		struct sockaddr_in cliaddr;
		socklen_t clilen = sizeof(cliaddr);
		int connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
		if (connfd < 0) {
			perror("accept");
			exit(-1);
		}

		pid_t childpid;
		if ((childpid = fork()) == 0) { /* child process */
			close(listenfd);			/* close listening socket */
			str_echo(connfd);			/* process the request */
			exit(0);
		} else if (childpid < 0) {
			perror("fork");
			exit(-1);
		}
		close(connfd); /* parent closes connected socket */
	}

	return 0;
}
