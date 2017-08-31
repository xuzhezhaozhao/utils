/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月20日 星期日 23时15分17秒
 @File Name: sock_ntop.c
 @Description:
 ******************************************************/

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

// unpv1 p87 (lib/sock_ntop.c)
char *sock_ntop(const struct sockaddr *sa, socklen_t salen) {
	char portstr[8];
	static char str[128];
	switch (sa->sa_family) {
		case AF_INET: {
			struct sockaddr_in *sin = (struct sockaddr_in *)sa;
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
			struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)sa;

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
			struct sockaddr_un *unp = (struct sockaddr_un *)sa;

			/* OK to have no pathname bound to the socket: happens on
			   every connect() unless client calls bind() first. */
			if (unp->sun_path[0] == 0)
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
