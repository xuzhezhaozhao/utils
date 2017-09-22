/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月22日 星期五 20时15分01秒
 @File Name: ip.cpp
 @Description: from libzmq/src/ip.cpp
 ******************************************************/

#include "ip.h"
#include "error/err.h"
#include "macros/macros.h"

#include <fcntl.h>
#include <sys/socket.h>

int utils::open_socket(int domain_, int type_, int protocol_) {
	int rc;

	//  Setting this option result in sane behaviour when exec() functions
	//  are used. Old sockets are closed and don't block TCP ports etc.
	type_ |= SOCK_CLOEXEC;

	int s = socket(domain_, type_, protocol_);
	if (s == -1) {
		return -1;
	}

	//  Socket is not yet connected so EINVAL is not a valid networking error
	rc = utils::set_nosigpipe(s);
	errno_assert(rc == 0);

	return s;
}

void utils::unblock_socket(int s_) {
	int flags = fcntl(s_, F_GETFL, 0);
	if (flags == -1) {
		flags = 0;
	}
	int rc = fcntl(s_, F_SETFL, flags | O_NONBLOCK);
	errno_assert(rc != -1);
}

void utils::enable_ipv4_mapping(int s_) {
	UNUSED(s_);

#ifdef IPV6_V6ONLY
	int flag = 0;
	int rc = setsockopt(s_, IPPROTO_IPV6, IPV6_V6ONLY, (const char *)&flag,
						sizeof(flag));
	errno_assert(rc == 0);
#endif
}

int utils::get_peer_ip_address(int sockfd_, std::string &ip_addr_) {
	int rc;
	struct sockaddr_storage ss;

	socklen_t addrlen = sizeof(ss);
	rc = getpeername(sockfd_, (struct sockaddr *)&ss, &addrlen);

	if (rc == -1) {
		errno_assert(errno != EBADF && errno != EFAULT && errno != ENOTSOCK);
		return 0;
	}

	char host[NI_MAXHOST];
	rc = getnameinfo((struct sockaddr *)&ss, addrlen, host, sizeof(host), NULL,
					 0, NI_NUMERICHOST);
	if (rc != 0) return 0;

	ip_addr_ = host;

	union {
		struct sockaddr sa;
		struct sockaddr_storage sa_stor;
	} u;

	u.sa_stor = ss;
	return (int)u.sa.sa_family;
}

void utils::set_ip_type_of_service(int s_, int iptos) {
	int rc = setsockopt(s_, IPPROTO_IP, IP_TOS,
						reinterpret_cast<const char *>(&iptos), sizeof(iptos));

	errno_assert(rc == 0);

	rc = setsockopt(s_, IPPROTO_IPV6, IPV6_TCLASS,
					reinterpret_cast<const char *>(&iptos), sizeof(iptos));

	//  If IPv6 is not enabled ENOPROTOOPT will be returned on Linux and
	//  EINVAL on OSX
	if (rc == -1) {
		errno_assert(errno == ENOPROTOOPT || errno == EINVAL);
	}
}

int utils::set_nosigpipe(int s_) {
#ifdef SO_NOSIGPIPE
	//  Make sure that SIGPIPE signal is not generated when writing to a
	//  connection that was already closed by the peer.
	//  As per POSIX spec, EINVAL will be returned if the socket was valid but
	//  the connection has been reset by the peer. Return an error so that the
	//  socket can be closed and the connection retried if necessary.
	int set = 1;
	int rc = setsockopt(s_, SOL_SOCKET, SO_NOSIGPIPE, &set, sizeof(int));
	if (rc != 0 && errno == EINVAL) return -1;
	errno_assert(rc == 0);
#else
	UNUSED(s_);
#endif

	return 0;
}
