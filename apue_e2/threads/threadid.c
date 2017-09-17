/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月17日 星期日 17时39分23秒
 @File Name: threadid.c
 @Description: apue p359 打印线程 id
 ******************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

pthread_t ntid;

void printids(const char *s) {
	pid_t pid;
	pthread_t tid;

	pid = getpid();
	tid = pthread_self();
	printf("%s pid %u tid %u (0x%x)\n", s, (unsigned int)pid, (unsigned int)tid,
		   (unsigned int)tid);
}

void *thr_fn(void *arg) {
	(void)arg;
	printids("new thread: ");
	return ((void *)0);
}

int main(void) {
	int err;

	err = pthread_create(&ntid, NULL, thr_fn, NULL);
	if (err != 0) {
		fprintf(stderr, "can't create thread: %s\n", strerror(err));
		exit(-1);
	}
	printids("main thread:");
	sleep(1); /* 防止线程竞争 */
	return 0;
}
