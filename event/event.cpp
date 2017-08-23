/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月21日 星期一 14时36分49秒
 @File Name: event.cpp
 @Description:
 ******************************************************/

#include "event.h"

#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <string.h>

namespace utils {

static void* reallocv_(void *block, size_t /* osize */, size_t nsize) {
	void *newblock = realloc(block, nsize);
	if (!newblock) {
		// TODO 错误处理
		return NULL;
	}

	return newblock;
}

static void *reallocv(void *block, size_t onum, size_t nnum, size_t esize) {
	return reallocv_(block, onum * esize, nnum * esize);
}

static void *creallocv(void *block, size_t onum, size_t nnum, size_t esize) {
	void *newblock = reallocv(block, onum, nnum, esize);
	if (!newblock) {
		return NULL;
	}
	if (onum < nnum) {
		memset((char *)newblock + onum * esize, 0, (nnum - onum) * esize);
	}
	return block;
}

int BaseEvent::AddEvent(int fd, int events, void *args, event_handle_t event_handle) {
	// 自动扩容
	if (fd >= size_) {
		int newsize = size_ * 2;
		void * newblock = 
			creallocv(event_info_vector_, size_, newsize, sizeof(event_info));
		if (!newblock) {
			// TODO 出错处理
			return -1;
		}
		event_info_vector_ = (event_info *)newblock;
	}

	struct event_info * ev_info = get_event_info(fd);
	int op;
	struct epoll_event ev;
	ev.data.fd = fd;
	if (!ev_info->valid) {
		// 新增 
		ev_info->events = events;
		ev_info->valid = 1;
		op = EPOLL_CTL_ADD;
	} else {
		// 修改
		ev_info->events |= events;
		op = EPOLL_CTL_MOD;
	}

	if (ev_info->events & EV_READABLE) {
		ev.events |= EPOLLIN;
	}
	if (ev_info->events & EV_WRITEABLE) {
		ev.events |= EPOLLOUT;
	}
	ev_info->handle = event_handle;
	ev_info->args = args;
	if (epoll_ctl(epfd_, op, ev_info->fd, &ev) == -1) {
		// TODO 出错信息
		return -1;
	}

	return 0;
}

// timeout: ms
int BaseEvent::AddTimerEvent(int timeout, void *args, time_handle_t time_handle) {
	return 0;
}

int BaseEvent::DelEvent(int fd) {
	struct event_info * ev_info = get_event_info(fd);
	if (!ev_info->valid) {
		return 0;
	}
	if (epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL) == -1) {
		// TODO 出错处理
		return -1;
	}
	ev_info->valid = 0;
	return 0;
}

int BaseEvent::init() {
	epfd_ = epoll_create(1024);
	if (epfd_ == -1) {
		// TODO 出错信息
		return -1;
	}

	event_info_vector_ = 
		(event_info *)calloc(EVENT_INFO_VECTOR_INIT_SIZE, sizeof(event_info));
	if (!event_info_vector_) {
		// TODO 出错信息
		return -1;
	}
	return 0;
}

void BaseEvent::event_loop() {
	// TODO 加入 timer 处理
	// TODO 数组大小
	struct epoll_event events[128];
	while (true) {
		int nfds;
		if ((nfds = epoll_wait(epfd_, events, 128, -1)) < 0) {
			// TODO 错误处理
			break;
		}
		for (int i = 0; i < nfds; ++i) {
			int fd = events[i].data.fd;
			int mask = events[i].events;
			get_event_info(fd)->handle(fd, mask, NULL);
		}
	}
}

}
