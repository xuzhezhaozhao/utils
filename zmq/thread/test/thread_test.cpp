/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 20 Sep 2017 07:57:04 PM CST
 @File Name: thread_test.cpp
 @Description:
 ******************************************************/
#include "macros/macros.h"
#include "../thread.hpp"

#include <stdio.h>
#include <unistd.h>

void f(void *arg) {
	UNUSED(arg);
	int *pn = (int *)arg;
	printf("start a thread %d.\n", *pn);
}

int main(int argc, char *argv[]) {
	UNUSED(argc);
	UNUSED(argv);

	utils::uthread_t th;
	int n = 30;
	th.start(f, &n);
	th.stop();

	return 0;
}
