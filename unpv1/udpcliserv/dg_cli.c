/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月15日 星期五 23时25分02秒
 @File Name: dg_cli.c
 @Description:
 ******************************************************/

#include "def.h"
#include "znet/znet.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void dg_cli(FILE* fp, int sockfd, const struct sockaddr* pservaddr,
			socklen_t servlen) {
	char sendline[MAXLINE], recvline[MAXLINE + 1];
	struct sockaddr* preply_addr;
	preply_addr = (struct sockaddr*)malloc(servlen);
	if (!preply_addr) {
		perror("malloc");
		exit(-1);
	}

	while (fgets(sendline, MAXLINE, fp) != NULL) {
		if (sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen) <
			0) {
			perror("sendto");
			exit(-1);
		}

		socklen_t len = servlen;
		ssize_t n = recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
		if (n < 0) {
			perror("recvfrom");
			exit(-1);
		}
		if (len != servlen || memcmp(pservaddr, preply_addr, len) != 0) {
			const char* addr = zsock_ntop(preply_addr, len);
			if (addr) {
				printf("reply from %s ignored.\n", addr);
			} else {
				printf("reply from unknown ignored.\n");
			}
		}

		recvline[n] = 0; /* null terminate */
		fputs(recvline, stdout);
	}
}

void dg_cliconnect(FILE* fp, int sockfd, const struct sockaddr* pservaddr,
				   socklen_t servlen) {
	char sendline[MAXLINE], recvline[MAXLINE + 1];
	if (connect(sockfd, (struct sockaddr*)pservaddr, servlen) < 0) {
		perror("connect");
		exit(-1);
	}

	while (fgets(sendline, MAXLINE, fp) != NULL) {
		size_t len = strlen(sendline);
		if (write(sockfd, sendline, len) != (ssize_t)len) {
			perror("write");
			exit(-1);
		}

		ssize_t n = read(sockfd, recvline, MAXLINE);
		if (n < 0) {
			perror("read");
			exit(-1);
		}

		recvline[n] = 0; /* null terminate */
		fputs(recvline, stdout);
	}
}
