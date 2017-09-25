/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Mon 25 Sep 2017 01:28:58 PM CST
 @File Name: tcp_address.cpp
 @Description: from libzmq/src/tcp_address.cpp
 ******************************************************/

#include "tcp_address.hpp"
#include "error/err.h"
#include "ip/ip.hpp"
#include "macros/macros.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <sstream>
#include <string>

//  On linux, network interface name can be queried using getifaddrs function.
int utils::tcp_address_t::resolve_nic_name(const char *nic_, bool ipv6_,
										   bool is_src_) {
	//  Get the addresses.
	ifaddrs *ifa = NULL;
	int rc = 0;
	const int max_attempts = 10;
	const int backoff_msec = 1;
	for (int i = 0; i < max_attempts; i++) {
		rc = getifaddrs(&ifa);
		if (rc == 0 || (rc < 0 && errno != ECONNREFUSED)) break;
		usleep((backoff_msec << i) * 1000);
	}

	if (rc != 0 && ((errno == EINVAL) || (errno == EOPNOTSUPP))) {
		// Windows Subsystem for Linux compatibility
		UNUSED(nic_);
		UNUSED(ipv6_);

		errno = ENODEV;
		return -1;
	}
	errno_assert(rc == 0);
	utils_assert(ifa != NULL);

	//  Find the corresponding network interface.
	bool found = false;
	for (ifaddrs *ifp = ifa; ifp != NULL; ifp = ifp->ifa_next) {
		if (ifp->ifa_addr == NULL) continue;

		const int family = ifp->ifa_addr->sa_family;
		if ((family == AF_INET || (ipv6_ && family == AF_INET6)) &&
			!strcmp(nic_, ifp->ifa_name)) {
			if (is_src_)
				memcpy(&source_address, ifp->ifa_addr, (family == AF_INET)
						   ? sizeof(struct sockaddr_in)
						   : sizeof(struct sockaddr_in6));
			else
				memcpy(&address, ifp->ifa_addr, (family == AF_INET)
						   ? sizeof(struct sockaddr_in)
						   : sizeof(struct sockaddr_in6));
			found = true;
			break;
		}
	}

	//  Clean-up;
	freeifaddrs(ifa);

	if (!found) {
		errno = ENODEV;
		return -1;
	}
	return 0;
}

int utils::tcp_address_t::resolve_interface(const char *interface_, bool ipv6_,
											bool is_src_) {
	//  Initialize temporary output pointers with storage address.
	sockaddr_storage ss;
	sockaddr *out_addr = (sockaddr *)&ss;
	size_t out_addrlen;

	//  Initialise IP-format family/port and populate temporary output pointers
	//  with the address.
	if (ipv6_) {
		sockaddr_in6 ip6_addr;
		memset(&ip6_addr, 0, sizeof(ip6_addr));
		ip6_addr.sin6_family = AF_INET6;
		memcpy(&ip6_addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
		out_addrlen = sizeof(ip6_addr);
		memcpy(out_addr, &ip6_addr, out_addrlen);
	} else {
		sockaddr_in ip4_addr;
		memset(&ip4_addr, 0, sizeof(ip4_addr));
		ip4_addr.sin_family = AF_INET;
		ip4_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		out_addrlen = sizeof(ip4_addr);
		memcpy(out_addr, &ip4_addr, out_addrlen);
	}
	//  "*" resolves to INADDR_ANY or in6addr_any.
	if (strcmp(interface_, "*") == 0) {
		utils_assert(out_addrlen <= sizeof(address));
		if (is_src_)
			memcpy(&source_address, out_addr, out_addrlen);
		else
			memcpy(&address, out_addr, out_addrlen);
		return 0;
	}

	//  Try to resolve the string as a NIC name.
	int rc = resolve_nic_name(interface_, ipv6_, is_src_);
	if (rc == 0 || errno != ENODEV) return rc;

	//  There's no such interface name. Assume literal address.
	addrinfo *res = NULL;
	addrinfo req;
	memset(&req, 0, sizeof(req));

	//  Choose IPv4 or IPv6 protocol family. Note that IPv6 allows for
	//  IPv4-in-IPv6 addresses.
	req.ai_family = ipv6_ ? AF_INET6 : AF_INET;

	//  Arbitrary, not used in the output, but avoids duplicate results.
	req.ai_socktype = SOCK_STREAM;

	//  Restrict hostname/service to literals to avoid any DNS lookups or
	//  service-name irregularity due to indeterminate socktype.
	req.ai_flags = AI_PASSIVE | AI_NUMERICHOST;

#if defined AI_V4MAPPED
	//  In this API we only require IPv4-mapped addresses when
	//  no native IPv6 interfaces are available (~AI_ALL).
	//  This saves an additional DNS roundtrip for IPv4 addresses.
	//  Note: While the AI_V4MAPPED flag is defined on FreeBSD system,
	//  it is not supported here. See libzmq issue #331.
	if (req.ai_family == AF_INET6) req.ai_flags |= AI_V4MAPPED;
#endif

	//  Resolve the literal address. Some of the error info is lost in case
	//  of error, however, there's no way to report EAI errors via errno.

	rc = getaddrinfo(interface_, NULL, &req, &res);

	if (rc) {
		errno = ENODEV;
		return -1;
	}

	//  Use the first result.
	utils_assert(res != NULL);
	utils_assert((size_t)res->ai_addrlen <= sizeof(address));
	if (is_src_)
		memcpy(&source_address, res->ai_addr, res->ai_addrlen);
	else
		memcpy(&address, res->ai_addr, res->ai_addrlen);

	//  Cleanup getaddrinfo after copying the possibly referenced result.
	freeaddrinfo(res);

	return 0;
}

int utils::tcp_address_t::resolve_hostname(const char *hostname_, bool ipv6_,
										   bool is_src_) {
	//  Set up the query.
	addrinfo req;
	memset(&req, 0, sizeof(req));

	//  Choose IPv4 or IPv6 protocol family. Note that IPv6 allows for
	//  IPv4-in-IPv6 addresses.
	req.ai_family = ipv6_ ? AF_INET6 : AF_INET;

	//  Need to choose one to avoid duplicate results from getaddrinfo() - this
	//  doesn't really matter, since it's not included in the addr-output.
	req.ai_socktype = SOCK_STREAM;

#if defined AI_V4MAPPED
	//  In this API we only require IPv4-mapped addresses when
	//  no native IPv6 interfaces are available.
	//  This saves an additional DNS roundtrip for IPv4 addresses.
	//  Note: While the AI_V4MAPPED flag is defined on FreeBSD system,
	//  it is not supported here. See libzmq issue #331.
	if (req.ai_family == AF_INET6) req.ai_flags |= AI_V4MAPPED;
#endif

	//  Resolve host name. Some of the error info is lost in case of error,
	//  however, there's no way to report EAI errors via errno.
	addrinfo *res;

	const int rc = getaddrinfo(hostname_, NULL, &req, &res);
	if (rc) {
		switch (rc) {
			case EAI_MEMORY:
				errno = ENOMEM;
				break;
			default:
				errno = EINVAL;
				break;
		}
		return -1;
	}

	//  Copy first result to output addr with hostname and service.
	utils_assert((size_t)res->ai_addrlen <= sizeof(address));
	if (is_src_)
		memcpy(&source_address, res->ai_addr, res->ai_addrlen);
	else
		memcpy(&address, res->ai_addr, res->ai_addrlen);

	freeaddrinfo(res);

	return 0;
}

utils::tcp_address_t::tcp_address_t() : _has_src_addr(false) {
	memset(&address, 0, sizeof(address));
	memset(&source_address, 0, sizeof(source_address));
}

utils::tcp_address_t::tcp_address_t(const sockaddr *sa, socklen_t sa_len)
	: _has_src_addr(false) {
	utils_assert(sa && sa_len > 0);

	memset(&address, 0, sizeof(address));
	memset(&source_address, 0, sizeof(source_address));
	if (sa->sa_family == AF_INET && sa_len >= (socklen_t)sizeof(address.ipv4))
		memcpy(&address.ipv4, sa, sizeof(address.ipv4));
	else if (sa->sa_family == AF_INET6 &&
			 sa_len >= (socklen_t)sizeof(address.ipv6))
		memcpy(&address.ipv6, sa, sizeof(address.ipv6));
}

utils::tcp_address_t::~tcp_address_t() {}

int utils::tcp_address_t::resolve(const char *name_, bool local_, bool ipv6_,
								  bool is_src_) {
	if (!is_src_) {
		// Test the ';' to know if we have a source address in name_
		const char *src_delimiter = strrchr(name_, ';');
		if (src_delimiter) {
			std::string src_name(name_, src_delimiter - name_);
			const int rc = resolve(src_name.c_str(), local_, ipv6_, true);
			if (rc != 0) return -1;
			name_ = src_delimiter + 1;
			_has_src_addr = true;
		}
	}

	//  Find the ':' at end that separates address from the port number.
	const char *delimiter = strrchr(name_, ':');
	if (!delimiter) {
		errno = EINVAL;
		return -1;
	}

	//  Separate the address/port.
	std::string addr_str(name_, delimiter - name_);
	std::string port_str(delimiter + 1);

	//  Remove square brackets around the address, if any, as used in IPv6
	if (addr_str.size() >= 2 && addr_str[0] == '[' &&
		addr_str[addr_str.size() - 1] == ']')
		addr_str = addr_str.substr(1, addr_str.size() - 2);

	// Test the '%' to know if we have an interface name / zone_id in the
	// address
	// Reference: https://tools.ietf.org/html/rfc4007
	std::size_t pos = addr_str.rfind('%');
	uint32_t zone_id = 0;
	if (pos != std::string::npos) {
		std::string if_str = addr_str.substr(pos + 1);
		addr_str = addr_str.substr(0, pos);
		if (isalpha(if_str.at(0)))
			zone_id = if_nametoindex(if_str.c_str());
		else
			zone_id = (uint32_t)atoi(if_str.c_str());
		if (zone_id == 0) {
			errno = EINVAL;
			return -1;
		}
	}

	//  Allow 0 specifically, to detect invalid port error in atoi if not
	uint16_t port;
	if (port_str == "*" || port_str == "0")
		//  Resolve wildcard to 0 to allow autoselection of port
		port = 0;
	else {
		//  Parse the port number (0 is not a valid port).
		port = (uint16_t)atoi(port_str.c_str());
		if (port == 0) {
			errno = EINVAL;
			return -1;
		}
	}

	//  Resolve the IP address.
	int rc;
	if (local_ || is_src_)
		rc = resolve_interface(addr_str.c_str(), ipv6_, is_src_);
	else
		rc = resolve_hostname(addr_str.c_str(), ipv6_, is_src_);
	if (rc != 0) return -1;

	//  Set the port into the address structure.
	if (is_src_) {
		if (source_address.generic.sa_family == AF_INET6) {
			source_address.ipv6.sin6_port = htons(port);
			source_address.ipv6.sin6_scope_id = zone_id;
		} else
			source_address.ipv4.sin_port = htons(port);
	} else {
		if (address.generic.sa_family == AF_INET6) {
			address.ipv6.sin6_port = htons(port);
			address.ipv6.sin6_scope_id = zone_id;
		} else
			address.ipv4.sin_port = htons(port);
	}

	return 0;
}

int utils::tcp_address_t::to_string(std::string &addr_) {
	if (address.generic.sa_family != AF_INET &&
		address.generic.sa_family != AF_INET6) {
		addr_.clear();
		return -1;
	}

	//  Not using service resolving because of
	//  https://github.com/zeromq/libzmq/commit/1824574f9b5a8ce786853320e3ea09fe1f822bc4
	char hbuf[NI_MAXHOST];
	int rc = getnameinfo(addr(), addrlen(), hbuf, sizeof(hbuf), NULL, 0,
						 NI_NUMERICHOST);
	if (rc != 0) {
		addr_.clear();
		return rc;
	}

	if (address.generic.sa_family == AF_INET6) {
		std::stringstream s;
		s << "tcp://[" << hbuf << "]:" << ntohs(address.ipv6.sin6_port);
		addr_ = s.str();
	} else {
		std::stringstream s;
		s << "tcp://" << hbuf << ":" << ntohs(address.ipv4.sin_port);
		addr_ = s.str();
	}
	return 0;
}

const sockaddr *utils::tcp_address_t::addr() const { return &address.generic; }

socklen_t utils::tcp_address_t::addrlen() const {
	if (address.generic.sa_family == AF_INET6)
		return (socklen_t)sizeof(address.ipv6);
	else
		return (socklen_t)sizeof(address.ipv4);
}

const sockaddr *utils::tcp_address_t::src_addr() const {
	return &source_address.generic;
}

socklen_t utils::tcp_address_t::src_addrlen() const {
	if (address.generic.sa_family == AF_INET6)
		return (socklen_t)sizeof(source_address.ipv6);
	else
		return (socklen_t)sizeof(source_address.ipv4);
}

bool utils::tcp_address_t::has_src_addr() const { return _has_src_addr; }

sa_family_t utils::tcp_address_t::family() const {
	return address.generic.sa_family;
}

utils::tcp_address_mask_t::tcp_address_mask_t()
	: tcp_address_t(), address_mask(-1) {}

int utils::tcp_address_mask_t::mask() const { return address_mask; }

int utils::tcp_address_mask_t::resolve(const char *name_, bool ipv6_) {
	// Find '/' at the end that separates address from the cidr mask number.
	// Allow empty mask clause and treat it like '/32' for ipv4 or '/128' for
	// ipv6.
	std::string addr_str, mask_str;
	const char *delimiter = strrchr(name_, '/');
	if (delimiter != NULL) {
		addr_str.assign(name_, delimiter - name_);
		mask_str.assign(delimiter + 1);
		if (mask_str.empty()) {
			errno = EINVAL;
			return -1;
		}
	} else
		addr_str.assign(name_);

	// Parse address part using standard routines.
	const int rc = tcp_address_t::resolve_hostname(addr_str.c_str(), ipv6_);
	if (rc != 0) return rc;

	// Parse the cidr mask number.
	if (mask_str.empty()) {
		if (address.generic.sa_family == AF_INET6)
			address_mask = 128;
		else
			address_mask = 32;
	} else if (mask_str == "0")
		address_mask = 0;
	else {
		const int mask = atoi(mask_str.c_str());
		if ((mask < 1) ||
			(address.generic.sa_family == AF_INET6 && mask > 128) ||
			(address.generic.sa_family != AF_INET6 && mask > 32)) {
			errno = EINVAL;
			return -1;
		}
		address_mask = mask;
	}

	return 0;
}

int utils::tcp_address_mask_t::to_string(std::string &addr_) {
	if (address.generic.sa_family != AF_INET &&
		address.generic.sa_family != AF_INET6) {
		addr_.clear();
		return -1;
	}
	if (address_mask == -1) {
		addr_.clear();
		return -1;
	}

	char hbuf[NI_MAXHOST];
	int rc = getnameinfo(addr(), addrlen(), hbuf, sizeof(hbuf), NULL, 0,
						 NI_NUMERICHOST);
	if (rc != 0) {
		addr_.clear();
		return rc;
	}

	if (address.generic.sa_family == AF_INET6) {
		std::stringstream s;
		s << "[" << hbuf << "]/" << address_mask;
		addr_ = s.str();
	} else {
		std::stringstream s;
		s << hbuf << "/" << address_mask;
		addr_ = s.str();
	}
	return 0;
}

bool utils::tcp_address_mask_t::match_address(const struct sockaddr *ss,
											  const socklen_t ss_len) const {
	utils_assert(address_mask != -1 && ss != NULL &&
				 ss_len >= (socklen_t)sizeof(struct sockaddr));

	if (ss->sa_family != address.generic.sa_family) return false;

	if (address_mask > 0) {
		int mask;
		const uint8_t *our_bytes, *their_bytes;
		if (ss->sa_family == AF_INET6) {
			utils_assert(ss_len == sizeof(struct sockaddr_in6));
			their_bytes = (const uint8_t *)&(
				((const struct sockaddr_in6 *)ss)->sin6_addr);
			our_bytes = (const uint8_t *)&address.ipv6.sin6_addr;
			mask = sizeof(struct in6_addr) * 8;
		} else {
			utils_assert(ss_len == sizeof(struct sockaddr_in));
			their_bytes =
				(const uint8_t *)&(((const struct sockaddr_in *)ss)->sin_addr);
			our_bytes = (const uint8_t *)&address.ipv4.sin_addr;
			mask = sizeof(struct in_addr) * 8;
		}
		if (address_mask < mask) mask = address_mask;

		const size_t full_bytes = mask / 8;
		if (memcmp(our_bytes, their_bytes, full_bytes)) return false;

		const uint8_t last_byte_bits = (uint8_t)(0xffU << (8 - mask % 8));
		if (last_byte_bits) {
			if ((their_bytes[full_bytes] & last_byte_bits) !=
				(our_bytes[full_bytes] & last_byte_bits))
				return false;
		}
	}

	return true;
}
