/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 20 Sep 2017 07:45:49 PM CST
 @File Name: thread.cpp
 @Description: from libzmq/src/thread.cpp
 ******************************************************/

#include "thread.hpp"
#include "error/err.h"
#include "macros/macros.h"

#include <signal.h>
#include <unistd.h>

extern "C" {
static void *thread_routine(void *arg_) {
	//  Following code will guarantee more predictable latencies as it'll
	//  disallow any signal handling in the I/O thread.
	sigset_t signal_set;
	int rc = sigfillset(&signal_set);
	errno_assert(rc == 0);
	rc = pthread_sigmask(SIG_BLOCK, &signal_set, NULL);
	posix_assert(rc);

	utils::uthread_t *self = (utils::uthread_t *)arg_;
	self->tfn(self->arg);
	return NULL;
}
}

void utils::uthread_t::start(thread_fn *tfn_, void *arg_) {
	tfn = tfn_;
	arg = arg_;
	int rc = pthread_create(&descriptor, NULL, thread_routine, this);
	posix_assert(rc);
}

void utils::uthread_t::stop() {
	int rc = pthread_join(descriptor, NULL);
	posix_assert(rc);
}

void utils::uthread_t::setSchedulingParameters(int priority_,
											  int schedulingPolicy_) {
#if defined _POSIX_THREAD_PRIORITY_SCHEDULING && \
	_POSIX_THREAD_PRIORITY_SCHEDULING >= 0
	int policy = 0;
	struct sched_param param;

#if _POSIX_THREAD_PRIORITY_SCHEDULING == 0 && \
	defined _SC_THREAD_PRIORITY_SCHEDULING
	if (sysconf(_SC_THREAD_PRIORITY_SCHEDULING) < 0) {
		return;
	}
#endif
	int rc = pthread_getschedparam(descriptor, &policy, &param);
	posix_assert(rc);

	if (priority_ != -1) {
		param.sched_priority = priority_;
	}

	if (schedulingPolicy_ != -1) {
		policy = schedulingPolicy_;
	}

#ifdef __NetBSD__
	if (policy == SCHED_OTHER) param.sched_priority = -1;
#endif

	rc = pthread_setschedparam(descriptor, policy, &param);

#ifdef __FreeBSD_kernel__
	// If this feature is unavailable at run-time, don't abort.
	if (rc == ENOSYS) return;
#endif

	posix_assert(rc);
#else

	UNUSED(priority_);
	UNUSED(schedulingPolicy_);
#endif
}
