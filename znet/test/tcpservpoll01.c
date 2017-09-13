/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月13日 星期三 20时56分19秒
 @File Name: tcpservpoll01.c
 @Description:
 ******************************************************/

#include "../znet.h"
#include "def.h"
#include "unpv1/unp.h"

#include <bits/socket.h>
#include <limits.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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

	struct pollfd client[OPEN_MAX];
	client[0].fd = listenfd;
	client[0].events = POLLIN;
	for (int i = 1; i < OPEN_MAX; ++i) {
		client[i].fd = -1;
	}
	int maxi = 0;
	struct sockaddr_in cliaddr;
	char buf[MAXLINE];
	for (;;) {
		int nready = poll(client, maxi + 1, -1);
		if (nready < 0) {
			perror("poll");
			exit(-1);
		}
		if (client[0].revents & POLLIN) {
			/* new client connection */
			socklen_t clilen = sizeof(cliaddr);
			int connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
			if (connfd < 0) {
				perror("accept");
				exit(-1);
			}

			int i;
			for (i = 1; i < OPEN_MAX; ++i) {
				if (client[i].fd < 0) {
					client[i].fd = connfd; /* save descriptor */
					break;
				}
			}
			if (i == OPEN_MAX) {
				fprintf(stderr, "too many clients");
				exit(-1);
			}

			client[i].events = POLLIN;
			if (i > maxi) {
				maxi = i;
			}

			if (--nready <= 0) {
				continue;
			}
		}

		for (int i = 1; i <= maxi; i++) {
			int sockfd = client[i].fd;
			if (sockfd < 0) {
				continue;
			}
			if (client[i].revents & (POLLIN | POLLERR)) {
				ssize_t n = read(sockfd, buf, MAXLINE);
				if (n < 0) {
					if (errno == ECONNRESET) {
						/* connection reset by client */
						close(sockfd);
						client[i].fd = -1;
					} else {
						perror("read");
						exit(-1);
					}
				} else if (n == 0) {
					if (close(sockfd) < 0) {
						perror("close");
						exit(-1);
					}
					client[i].fd = -1;
				} else {
					if (zwriten(sockfd, buf, n) != n) {
						printf("write");
						exit(-1);
					}
				}
				if (--nready <= 0) {
					break;
				}
			}
		}
	}
	return 0;
}
