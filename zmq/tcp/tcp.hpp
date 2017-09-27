/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Mon 25 Sep 2017 06:41:27 PM CST
 @File Name: tcp.hpp
 @Description: from libzmq/src/tcp.hpp
 ******************************************************/

#ifndef UTILS_ZMQ_TCP_H_
#define UTILS_ZMQ_TCP_H_

#include <sys/types.h>  // for size_t

namespace utils {

//  Tunes the supplied TCP socket for the best latency.
void tune_tcp_socket(int s_);

//  Sets the socket send buffer size.
void set_tcp_send_buffer(int sockfd_, int bufsize_);

//  Sets the socket receive buffer size.
void set_tcp_receive_buffer(int sockfd_, int bufsize_);

//  Tunes TCP keep-alives
void tune_tcp_keepalives(int s_, int keepalive_, int keepalive_cnt_,
						 int keepalive_idle_, int keepalive_intvl_);

//  Tunes TCP max retransmit timeout
void tune_tcp_maxrt(int sockfd_, int timeout_);

//  Writes data to the socket. Returns the number of bytes actually
//  written (even zero is to be considered to be a success). In case
//  of error or orderly shutdown by the other peer -1 is returned.
int tcp_write(int s_, const void *data_, size_t size_);

//  Reads data from the socket (up to 'size' bytes).
//  Returns the number of bytes actually read or -1 on error.
//  Zero indicates the peer has closed the connection.
int tcp_read(int s_, void *data_, size_t size_);
}

#endif
