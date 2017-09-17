/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月17日 星期日 18时06分02秒
 @File Name: exitstatus.c
 @Description: apue p362
 ******************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void *thr_fn1(void *arg) {
	(void)arg;
	printf("thread 1 returning\n");
	return ((void *)1);
}

void *thr_fn2(void *arg) {
	(void)arg;
	printf("thread 2 exiting\n");
	pthread_exit((void *)2);
}

int main(void) {
	int err;
	pthread_t tid1, tid2;
	void *tret;

	err = pthread_create(&tid1, NULL, thr_fn1, NULL);
	if (err != 0) {
		fprintf(stderr, "can't create thread 1: %s\n", strerror(err));
		exit(-1);
	}
	err = pthread_create(&tid2, NULL, thr_fn2, NULL);
	if (err != 0) {
		fprintf(stderr, "can't create thread 2: %s\n", strerror(err));
		exit(-1);
	}
	err = pthread_join(tid1, &tret);
	if (err != 0) {
		fprintf(stderr, "can't join with thread 1: %s\n", strerror(err));
		exit(-1);
	}
	printf("thread 1 exit code %ld\n", (long) tret);
	err = pthread_join(tid2, &tret);
	if (err != 0) {
		fprintf(stderr, "can't join with thread 2: %s\n", strerror(err));
		exit(-1);
	}
	printf("thread 2 exit code %ld\n", (long)tret);
	exit(0);
}
