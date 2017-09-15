/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月15日 星期五 22时16分08秒
 @File Name: udpserv01.c
 @Description:
 ******************************************************/

#include "def.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

extern void dg_echo(int sockfd, struct sockaddr* pcliaddr, socklen_t clilen);

int main() {
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(-1);
	}

	int onoff = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &onoff, sizeof(int)) < 0) {
		perror("setsockopt");
		exit(-1);
	}

	struct sockaddr_in servaddr, cliaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind");
		exit(-1);
	}

	dg_echo(sockfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr));

	return 0;
}
