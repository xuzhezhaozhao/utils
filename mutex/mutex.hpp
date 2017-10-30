/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Tue 19 Sep 2017 07:42:57 PM CST
 @File Name: mutex.hpp
 @Description: from libzmq/src/mutex.hpp
 ******************************************************/

#ifndef UTILS_MUTEX_HPP_
#define UTILS_MUTEX_HPP_

#include "error/err.h"

#include <pthread.h>

namespace utils {

class mutex_t {
public:
	inline mutex_t() {
		int rc = pthread_mutexattr_init(&attr);
		posix_assert(rc);

		rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		posix_assert(rc);

		rc = pthread_mutex_init(&mutex, &attr);
		posix_assert(rc);
	}

	inline ~mutex_t() {
		int rc = pthread_mutex_destroy(&mutex);
		posix_assert(rc);

		rc = pthread_mutexattr_destroy(&attr);
		posix_assert(rc);
	}

	inline void lock() {
		int rc = pthread_mutex_lock(&mutex);
		posix_assert(rc);
	}

	inline bool try_lock() {
		int rc = pthread_mutex_trylock(&mutex);
		if (rc == EBUSY) {
			return false;
		}

		posix_assert(rc);
		return true;
	}

	inline void unlock() {
		int rc = pthread_mutex_unlock(&mutex);
		posix_assert(rc);
	}

	inline pthread_mutex_t* get_mutex() { return &mutex; }

private:
	pthread_mutex_t mutex;
	pthread_mutexattr_t attr;

	// Disable copy construction and assignment.
	mutex_t(const mutex_t&);
	const mutex_t& operator=(const mutex_t&);
};

class scoped_lock_t {
public:
	scoped_lock_t(mutex_t& mutex_) : mutex(mutex_) { mutex.lock(); }
	~scoped_lock_t() { mutex.unlock(); }

private:
	mutex_t& mutex;

	// Disable copy construction and assignment.
	scoped_lock_t(const scoped_lock_t&);
	const scoped_lock_t& operator=(const scoped_lock_t&);
};

class scoped_optional_lock_t {
public:
	scoped_optional_lock_t(mutex_t* mutex_) : mutex(mutex_) {
		if (mutex != NULL) {
			mutex->lock();
		}
	}

	~scoped_optional_lock_t() {
		if (mutex != NULL) {
			mutex->unlock();
		}
	}

private:
	mutex_t* mutex;

	// Disable copy construction and assignment.
	scoped_optional_lock_t(const scoped_lock_t&);
	const scoped_optional_lock_t& operator=(const scoped_lock_t&);
};
}

#endif
