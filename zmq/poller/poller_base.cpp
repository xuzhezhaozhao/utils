/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月23日 星期六 16时09分22秒
 @File Name: poller_base.cpp
 @Description: from libzmq/src/poller_base.cpp
 ******************************************************/

#include "poller_base.hpp"
#include "i_poll_events.hpp"

#include "error/err.h"

utils::poller_base_t::poller_base_t() {}

utils::poller_base_t::~poller_base_t() {
	//  Make sure there is no more load on the shutdown.
	utils_assert(get_load() == 0);
}

int utils::poller_base_t::get_load() { return load.get(); }

void utils::poller_base_t::adjust_load(int amount_) {
	if (amount_ > 0) {
		load.add(amount_);
	}
	else if (amount_ < 0) {
		load.sub(-amount_);
	}
}

void utils::poller_base_t::add_timer(int timeout_, i_poll_events *sink_,
								   int id_) {
	uint64_t expiration = clock.now_ms() + timeout_;
	timer_info_t info = {sink_, id_};
	timers.insert(timers_t::value_type(expiration, info));
}

void utils::poller_base_t::cancel_timer(i_poll_events *sink_, int id_) {
	//  Complexity of this operation is O(n). We assume it is rarely used.
	for (timers_t::iterator it = timers.begin(); it != timers.end(); ++it)
		if (it->second.sink == sink_ && it->second.id == id_) {
			timers.erase(it);
			return;
		}

	//  Timer not found.
	utils_assert(false);
}

uint64_t utils::poller_base_t::execute_timers() {
	//  Fast track.
	if (timers.empty()) return 0;

	//  Get the current time.
	uint64_t current = clock.now_ms();

	//   Execute the timers that are already due.
	timers_t::iterator it = timers.begin();
	while (it != timers.end()) {
		//  If we have to wait to execute the item, same will be true about
		//  all the following items (multimap is sorted). Thus we can stop
		//  checking the subsequent timers and return the time to wait for
		//  the next timer (at least 1ms).
		if (it->first > current) return it->first - current;

		//  Trigger the timer.
		it->second.sink->timer_event(it->second.id);

		//  Remove it from the list of active timers.
		timers_t::iterator o = it;
		++it;
		timers.erase(o);
	}

	//  There are no more timers.
	return 0;
}
