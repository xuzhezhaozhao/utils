/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月23日 星期六 16时28分01秒
 @File Name: epoll.hpp
 @Description: from libzmq/src/epoll.hpp
 ******************************************************/

#ifndef UTILS_POLLER_EPOLL_H_
#define UTILS_POLLER_EPOLL_H_

#include "mutex/mutex.hpp"
#include "poller_base.hpp"
#include "thread/thread.hpp"

#include <sys/epoll.h>
#include <vector>

namespace utils {

struct i_poll_events;

//  This class implements socket polling mechanism using the Linux-specific
//  epoll mechanism.

class epoll_t : public poller_base_t {
public:
	typedef void *handle_t;

	//  Maximum number of events the I/O thread can process in one go.
	static const int max_io_events = 256;

	enum { retired_fd = -1 };

	epoll_t();
	~epoll_t();

	//  "poller" concept.
	handle_t add_fd(int fd_, utils::i_poll_events *events_);
	void rm_fd(handle_t handle_);
	void set_pollin(handle_t handle_);
	void reset_pollin(handle_t handle_);
	void set_pollout(handle_t handle_);
	void reset_pollout(handle_t handle_);
	void start();
	void stop();

	static int max_fds();

private:
	//  Main worker thread routine.
	static void worker_routine(void *arg_);

	//  Main event loop.
	void loop();

	//  Main epoll file descriptor
	int epoll_fd;

	struct poll_entry_t {
		int fd;
		epoll_event ev;
		utils::i_poll_events *events;
	};

	//  List of retired event sources.
	typedef std::vector<poll_entry_t *> retired_t;
	retired_t retired;

	//  If true, thread is in the process of shutting down.
	bool stopping;

	//  Handle of the physical thread doing the I/O work.
	uthread_t worker;

	//  Synchronisation of retired event sources
	mutex_t retired_sync;

	epoll_t(const epoll_t &);
	const epoll_t &operator=(const epoll_t &);
};

typedef epoll_t poller_t;
}

#endif
