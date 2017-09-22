/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月21日 星期一 14时36分49秒
 @File Name: zevent_epoll.cpp
 @Description:
 ******************************************************/

#include "zevent.hpp"

#include <stdlib.h>
#include <string.h>  // strerror()
#include <sys/epoll.h>
#include <unistd.h>

namespace utils {
namespace zevent {

struct zApiState {
	int epfd;
	struct epoll_event* events;
};

int BaseEvent::zApiCreate() {
	zApiState* state = (zApiState*)malloc(sizeof(zApiState));
	if (!state) {
		errmsg_ = "malloc: ";
		errmsg_ += strerror(errno);
		return -1;
	}
	state->events =
		(struct epoll_event*)malloc(sizeof(struct epoll_event) * setsize_);
	if (!state->events) {
		free(state);
		errmsg_ = "malloc: ";
		errmsg_ += strerror(errno);
		return -1;
	}
	state->epfd = epoll_create(1024); /* 1024 is just a hint for the kernel */
	if (state->epfd == -1) {
		free(state->events);
		free(state);
		errmsg_ = "epoll_create: ";
		errmsg_ += strerror(errno);
		return -1;
	}
	apidata_ = state;

	return 0;
}

int BaseEvent::zApiResize(int setsize) {
	zApiState* state = (zApiState*)apidata_;
	state->events = (struct epoll_event*)realloc(
		state->events, sizeof(struct epoll_event) * setsize);
	if (!state->events) {
		errmsg_ = "realloc: ";
		errmsg_ += strerror(errno);
		return -1;
	}

	return 0;
}

void BaseEvent::zApiFree() {
	if (!apidata_) {
		zApiState* state = (zApiState*)apidata_;
		close(state->epfd);
		free(state->events);
		free(state);
	}
}

int BaseEvent::zApiAddEvent(int fd, int mask) {
	zApiState* state = (zApiState*)apidata_;
	int op = (events_[fd].mask == Z_NONE) ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;

	struct epoll_event ee;
	ee.events = 0;
	mask |= events_[fd].mask;
	if (mask & Z_READABLE) {
		ee.events |= EPOLLIN;
	}
	if (mask & Z_WRITEABLE) {
		ee.events |= EPOLLOUT;
	}
	ee.data.fd = fd;
	if (epoll_ctl(state->epfd, op, fd, &ee) == -1) {
		errmsg_ = "epoll_ctl: ";
		errmsg_ += strerror(errno);
		return -1;
	}

	return 0;
}

int BaseEvent::zApiDelEvent(int fd, int delmask) {
	zApiState* state = (zApiState*)apidata_;
	int mask = events_[fd].mask & (~delmask);

	struct epoll_event ee;
	ee.events = 0;
	if (mask & Z_READABLE) {
		ee.events |= EPOLLIN;
	}
	if (mask & Z_WRITEABLE) {
		ee.events |= EPOLLOUT;
	}
	ee.data.fd = fd;
	int op = (mask == Z_NONE) ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;
	int ret = epoll_ctl(state->epfd, op, fd, &ee);
	if (ret == -1) {
		errmsg_ = "epoll_ctl: ";
		errmsg_ += strerror(errno);
		return -1;
	}

	return 0;
}

int BaseEvent::zApiPoll(struct timeval* tvp) {
	zApiState* state = (zApiState*)apidata_;
	int retval =
		epoll_wait(state->epfd, state->events, setsize_,
				   tvp ? (int)(tvp->tv_sec * 1000 + tvp->tv_usec / 1000) : -1);
	if (retval == -1) {
		errmsg_ = "epoll_pwait: ";
		errmsg_ += strerror(errno);
		return -1;
	}

	int numevents = 0;
	if (retval > 0) {
		numevents = retval;
		for (int j = 0; j < numevents; ++j) {
			int mask = 0;
			struct epoll_event* e = state->events + j;
			if (e->events & EPOLLIN) {
				mask |= Z_READABLE;
			}
			if (e->events & EPOLLOUT) {
				mask |= Z_WRITEABLE;
			}
			if (e->events & EPOLLERR) {
				mask |= Z_WRITEABLE;
			}
			if (e->events & EPOLLHUP) {
				mask |= Z_WRITEABLE;
			}
			fired_[j].fd = e->data.fd;
			fired_[j].mask = mask;
		}
	}
	return numevents;
}

const char* zApiName() { return "epoll"; }

}  // namespace zevent
}  // namespace utils
