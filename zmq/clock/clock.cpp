/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月23日 星期六 15时36分03秒
 @File Name: clock.cpp
 @Description: from libzmq/src/clock.cpp
 ******************************************************/

#include "clock.hpp"
#include "error/err.h"
#include "likely/likely.h"

#include <stddef.h>
#include <sys/time.h>
#include <time.h>

utils::clock_t::clock_t() : last_tsc(rdtsc()), last_time(now_us() / 1000) {}

utils::clock_t::~clock_t() {}

uint64_t utils::clock_t::now_us() {
	//  Use POSIX clock_gettime function to get precise monotonic time.
	struct timespec tv;
	int rc = clock_gettime(CLOCK_MONOTONIC, &tv);
	// Fix case where system has clock_gettime but CLOCK_MONOTONIC is not
	// supported.
	// This should be a configuration check, but I looked into it and writing an
	// AC_FUNC_CLOCK_MONOTONIC seems beyond my powers.
	if (rc != 0) {
		//  Use POSIX gettimeofday function to get precise time.
		struct timeval tv;
		int rc = gettimeofday(&tv, NULL);
		errno_assert(rc == 0);
		return (tv.tv_sec * (uint64_t)1000000 + tv.tv_usec);
	}
	return (tv.tv_sec * (uint64_t)1000000 + tv.tv_nsec / 1000);
}

uint64_t utils::clock_t::now_ms() {
	uint64_t tsc = rdtsc();

	//  If TSC is not supported, get precise time and chop off the microseconds.
	if (!tsc) {
		return now_us() / 1000;
	}

	//  Low-precision clock precision in CPU ticks. 1ms. Value of 1000000
	//  should be OK for CPU frequencies above 1GHz. If should work
	//  reasonably well for CPU frequencies above 500MHz. For lower CPU
	//  frequencies you may consider lowering this value to get best
	//  possible latencies.
	const int clock_precision = 1000000;

	//  If TSC haven't jumped back (in case of migration to a different
	//  CPU core) and if not too much time elapsed since last measurement,
	//  we can return cached time value.
	if (likely(tsc - last_tsc <= (clock_precision / 2) && tsc >= last_tsc)) {
		return last_time;
	}

	last_tsc = tsc;
	last_time = now_us() / 1000;
	return last_time;
}

uint64_t utils::clock_t::rdtsc() {
	uint32_t low, high;
	__asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
	return (uint64_t)high << 32 | low;
}
