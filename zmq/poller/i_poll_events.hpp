/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月23日 星期六 08时12分03秒
 @File Name: i_poll_events.hpp
 @Description: from libzmq/src/i_poll_events.hpp
 ******************************************************/
#ifndef UTIlS_POLLER_I_POLL_EVENTS_H_
#define UTIlS_POLLER_I_POLL_EVENTS_H_

namespace utils {

// Virtual interface to be exposed by object that want to be notified
// about events on file descriptors.

struct i_poll_events {
	virtual ~i_poll_events() {}

	// Called by I/O thread when file descriptor is ready for reading.
	virtual void in_event() = 0;

	// Called by I/O thread when file descriptor is ready for writing.
	virtual void out_event() = 0;

	// Called when timer expires.
	virtual void timer_event(int id_) = 0;
};
}

#endif
