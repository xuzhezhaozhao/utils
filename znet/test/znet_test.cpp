/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月04日 星期一 17时04分21秒
 @File Name: znet_test.cpp
 @Description:
 ******************************************************/

#include "gtest/gtest.h"
#include "../znet.h"

TEST(zsock_ntop, ipv4) {
	struct in_addr addr;
	if (inet_pton(AF_INET, "196.168.0.111", &addr) == -1) {
		perror("inet_pton: ");
	}
	struct sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_addr = addr;
	serv.sin_port = htons(54096);

	const char* str = zsock_ntop((struct sockaddr *)&serv, sizeof(serv));
	EXPECT_TRUE(!strcmp(str, "196.168.0.111:54096"));
}

TEST(zsock_ntop, ipv6) {
	struct in6_addr addr;
	if (inet_pton(AF_INET6, "ff00:ff00:1:2:3:4:5:ffff", &addr) == -1) {
		perror("inet_pton: ");
	}
	struct sockaddr_in6 serv;
	serv.sin6_family = AF_INET6;
	serv.sin6_addr = addr;
	serv.sin6_port = htons(54096);
	const char* str = zsock_ntop((struct sockaddr *)&serv, sizeof(serv));
	EXPECT_TRUE(!strcmp(str, "[ff00:ff00:1:2:3:4:5:ffff]:54096"));
}

TEST(zsock_ntop, unix) {
	struct sockaddr_un serv;
	serv.sun_family = AF_UNIX;
	strncpy(serv.sun_path, "/var/example.socket", sizeof(serv.sun_path));
	const char* str = zsock_ntop((struct sockaddr *)&serv, sizeof(serv));
	EXPECT_TRUE(!strcmp(str, "/var/example.socket"));
}

TEST(sockfd_to_family, all) {
	int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	ASSERT_TRUE(sockfd > 0);
	EXPECT_TRUE(sockfd_to_family(sockfd) == AF_UNIX);
	close(sockfd);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	EXPECT_TRUE(sockfd_to_family(sockfd) == AF_INET);
	close(sockfd);
	sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	EXPECT_TRUE(sockfd_to_family(sockfd) == AF_INET6);
	close(sockfd);
}
