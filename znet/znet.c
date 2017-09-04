/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月04日 星期一 16时52分56秒
 @File Name: znet.c
 @Description:
 ******************************************************/

#include "znet.h"

// unpv1 p87 (lib/sock_ntop.c)
const char* zsock_ntop(const struct sockaddr* sa, socklen_t salen) {
	char portstr[8];
	static char str[128];
	switch (sa->sa_family) {
		case AF_INET: {
			struct sockaddr_in* sin = (struct sockaddr_in*)sa;
			if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL) {
				return NULL;
			}
			if (ntohs(sin->sin_port) != 0) {
				snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
				strcat(str, portstr);
			}
			return str;
		}
		case AF_INET6: {
			struct sockaddr_in6* sin6 = (struct sockaddr_in6*)sa;
			str[0] = '[';
			if (inet_ntop(AF_INET6, &sin6->sin6_addr, str + 1,
						  sizeof(str) - 1) == NULL)
				return (NULL);
			if (ntohs(sin6->sin6_port) != 0) {
				snprintf(portstr, sizeof(portstr), "]:%d",
						 ntohs(sin6->sin6_port));
				strcat(str, portstr);
				return (str);
			}
			return (str + 1);
		}
		case AF_UNIX: {
			struct sockaddr_un* unp = (struct sockaddr_un*)sa;

			/* OK to have no pathname bound to the socket: happens on
			   every connect() unless client calls bind() first. */
			if (unp->sun_path[0] == '\0')
				strcpy(str, "(no pathname bound)");
			else
				snprintf(str, sizeof(str), "%s", unp->sun_path);
			return (str);
		}
		default:
			snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d, len %d",
					 sa->sa_family, salen);
			return (str);
	}
	return NULL;
}

// unpv1 p72 (lib/readn.c)
// Read "n" bytes from a descriptor.
ssize_t zreadn(int fd, void* buf, size_t count) {
	char* ptr = (char*)buf;
	size_t nleft = count;
	while (nleft > 0) {
		ssize_t nread = 0;
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR) {
				nread = 0;  // and call read() again
			} else {
				return -1;
			}
		} else if (nread == 0) {
			break;  // EOF
		}

		nleft -= nread;
		ptr += nread;
	}
	return (count - nleft);  // return >= 0
}

// unpv1 p73 (lib/writen.c)
// Write "n" bytes to a descriptor.
ssize_t zwriten(int fd, const void* buf, size_t count) {
	const char* ptr = (char*)buf;
	ssize_t nleft = count;
	while (nleft > 0) {
		ssize_t nwritten = 0;
		if ((nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR) {
				nwritten = 0;  // and call write() again
			} else {
				return -1;  // error
			}
		}

		nleft -= nwritten;
		ptr += nwritten;
	}
	return count;
}

// unpv1 p96 (lib/sockfd_to_family.c)
int sockfd_to_family(int sockfd) {
	// 通用地址结构 sockaddr_storage 提供足够的大小存储任意地址类型
	struct sockaddr_storage ss;
	socklen_t len = sizeof(ss);
	if (getsockname(sockfd, (struct sockaddr*)&ss, &len) == -1) {
		return -1;
	}
	return ss.ss_family;
}
