/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 27 Sep 2017 02:03:20 PM CST
 @File Name: v1_encoder.hpp
 @Description: from libzmq/src/v1_encoder.hpp
 ******************************************************/

#ifndef UTILS_ZMQ_V1_ENCODER_HPP_
#define UTILS_ZMQ_V1_ENCODER_HPP_

#include "encoder.hpp"

namespace utils {
//  Encoder for ZMTP/1.0 protocol. Converts messages into data batches.

class v1_encoder_t : public encoder_base_t<v1_encoder_t> {
public:
	v1_encoder_t(size_t bufsize_);
	~v1_encoder_t();

private:
	void size_ready();
	void message_ready();

	unsigned char tmpbuf[10];

	v1_encoder_t(const v1_encoder_t &);
	const v1_encoder_t &operator=(const v1_encoder_t &);
};
}

#endif
