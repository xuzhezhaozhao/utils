/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月18日 星期一 22时59分30秒
 @File Name: unixdgcli01.c
 @Description:
 ******************************************************/

#include "../unp.h"
#include "znet/znet.h"

#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXLINE 1024
#define	UNIXDG_PATH		"/tmp/unix.dg"	/* Unix domain datagram */

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
				printf("reply from %s.\n", addr);
			} else {
				printf("reply from unknown.\n");
			}
		}

		recvline[n] = 0; /* null terminate */
		fputs(recvline, stdout);
	}
}

int main() {
	struct sockaddr_un cliaddr, servaddr;

	int sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(-1);
	}

	bzero(&cliaddr, sizeof(cliaddr)); /* bind an address for us */
	cliaddr.sun_family = AF_LOCAL;
	char tmpname[128] = "unix.socket.XXXXXX";
	int tmpfd = mkstemp(tmpname);
	if (tmpfd < 0) {
		perror("mkstemp");
		exit(-1);
	}
	strcpy(cliaddr.sun_path, tmpname); /* 用于显示 bind */
	close(tmpfd);
	unlink(tmpname);

	/* unix 套接字必须显示调用bind，否则对端无法返回数据 */
	if (bind(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0) {
		perror("bind");
		exit(-1);
	}

	bzero(&servaddr, sizeof(servaddr)); /* fill in server's address */
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path, UNIXDG_PATH);

	dg_cli(stdin, sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	unlink(tmpname);

	return 0;
}
