/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 27 Sep 2017 11:43:14 AM CST
 @File Name: raw_encoder.hpp
 @Description: libzmq/src/raw_encoder.hpp
 ******************************************************/

#ifndef UTILS_ENCODER_RAW_ENCODER_HPP_
#define UTILS_ENCODER_RAW_ENCODER_HPP_

#include "encoder.hpp"
#include "error/err.h"
#include "i_encoder.hpp"
#include "msg/msg.hpp"

namespace utils {

//  Encoder for 0MQ framing protocol. Converts messages into data batches.

class raw_encoder_t : public encoder_base_t<raw_encoder_t> {
public:
	raw_encoder_t(size_t bufsize_);
	~raw_encoder_t();

private:
	void raw_message_ready();

	raw_encoder_t(const raw_encoder_t &);
	const raw_encoder_t &operator=(const raw_encoder_t &);
};
}
#endif
