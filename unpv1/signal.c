/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月05日 星期二 22时45分59秒
 @File Name: signal.c
 @Description:
 ******************************************************/

#include "unp.h"
#include <signal.h>

Sigfunc* unp_signal(int signo, Sigfunc* func) {
	struct sigaction act, oact;
	act.sa_handler = func;
	// 不阻塞其他信号, POSIX 规定当前处理的信号是阻塞的;
	// 一个信号在被阻塞期间产生了一次或多次，那么该信号被解阻塞之后通常只递交
	// 一次，也就是说Unix信号默认是不排队的
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT; /* SunOS 4.x */
#endif
	} else {
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART; /* SVR4, 44BSD */
#endif
	}
	if (sigaction(signo, &act, &oact) < 0) {
		return SIG_ERR;
	}
	return oact.sa_handler;
}
