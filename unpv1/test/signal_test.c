/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月13日 星期三 20时10分58秒
 @File Name: signal_test.c
 @Description:
 ******************************************************/

#include "../unp.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void test_handle(int signo) { printf("receive signal %d\n", signo); }

#define INSTALL_HANDLE(sig)                        \
	if (unp_signal(sig, test_handle) == SIG_ERR) { \
		perror("sigaction");                       \
		exit(-1);                                  \
	}

int main() {
	INSTALL_HANDLE(SIGHUP);
	INSTALL_HANDLE(SIGINT); // ctrl-c
	INSTALL_HANDLE(SIGQUIT);
	INSTALL_HANDLE(SIGILL);
	INSTALL_HANDLE(SIGABRT);
	INSTALL_HANDLE(SIGFPE);
	// INSTALL_HANDLE(SIGKILL);  // cannot be caught, blocked, or ignored
	INSTALL_HANDLE(SIGSEGV);
	INSTALL_HANDLE(SIGPIPE);
	INSTALL_HANDLE(SIGALRM);
	INSTALL_HANDLE(SIGTERM);
	INSTALL_HANDLE(SIGUSR1);
	INSTALL_HANDLE(SIGUSR2);
	INSTALL_HANDLE(SIGCHLD);
	INSTALL_HANDLE(SIGCONT);
	// INSTALL_HANDLE(SIGSTOP);  // cannot be caught, blocked, or ignored
	INSTALL_HANDLE(SIGTSTP); // ctrl-z
	INSTALL_HANDLE(SIGTTIN);
	INSTALL_HANDLE(SIGTTOU);

	INSTALL_HANDLE(SIGBUS);
	INSTALL_HANDLE(SIGPOLL);
	INSTALL_HANDLE(SIGPROF);
	INSTALL_HANDLE(SIGSYS);
	INSTALL_HANDLE(SIGTRAP);
	INSTALL_HANDLE(SIGVTALRM);
	INSTALL_HANDLE(SIGXCPU);
	INSTALL_HANDLE(SIGXFSZ);

	while (1) {
		int seconds = 30;
		printf("sleeping...\n");
		int elapse = sleep(seconds);
		if (elapse != 0) {
			printf("wake up by signal.\n");
		} else {
			printf("elapse %d seconds, wake up.\n", seconds);
		}
	}

	return 0;
}
