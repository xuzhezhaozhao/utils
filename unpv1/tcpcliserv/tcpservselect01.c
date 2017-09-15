/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月13日 星期三 10时14分57秒
 @File Name: tcpservselect01.c
 @Description:
 ******************************************************/

#include "znet/znet.h"
#include "def.h"
#include "unpv1/unp.h"

#include <bits/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/select.h>

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

	int client[FD_SETSIZE];
	fd_set rset, allset;
	char buf[MAXLINE];
	int maxfd = listenfd;
	int maxi = -1;
	for (int i = 0; i < FD_SETSIZE; ++i) {
		client[i] = -1;
	}
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	for (;;) {
		struct sockaddr_in cliaddr;
		socklen_t clilen = sizeof(cliaddr);
		rset = allset; /* structure assignment */
		int nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
		if (nready < 0) {
			perror("nready");
			exit(-1);
		}
		if (FD_ISSET(listenfd, &rset)) {
			int connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
			if (connfd < 0) {
				perror("accept");
				exit(-1);
			}
			int i;
			for (i = 0; i < FD_SETSIZE; ++i) {
				if (client[i] < 0) {
					client[i] = connfd;
					break;
				}
			}
			if (i == FD_SETSIZE) {
				fprintf(stderr, "too many clients");
				exit(-1);
			}
			FD_SET(connfd, &allset); /* add new descriptor to set */
			if (connfd > maxfd) {
				maxfd = connfd; /* for select */
			}
			if (i > maxi) {
				maxi = i; /* max index in client[] array */
			}
			if (--nready <= 0) {
				continue; /* no more readable descriptors */
			}
		}
		for (int i = 0; i <= maxi; ++i) {
			// check all clients for data
			int sockfd = client[i];
			if (sockfd < 0) {
				continue;
			}
			if (FD_ISSET(sockfd, &rset)) {
				ssize_t n = read(sockfd, buf, MAXLINE);
				if (n < 0) {
					perror("read");
					exit(-1);
				} else if (n == 0) {
					// connection closed by client
					if (close(sockfd) < 0) {
						perror("close");
						exit(-1);
					}
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				} else {
					if (zwriten(sockfd, buf, n) != n) {
						printf("write");
						exit(-1);
					}
				}
				if (--nready <= 0) {
					break;  // no more readable descriptors
				}
			}
		}
	}

	return 0;
}
