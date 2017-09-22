/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月22日 星期五 19时49分38秒
 @File Name: signaler.hpp
 @Description: from libzmq/src/signaler.hpp
 ******************************************************/

#ifndef UTILS_SIGNALER_H_
#define UTILS_SIGNALER_H_

namespace utils {

enum { retired_fd = -1 };

//  This is a cross-platform equivalent to signal_fd. However, as opposed
//  to signal_fd there can be at most one signal in the signaler at any
//  given moment. Attempt to send a signal before receiving the previous
//  one will result in undefined behaviour.

class signaler_t {
public:
	signaler_t();
	~signaler_t();

	int get_fd() const;
	void send();
	int wait(int timeout_);
	void recv();
	int recv_failable();

private:
	//  Creates a pair of file descriptors that will be used
	//  to pass the signals.
	static int make_fdpair(int *r_, int *w_);

	//  Underlying write & read file descriptor
	//  Will be -1 if we exceeded number of available handles
	int w;
	int r;

	//  Disable copying of signaler_t object.
	signaler_t(const signaler_t &);
	const signaler_t &operator=(const signaler_t &);
};
}

#endif
