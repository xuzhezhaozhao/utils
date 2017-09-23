/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月23日 星期六 16时07分24秒
 @File Name: poller_base.hpp
 @Description: from libzmq/src/poller_base.hpp
 ******************************************************/

#ifndef UTISL_POLLER_POLLER_BASE_H_
#define UTISL_POLLER_POLLER_BASE_H_

#include <map>

#include "atomic_counter/atomic_counter.hpp"
#include "clock/clock.hpp"

namespace utils {

struct i_poll_events;

class poller_base_t {
public:
	poller_base_t();
	virtual ~poller_base_t();

	//  Returns load of the poller. Note that this function can be
	//  invoked from a different thread!
	int get_load();

	//  Add a timeout to expire in timeout_ milliseconds. After the
	//  expiration timer_event on sink_ object will be called with
	//  argument set to id_.
	void add_timer(int timeout_, utils::i_poll_events *sink_, int id_);

	//  Cancel the timer created by sink_ object with ID equal to id_.
	void cancel_timer(utils::i_poll_events *sink_, int id_);

protected:
	//  Called by individual poller implementations to manage the load.
	void adjust_load(int amount_);

	//  Executes any timers that are due. Returns number of milliseconds
	//  to wait to match the next timer or 0 meaning "no timers".
	uint64_t execute_timers();

private:
	//  Clock instance private to this I/O thread.
	uclock_t clock;

	//  List of active timers.
	struct timer_info_t {
		utils::i_poll_events *sink;
		int id;
	};
	typedef std::multimap<uint64_t, timer_info_t> timers_t;
	timers_t timers;

	//  Load of the poller. Currently the number of file descriptors
	//  registered.
	atomic_counter_t load;

	poller_base_t(const poller_base_t &);
	const poller_base_t &operator=(const poller_base_t &);
};
}

#endif
