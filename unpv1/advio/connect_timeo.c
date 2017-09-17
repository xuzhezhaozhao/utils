/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月17日 星期日 21时43分30秒
 @File Name: connect_timeo.c
 @Description: unpv1 p301 使用 alarm 信号为 connect 函数设置超时时间，因为在
 多线程中正确使用信号非常困难，因此只建议在单线程程序中使用本技术
 ******************************************************/

#include "../unp.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <unistd.h>

static void connect_alarm(int);

// 正确返回 0, 出错则终止程序
int connect_timeo(int sockfd, const struct sockaddr *saptr, socklen_t salen,
				  int nsec) {
	Sigfunc *sigfunc;
	int n;

	sigfunc = unp_signal(SIGALRM, connect_alarm);
	if (sigfunc == SIG_ERR) {
		perror("unp_signal");
		exit(-1);
	}
	if (alarm(nsec) != 0) {
		fprintf(stderr, "connect_timeo: alarm was already set");
		exit(-1);
	}

	if ((n = connect(sockfd, saptr, salen)) < 0) {
		close(sockfd);
		if (errno == EINTR) {
			errno = ETIMEDOUT;
		}
	}

	alarm(0); /* turn off the alarm */
	if (unp_signal(SIGALRM, sigfunc) == SIG_ERR) {
		/* restore previous signal handler */
		perror("unp_signal");
		exit(-1);
	}

	return n;
}

static void connect_alarm(int signo) {
	(void)signo;
	return; /* just interrupt the connect() */
}
