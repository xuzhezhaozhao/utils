/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月23日 星期六 15时32分10秒
 @File Name: clock.hpp
 @Description: from libzmq/src/clock.hpp
 ******************************************************/

#ifndef UTILS_CLOCK_CLOCK_H_
#define UTILS_CLOCK_CLOCK_H_

#include <stdint.h>

namespace utils {

class uclock_t {
public:
	uclock_t();
	~uclock_t();

	//  CPU's timestamp counter. Returns 0 if it's not available.
	static uint64_t rdtsc();

	//  High precision timestamp.
	static uint64_t now_us();

	//  Low precision timestamp. In tight loops generating it can be
	//  10 to 100 times faster than the high precision timestamp.
	uint64_t now_ms();

private:
	//  TSC timestamp of when last time measurement was made.
	uint64_t last_tsc;

	//  Physical time corresponding to the TSC above (in milliseconds).
	uint64_t last_time;

	uclock_t(const uclock_t &);
	const uclock_t &operator=(const uclock_t &);
};
}

#endif
