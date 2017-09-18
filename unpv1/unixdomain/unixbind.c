/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月18日 星期一 21时45分46秒
 @File Name: unixbind.c
 @Description:
 ******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: unixbind <pathname>\n");
		exit(-1);
	}

	int sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(-1);
	}

	struct sockaddr_un addr1, addr2;
	bzero(&addr1, sizeof(addr1));
	addr1.sun_family = AF_LOCAL;
	strncpy(addr1.sun_path, argv[1], sizeof(addr1.sun_path) - 1);
	if (bind(sockfd, (struct sockaddr *)&addr1, (socklen_t)SUN_LEN(&addr1)) <
		0) {
		perror("bind");
		exit(-1);
	}

	socklen_t len = sizeof(addr2);
	if (getsockname(sockfd, (struct sockaddr *)&addr2, &len)) {
		perror("sockfd");
		exit(-1);
	}
	printf("bound name = %s, returned len = %d\n", addr2.sun_path, len);

	unlink(argv[1]);

	exit(0);
}
