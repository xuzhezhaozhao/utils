/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月22日 星期五 19时57分44秒
 @File Name: signaler.cpp
 @Description: from libzmq/src/signaler.cpp
 ******************************************************/

#include "signaler.hpp"
#include "error/err.h"
#include "ip/ip.hpp"

#include <errno.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Helper to sleep for specific number of milliseconds (or until signal)
static int sleep_ms(unsigned int ms_) {
	if (ms_ == 0) {
		return 0;
	}
	return usleep(ms_ * 1000);
}

// Helper to wait on close(), for non-blocking sockets, until it completes
// If EAGAIN is received, will sleep briefly (1-100ms) then try again, until
// the overall timeout is reached.
//
static int close_wait_ms(int fd_, unsigned int max_ms_ = 2000) {
	unsigned int ms_so_far = 0;
	unsigned int step_ms = max_ms_ / 10;
	if (step_ms < 1) step_ms = 1;
	if (step_ms > 100) step_ms = 100;

	int rc = 0;  // do not sleep on first attempt
	do {
		if (rc == -1 && errno == EAGAIN) {
			sleep_ms(step_ms);
			ms_so_far += step_ms;
		}
		rc = close(fd_);
	} while (ms_so_far < max_ms_ && rc == -1 && errno == EAGAIN);

	return rc;
}

utils::signaler_t::signaler_t() {
	//  Create the socketpair for signaling.
	if (make_fdpair(&r, &w) == 0) {
		unblock_socket(w);
		unblock_socket(r);
	}
}

// This might get run after some part of construction failed, leaving one or
// both of r and w retired_fd.
utils::signaler_t::~signaler_t() {
	if (r == retired_fd) {
		return;
	}
	int rc = close_wait_ms(r);
	errno_assert(rc == 0);
}

int utils::signaler_t::get_fd() const { return r; }

void utils::signaler_t::send() {
	const uint64_t inc = 1;
	ssize_t sz = write(w, &inc, sizeof(inc));
	errno_assert(sz == sizeof(inc));
}

int utils::signaler_t::wait(int timeout_) {
	struct pollfd pfd;
	pfd.fd = r;
	pfd.events = POLLIN;
	int rc = poll(&pfd, 1, timeout_);
	if (unlikely(rc < 0)) {
		errno_assert(errno == EINTR);
		return -1;
	} else if (unlikely(rc == 0)) {
		errno = EAGAIN;
		return -1;
	}
	utils_assert(rc == 1);
	utils_assert(pfd.revents & POLLIN);
	return 0;
}

void utils::signaler_t::recv() {
	//  Attempt to read a signal.
	uint64_t dummy;
	ssize_t sz = read(r, &dummy, sizeof(dummy));
	errno_assert(sz == sizeof(dummy));

	//  If we accidentally grabbed the next signal(s) along with the current
	//  one, return it back to the eventfd object.
	if (unlikely(dummy > 1)) {
		const uint64_t inc = dummy - 1;
		ssize_t sz2 = write(w, &inc, sizeof(inc));
		errno_assert(sz2 == sizeof(inc));
		return;
	}

	utils_assert(dummy == 1);
}

int utils::signaler_t::recv_failable() {
	//  Attempt to read a signal.
	uint64_t dummy;
	ssize_t sz = read(r, &dummy, sizeof(dummy));
	if (sz == -1) {
		errno_assert(errno == EAGAIN);
		return -1;
	} else {
		errno_assert(sz == sizeof(dummy));

		//  If we accidentally grabbed the next signal(s) along with the current
		//  one, return it back to the eventfd object.
		if (unlikely(dummy > 1)) {
			const uint64_t inc = dummy - 1;
			ssize_t sz2 = write(w, &inc, sizeof(inc));
			errno_assert(sz2 == sizeof(inc));
			return 0;
		}

		utils_assert(dummy == 1);
	}
	return 0;
}

//  Returns -1 if we could not make the socket pair successfully
int utils::signaler_t::make_fdpair(int *r_, int *w_) {
	int flags = 0;
	//  Setting this option result in sane behaviour when exec() functions
	//  are used. Old sockets are closed and don't block TCP ports, avoid
	//  leaks, etc.
	flags |= EFD_CLOEXEC;
	int fd = eventfd(0, flags);
	if (fd == -1) {
		errno_assert(errno == ENFILE || errno == EMFILE);
		*w_ = *r_ = -1;
		return -1;
	} else {
		*w_ = *r_ = fd;
		return 0;
	}
}
