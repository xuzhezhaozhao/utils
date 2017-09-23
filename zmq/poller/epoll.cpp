/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月23日 星期六 16时57分23秒
 @File Name: epoll.cpp
 @Description: from libzmq/src/epoll.cpp
 ******************************************************/

#include "epoll.hpp"
#include "error/err.h"
#include "i_poll_events.hpp"
#include "macros/macros.h"

#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <algorithm>
#include <new>

utils::epoll_t::epoll_t() : stopping(false) {
	//  Setting this option result in sane behaviour when exec() functions
	//  are used. Old sockets are closed and don't block TCP ports, avoid
	//  leaks, etc.
	epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	errno_assert(epoll_fd != -1);
}

utils::epoll_t::~epoll_t() {
	//  Wait till the worker thread exits.
	worker.stop();

	close(epoll_fd);
	for (retired_t::iterator it = retired.begin(); it != retired.end(); ++it) {
		UTILS_DELETE(*it);
	}
}

utils::epoll_t::handle_t utils::epoll_t::add_fd(int fd_,
												i_poll_events *events_) {
	poll_entry_t *pe = new (std::nothrow) poll_entry_t;
	alloc_assert(pe);

	//  The memset is not actually needed. It's here to prevent debugging
	//  tools to complain about using uninitialised memory.
	memset(pe, 0, sizeof(poll_entry_t));

	pe->fd = fd_;
	pe->ev.events = 0;
	pe->ev.data.ptr = pe;
	pe->events = events_;

	int rc = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_, &pe->ev);
	errno_assert(rc != -1);

	//  Increase the load metric of the thread.
	adjust_load(1);

	return pe;
}

void utils::epoll_t::rm_fd(handle_t handle_) {
	poll_entry_t *pe = (poll_entry_t *)handle_;
	int rc = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, pe->fd, &pe->ev);
	errno_assert(rc != -1);
	pe->fd = retired_fd;
	retired_sync.lock();
	retired.push_back(pe);
	retired_sync.unlock();

	//  Decrease the load metric of the thread.
	adjust_load(-1);
}

void utils::epoll_t::set_pollin(handle_t handle_) {
	poll_entry_t *pe = (poll_entry_t *)handle_;
	pe->ev.events |= EPOLLIN;
	int rc = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, pe->fd, &pe->ev);
	errno_assert(rc != -1);
}

void utils::epoll_t::reset_pollin(handle_t handle_) {
	poll_entry_t *pe = (poll_entry_t *)handle_;
	pe->ev.events &= ~((short)EPOLLIN);
	int rc = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, pe->fd, &pe->ev);
	errno_assert(rc != -1);
}

void utils::epoll_t::set_pollout(handle_t handle_) {
	poll_entry_t *pe = (poll_entry_t *)handle_;
	pe->ev.events |= EPOLLOUT;
	int rc = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, pe->fd, &pe->ev);
	errno_assert(rc != -1);
}

void utils::epoll_t::reset_pollout(handle_t handle_) {
	poll_entry_t *pe = (poll_entry_t *)handle_;
	pe->ev.events &= ~((short)EPOLLOUT);
	int rc = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, pe->fd, &pe->ev);
	errno_assert(rc != -1);
}

static void start_thread(utils::uthread_t &thread_, utils::thread_fn *tfn_,
						 void *arg_) {
	thread_.start(tfn_, arg_);
	const int thread_priority = -1;		 // default
	const int thread_sched_policy = -1;  // default
	thread_.setSchedulingParameters(thread_priority, thread_sched_policy);
}

void utils::epoll_t::start() { start_thread(worker, worker_routine, this); }

void utils::epoll_t::stop() { stopping = true; }

int utils::epoll_t::max_fds() { return -1; }

void utils::epoll_t::loop() {
	epoll_event ev_buf[max_io_events];

	while (!stopping) {
		//  Execute any due timers.
		int timeout = (int)execute_timers();

		//  Wait for events.
		int n = epoll_wait(epoll_fd, &ev_buf[0], max_io_events,
						   timeout ? timeout : -1);
		if (n == -1) {
			errno_assert(errno == EINTR);
			continue;
		}

		for (int i = 0; i < n; i++) {
			poll_entry_t *pe = ((poll_entry_t *)ev_buf[i].data.ptr);

			if (pe->fd == retired_fd) continue;
			if (ev_buf[i].events & (EPOLLERR | EPOLLHUP)) {
				pe->events->in_event();
			}
			if (pe->fd == retired_fd) continue;
			if (ev_buf[i].events & EPOLLOUT) {
				pe->events->out_event();
			}
			if (pe->fd == retired_fd) continue;
			if (ev_buf[i].events & EPOLLIN) {
				pe->events->in_event();
			}
		}

		//  Destroy retired event sources.
		retired_sync.lock();
		for (retired_t::iterator it = retired.begin(); it != retired.end();
			 ++it) {
			UTILS_DELETE(*it);
		}
		retired.clear();
		retired_sync.unlock();
	}
}

void utils::epoll_t::worker_routine(void *arg_) { ((epoll_t *)arg_)->loop(); }
