/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月16日 星期六 20时01分35秒
 @File Name: hostent.c
 @Description: gethostbyname 函数示例, 命令行参数为名字地址列表，如
 	$ ./hostent baidu.com tencent.com
 ******************************************************/

#define INET_ADDRSTRLEN 16
#define INET6_ADDRSTRLEN 46

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

int main(int argc, char **argv) {
	char str[INET_ADDRSTRLEN];
	while (--argc > 0) {
		char *ptr = *++argv;
		struct hostent *hptr = NULL;
		if ((hptr = gethostbyname(ptr)) == NULL) {
			fprintf(stderr, "gethostbyname error for host: %s: %s\n", ptr,
					hstrerror(h_errno));
			continue;
		}
		printf("official hostname: %s\n", hptr->h_name);

		char **pptr = NULL;
		for (pptr = hptr->h_aliases; *pptr != NULL; pptr++) {
			printf("\talias: %s\n", *pptr);
		}

		switch (hptr->h_addrtype) {
			case AF_INET:
				pptr = hptr->h_addr_list;
				for (; *pptr != NULL; pptr++) {
					const char *addr =
						inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str));
					if (addr == NULL) {
						perror("inet_ntop");
						exit(-1);
					}
					printf("\taddress: %s\n", addr);
				}
				break;

			default:
				fprintf(stderr, "unknown address type");
				break;
		}
	}
	return 0;
}
