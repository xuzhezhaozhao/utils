/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 27 Sep 2017 02:21:28 PM CST
 @File Name: v2_encoder.hpp
 @Description: from libzmq/src/v2_encoder.hpp
 ******************************************************/

#ifndef UTILS_ZMQ_V2_ENCODER_HPP_
#define UTILS_ZMQ_V2_ENCODER_HPP_

#include "encoder.hpp"

namespace utils {
//  Encoder for 0MQ framing protocol. Converts messages into data stream.

class v2_encoder_t : public encoder_base_t<v2_encoder_t> {
public:
	v2_encoder_t(size_t bufsize_);
	virtual ~v2_encoder_t();

private:
	void size_ready();
	void message_ready();

	unsigned char tmpbuf[9];

	v2_encoder_t(const v2_encoder_t &);
	const v2_encoder_t &operator=(const v2_encoder_t &);
};
}

#endif
