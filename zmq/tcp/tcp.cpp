/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Mon 25 Sep 2017 06:43:44 PM CST
 @File Name: tcp.cpp
 @Description: from libzmq/src/tcp.cpp
 ******************************************************/

#include "tcp.hpp"
#include "error/err.h"
#include "ip/ip.hpp"
#include "macros/macros.h"

#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>

void utils::tune_tcp_socket(int s_) {
	//  Disable Nagle's algorithm. We are doing data batching on 0MQ level,
	//  so using Nagle wouldn't improve throughput in anyway, but it would
	//  hurt latency.
	int nodelay = 1;
	int rc =
		setsockopt(s_, IPPROTO_TCP, TCP_NODELAY, (char *)&nodelay, sizeof(int));
	errno_assert(rc == 0);
}

void utils::set_tcp_send_buffer(int sockfd_, int bufsize_) {
	const int rc = setsockopt(sockfd_, SOL_SOCKET, SO_SNDBUF, (char *)&bufsize_,
							  sizeof bufsize_);
	errno_assert(rc == 0);
}

void utils::set_tcp_receive_buffer(int sockfd_, int bufsize_) {
	const int rc = setsockopt(sockfd_, SOL_SOCKET, SO_RCVBUF, (char *)&bufsize_,
							  sizeof bufsize_);
	errno_assert(rc == 0);
}

void utils::tune_tcp_keepalives(int s_, int keepalive_, int keepalive_cnt_,
								int keepalive_idle_, int keepalive_intvl_) {
	// These options are used only under certain #ifdefs below.
	UNUSED(keepalive_);
	UNUSED(keepalive_cnt_);
	UNUSED(keepalive_idle_);
	UNUSED(keepalive_intvl_);

	// If none of the #ifdefs apply, then s_ is unused.
	UNUSED(s_);

	//  Tuning TCP keep-alives if platform allows it
	//  All values = -1 means skip and leave it for OS

	if (keepalive_ != -1) {
		int rc = setsockopt(s_, SOL_SOCKET, SO_KEEPALIVE, (char *)&keepalive_,
							sizeof(int));
		errno_assert(rc == 0);

		if (keepalive_cnt_ != -1) {
			int rc = setsockopt(s_, IPPROTO_TCP, TCP_KEEPCNT, &keepalive_cnt_,
								sizeof(int));
			errno_assert(rc == 0);
		}

		if (keepalive_idle_ != -1) {
			int rc = setsockopt(s_, IPPROTO_TCP, TCP_KEEPIDLE, &keepalive_idle_,
								sizeof(int));
			errno_assert(rc == 0);
		}

		if (keepalive_intvl_ != -1) {
			int rc = setsockopt(s_, IPPROTO_TCP, TCP_KEEPINTVL,
								&keepalive_intvl_, sizeof(int));
			errno_assert(rc == 0);
		}
	}
}

void utils::tune_tcp_maxrt(int sockfd_, int timeout_) {
	if (timeout_ <= 0) return;

	UNUSED(sockfd_);

	int rc = setsockopt(sockfd_, IPPROTO_TCP, TCP_USER_TIMEOUT, &timeout_,
						sizeof(timeout_));
	errno_assert(rc == 0);
}

int utils::tcp_write(int s_, const void *data_, size_t size_) {
	ssize_t nbytes = send(s_, data_, size_, 0);

	//  Several errors are OK. When speculative write is being done we may not
	//  be able to write a single byte from the socket. Also, SIGSTOP issued
	//  by a debugging tool can result in EINTR error.
	if (nbytes == -1 &&
		(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
		return 0;

	//  Signalise peer failure.
	if (nbytes == -1) {
		errno_assert(errno != EACCES && errno != EBADF &&
					 errno != EDESTADDRREQ && errno != EFAULT &&
					 errno != EISCONN && errno != EMSGSIZE && errno != ENOMEM &&
					 errno != ENOTSOCK && errno != EOPNOTSUPP);
		return -1;
	}

	return static_cast<int>(nbytes);
}

int utils::tcp_read(int s_, void *data_, size_t size_) {
	const ssize_t rc = recv(s_, data_, size_, 0);

	//  Several errors are OK. When speculative read is being done we may not
	//  be able to read a single byte from the socket. Also, SIGSTOP issued
	//  by a debugging tool can result in EINTR error.
	if (rc == -1) {
		errno_assert(errno != EBADF && errno != EFAULT && errno != ENOMEM &&
					 errno != ENOTSOCK);
		if (errno == EWOULDBLOCK || errno == EINTR) errno = EAGAIN;
	}

	return static_cast<int>(rc);
}
