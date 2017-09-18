/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月18日 星期一 22时26分13秒
 @File Name: unixstrserv01.c
 @Description:
 ******************************************************/

#include "../unp.h"
#include "znet/znet.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/signal.h>
#include <errno.h>
#include <sys/wait.h>

#define UNIXSTR_PATH "/tmp/unix.str" /* Unix domain stream */
#define MAXLINE 1024

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
	char buf[MAXLINE];
	ssize_t n = 0;

again:
	while ((n = read(sockfd, buf, MAXLINE)) > 0) {
		if (zwriten(sockfd, buf, n) != n) {
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
	int connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_un cliaddr, servaddr;

	int listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (listenfd < 0) {
		perror("socket");
		exit(-1);
	}

	unlink(UNIXSTR_PATH);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path, UNIXSTR_PATH);

	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind");
		exit(-1);
	}

	if (listen(listenfd, 1024) < 0) {
		perror("listen");
		exit(-1);
	}

	unp_signal(SIGCHLD, sig_chld);

	for (;;) {
		clilen = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) <
			0) {
			if (errno == EINTR) {
				continue; /* back to for() */
			} else {
				perror("accept");
				exit(-1);
			}
		}

		if ((childpid = fork()) == 0) { /* child process */
			close(listenfd);			/* close listening socket */
			str_echo(connfd);			/* process request */
			exit(0);
		} else if (childpid < 0) {
			perror("fork");
			exit(-1);
		}
		close(connfd); /* parent closes connected socket */
	}
}
