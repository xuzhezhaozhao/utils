/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月16日 星期六 13时59分34秒
 @File Name: dg_cli.h
 @Description:
 ******************************************************/

#include <stdio.h>
#include <sys/socket.h>

void dg_cli(FILE* fp, int sockfd, const struct sockaddr* pservaddr,
			socklen_t servlen);

void dg_cliconnect(FILE* fp, int sockfd, const struct sockaddr* pservaddr,
				   socklen_t servlen);
