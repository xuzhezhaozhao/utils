/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 20 Sep 2017 07:43:50 PM CST
 @File Name: thread.hpp
 @Description: from libzmq/src/thread.hpp
 ******************************************************/

#ifndef UTILS_ZMQ_THREAD_H_
#define UTILS_ZMQ_THREAD_H_

#include <pthread.h>

namespace utils {

typedef void(thread_fn)(void *);

//  Class encapsulating OS thread. Thread initiation/termination is done
//  using special functions rather than in constructor/destructor so that
//  thread isn't created during object construction by accident, causing
//  newly created thread to access half-initialised object. Same applies
//  to the destruction process: Thread should be terminated before object
//  destruction begins, otherwise it can access half-destructed object.

class uthread_t {
public:
	inline uthread_t() : tfn(NULL), arg(NULL) {}

	//  Creates OS thread. 'tfn' is main thread function. It'll be passed
	//  'arg' as an argument.
	void start(thread_fn *tfn_, void *arg_);

	//  Waits for thread termination.
	void stop();

	// Sets the thread scheduling parameters. Only implemented for
	// pthread. Has no effect on other platforms.
	void setSchedulingParameters(int priority_, int schedulingPolicy_);

	//  These are internal members. They should be private, however then
	//  they would not be accessible from the main C routine of the thread.
	thread_fn *tfn;
	void *arg;

private:
	pthread_t descriptor;

	uthread_t(const uthread_t &);
	const uthread_t &operator=(const uthread_t &);
};
}

#endif
