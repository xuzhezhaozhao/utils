/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月03日 星期日 13时44分24秒
 @File Name: zevent.cpp
 @Description:
 ******************************************************/

#include "zevent.h"

// TODO
#include "zevent_epoll.cpp"

namespace utils {
namespace zevent {

BaseEvent::~BaseEvent() {
	zApiFree();
	free(events_);
	free(fired_);
}

int BaseEvent::init(int setsize) {
	events_ = (zFileEvent*)malloc(sizeof(zFileEvent) * setsize);
	fired_ = (zFiredEvent*)malloc(sizeof(zFiredEvent) * setsize);
	if (events_ == NULL || fired_ == NULL) {
		errmsg_ += "malloc: ";
		errmsg_ += strerror(errno);
		goto err;
	}

	setsize_ = setsize;
	maxfd_ = -1;

	if (zApiCreate() == -1) {
		goto err;
	}

	for (int i = 0; i < setsize; ++i) {
		events_[i].mask = Z_NONE;
	}

	return Z_OK;

err:
	if (!events_) {
		free(events_);
	}
	if (!fired_) {
		free(fired_);
	}

	return Z_ERR;
}

int BaseEvent::resizeSetsize(int setsize) {
	if (setsize == this->setsize_) {
		return Z_OK;
	}
	if (maxfd_ >= setsize) {
		errmsg_ = "fatal error: maxfd >= setsize.";
		return Z_ERR;
	}
	if (zApiResize(setsize) == -1) {
		return Z_ERR;
	}
	void* newevents = realloc(events_, sizeof(zFileEvent) * setsize);
	void* newfired = realloc(fired_, sizeof(zFiredEvent) * setsize);
	if (!newevents || !newfired) {
		errmsg_ = "realloc: ";
		errmsg_ += strerror(errno);
		return Z_ERR;
	}

	events_ = (zFileEvent*)newevents;
	fired_ = (zFiredEvent*)newfired;
	this->setsize_ = setsize;

	for (int i = maxfd_ + 1; i < setsize; ++i) {
		events_[i].mask = Z_NONE;
	}

	return Z_OK;
}

int BaseEvent::CreateFileEvent(int fd, int mask, zFileProc proc,
							   void* clientData) {
	if (fd >= setsize_) {
		errmsg_ = strerror(ERANGE);
		return Z_ERR;
	}
	zFileEvent* fe = &events_[fd];
	if (zApiAddEvent(fd, mask) == -1) {
		return Z_ERR;
	}
	fe->mask |= mask;
	if ((mask & Z_READABLE) || (mask & Z_WRITEABLE)) {
		fe->fileProc = proc;
	}

	fe->clientData = clientData;
	if (fd > maxfd_) {
		maxfd_ = fd;
	}

	return Z_OK;
}

int BaseEvent::DeleteFileEvent(int fd, int mask) {
	if (fd >= setsize_) {
		return Z_OK;
	}
	zFileEvent* fe = &events_[fd];
	if (fe->mask == Z_NONE) {
		return Z_OK;
	}
	if (zApiDelEvent(fd, mask) == -1) {
		return Z_ERR;
	}

	fe->mask = fe->mask & (~mask);
	if (fd == maxfd_ && fe->mask == Z_NONE) {
		// update the max fd
		int j;
		for (j = maxfd_ - 1; j >= 0; --j) {
			if (events_[j].mask != Z_NONE) {
				break;
			}
		}
		maxfd_ = j;
	}
	return Z_OK;
}

int BaseEvent::GetFileEvents(int fd) {
	if (fd >= setsize_) {
		return 0;
	}
	zFileEvent* fe = &events_[fd];
	return fe->mask;
}

// 返回处理是事件数目
int BaseEvent::ProcessEvents(int flags) {
	// Nothing to do? return ASAP
	if (!(flags & Z_TIME_EVENTS) && !(flags & Z_FILE_EVENTS)) {
		return 0;
	}
	int processed = 0;
	if (maxfd_ != -1 || ((flags & Z_TIME_EVENTS) && !(flags & Z_DONT_WAIT))) {
		struct timeval* tvp;
		tvp = NULL;

		int numevents = zApiPoll(tvp);
		for (int j = 0; j < numevents; ++j) {
			zFileEvent* fe = &events_[fired_[j].fd];
			int mask = fired_[j].mask;
			int fd = fired_[j].fd;
			// note the fe->mask & mask & ... code: maybe an already processed
			// event removed an element that fired and we still didn't
			// processed, so we check if the event is still valid.
			if (fe->mask & mask & (Z_READABLE | Z_WRITEABLE)) {
				fe->fileProc(fd, fe->clientData, mask);
			}
			++processed;
		}
	}
	return processed;
}

}  // namespace zevent
}  // namespace utils
