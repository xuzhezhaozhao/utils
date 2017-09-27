/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 27 Sep 2017 01:29:48 PM CST
 @File Name: i_decoder.hpp
 @Description: from libzmq/src/i_decoder.hpp
 ******************************************************/

#ifndef UTILS_ZMQ_I_DECODER_HPP_
#define UTILS_ZMQ_I_DECODER_HPP_

#include <stdint.h>
#include <sys/types.h>

namespace utils {

class msg_t;

//  Interface to be implemented by message decoder.

class i_decoder {
public:
	virtual ~i_decoder() {}

	virtual void get_buffer(unsigned char **data_, size_t *size_) = 0;

	virtual void resize_buffer(size_t) = 0;
	//  Decodes data pointed to by data_.
	//  When a message is decoded, 1 is returned.
	//  When the decoder needs more data, 0 is returned.
	//  On error, -1 is returned and errno is set accordingly.
	virtual int decode(const unsigned char *data_, size_t size_,
					   size_t &processed) = 0;

	virtual msg_t *msg() = 0;
};
}

#endif
