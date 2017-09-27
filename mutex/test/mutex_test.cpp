/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Tue 19 Sep 2017 08:01:14 PM CST
 @File: mutex_test.cpp
 @Description:
 ******************************************************/

#include "../mutex.hpp"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

void *thread_start(void *arg) {
	int n = *((int *)(arg));
	const char *msg =
		"This is a test print, should not be interact with other threads, to "
		"test, we want this string to be long enough, "
		"thread id is ";

	utils::mutex_t mutex;
	utils::scoped_lock_t locker(mutex);

	std::cout << msg << n << std::endl;
	return arg;
}

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	pthread_attr_t attr;

	if (pthread_attr_init(&attr)) {
		perror("pthread_attr_init");
		exit(-1);
	}

	const int threadnum = 10;
	pthread_t pid[threadnum];
	int args[threadnum];
	for (int i = 0; i < threadnum; ++i) {
		args[i] = i;
	}

	for (int i = 0; i < threadnum; ++i) {
		if (pthread_create(&pid[i], &attr, thread_start, &args[i]) != 0) {
			perror("pthread_create");
			continue;
		}
	}

	if (pthread_attr_destroy(&attr) != 0) {
		perror("pthread_attr_destroy");
		exit(-1);
	}

	void *res;
	for (int i = 0; i < threadnum; ++i) {
		if (pthread_join(pid[i], &res) != 0) {
			perror("pthread_join");
			continue;
		}
		printf("join %d\n", *((int *)res));
	}

	return 0;
}
