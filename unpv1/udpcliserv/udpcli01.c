/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月15日 星期五 23时17分40秒
 @File Name: udpcli01.c
 @Description:
 ******************************************************/

#include "def.h"
#include "dg_cli.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char **argv) {
	struct sockaddr_in servaddr;

	if (argc != 2) {
		fprintf(stderr, "usage: udpcli <IPaddress>");
		exit(-1);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
		fprintf(stderr, "inet_ntop error for %s\n", argv[1]);
		exit(-1);
	}

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(-1);
	}

	dg_cliconnect(stdin, sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

	return 0;
}
