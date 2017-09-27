/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 27 Sep 2017 10:52:10 AM CST
 @File Name: i_encoder.hpp
 @Description: from libzmq/src/i_encoder.hpp
 ******************************************************/

#ifndef UTILS_ENCODER_I_ENCODER_HPP_
#define UTILS_ENCODER_I_ENCODER_HPP_

#include <stdint.h>
#include <sys/types.h>

namespace utils {

//  Forward declaration
class msg_t;

//  Interface to be implemented by message encoder.

struct i_encoder {
	virtual ~i_encoder() {}

	//  The function returns a batch of binary data. The data
	//  are filled to a supplied buffer. If no buffer is supplied (data_
	//  is NULL) encoder will provide buffer of its own.
	//  Function returns 0 when a new message is required.
	virtual size_t encode(unsigned char **data_, size_t size) = 0;

	//  Load a new message into encoder.
	virtual void load_msg(msg_t *msg_) = 0;
};
}

#endif
