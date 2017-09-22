/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月22日 星期五 20时09分49秒
 @File Name: ip.h
 @Description: from libzmq/src/ip.hpp
 ******************************************************/

#ifndef UTILS_IP_H_
#define UTILS_IP_H_

#include <string>

namespace utils {
//  Same as socket(2), but allows for transparent tweaking the options.
int open_socket(int domain_, int type_, int protocol_);

//  Sets the socket into non-blocking mode.
void unblock_socket(int s_);

//  Enable IPv4-mapping of addresses in case it is disabled by default.
void enable_ipv4_mapping(int s_);

//  Returns string representation of peer's address.
//  Socket sockfd_ must be connected. Returns true iff successful.
int get_peer_ip_address(int sockfd_, std::string &ip_addr_);

// Sets the IP Type-Of-Service for the underlying socket
void set_ip_type_of_service(int s_, int iptos);

// Sets the SO_NOSIGPIPE option for the underlying socket.
// Return 0 on success, -1 if the connection has been closed by the peer
int set_nosigpipe(int s_);
}

#endif
