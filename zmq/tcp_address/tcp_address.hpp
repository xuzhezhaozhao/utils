/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Mon 25 Sep 2017 01:26:54 PM CST
 @File Name: tcp_address.hpp
 @Description: from libzmq/src/tcp_address.hpp
 ******************************************************/

#ifndef UTILS_ZMQ_TCP_ADRESS_H_
#define UTILS_ZMQ_TCP_ADRESS_H_

#include <netinet/in.h>
#include <sys/socket.h>
#include <string>

namespace utils {

class tcp_address_t {
public:
	tcp_address_t();
	tcp_address_t(const sockaddr *sa, socklen_t sa_len);
	virtual ~tcp_address_t();

	//  This function translates textual TCP address into an address
	//  structure. If 'local' is true, names are resolved as local interface
	//  names. If it is false, names are resolved as remote hostnames.
	//  If 'ipv6' is true, the name may resolve to IPv6 address.
	int resolve(const char *name_, bool local_, bool ipv6_,
				bool is_src_ = false);

	//  The opposite to resolve()
	virtual int to_string(std::string &addr_);

	sa_family_t family() const;
	const sockaddr *addr() const;
	socklen_t addrlen() const;

	const sockaddr *src_addr() const;
	socklen_t src_addrlen() const;
	bool has_src_addr() const;

protected:
	int resolve_nic_name(const char *nic_, bool ipv6_, bool is_src_ = false);
	int resolve_interface(const char *interface_, bool ipv6_,
						  bool is_src_ = false);
	int resolve_hostname(const char *hostname_, bool ipv6_,
						 bool is_src_ = false);

	union {
		sockaddr generic;
		sockaddr_in ipv4;
		sockaddr_in6 ipv6;
	} address;

	union {
		sockaddr generic;
		sockaddr_in ipv4;
		sockaddr_in6 ipv6;
	} source_address;
	bool _has_src_addr;
};

class tcp_address_mask_t : public tcp_address_t {
public:
	tcp_address_mask_t();

	// This function enhances tcp_address_t::resolve() with ability to parse
	// additional cidr-like(/xx) mask value at the end of the name string.
	// Works only with remote hostnames.
	int resolve(const char *name_, bool ipv6_);

	// The opposite to resolve()
	int to_string(std::string &addr_);

	int mask() const;

	bool match_address(const struct sockaddr *ss, const socklen_t ss_len) const;

private:
	int address_mask;
};
}

#endif
