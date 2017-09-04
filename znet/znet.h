/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月04日 星期一 17时10分52秒
 @File Name: znet.h
 @Description:
 ******************************************************/

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const char *zsock_ntop(const struct sockaddr *sa, socklen_t salen);

extern ssize_t zreadn(int fd, void* buf, size_t count);
extern ssize_t zwriten(int fd, const void* buf, size_t count);
extern int sockfd_to_family(int sockfd);

#ifdef __cplusplus
}
#endif
